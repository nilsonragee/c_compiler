#include <stdlib.h> // malloc(), realloc(), free()
#include <string.h> // memset(), memcpy()

#include "allocator.h"
#include "common.h"

// Disable warning complaining about different pointer type function parameters:
// Function pointer type declaration:
//   typedef u8 * ( *PFN_Allocator_allocate )( struct Allocator *allocator, u64 count, u64 size, Caller_Info caller );
// Assigned function:
//   u8 * system_allocator_allocate( System_Allocator *allocator, u64 count, u64 size, Caller_Info caller );
//   // Allocator * <-> System_Allocator *
// Expression:
//   .vtable.allocate = system_allocator_allocate;
#pragma warning( push )
#pragma warning( disable : 4028 )  // warning C4028: formal parameter 1 different from declaration

static bool power_of_2( u64 x ) {
	// u64 x   = 4
	//     x   = 0x100
	//     x-1 = 0x011
	// x & x-1 = 0x000
	//         => x (4) is power of 2.
	return ( x & ( x - 1 ) ) == 0;
}

static System_Allocator g_c_compiler_sys_allocator;
Allocator *g_sys_allocator = NULL;

/* Module initialization */

bool allocators_init( void ) {
	if ( !g_sys_allocator ) {
		g_c_compiler_sys_allocator = system_allocator_create();
		g_sys_allocator = ( Allocator * )&g_c_compiler_sys_allocator;
		return true;
	}

	return false;
};

void allocators_shutdown( void ) {
	g_sys_allocator = NULL;
};

/* Allocator */

Allocator allocator_create(
	PFN_Allocator_allocate pfn_allocate,
	PFN_Allocator_reallocate pfn_reallocate,
	PFN_Allocator_deallocate pfn_deallocate
) {
	Allocator allocator = ( Allocator ) {
		.allocate = pfn_allocate,
		.reallocate = pfn_reallocate,
		.deallocate = pfn_deallocate
	};

	return allocator;
}

u8 *allocator_request_allocate( Allocator *allocator, u64 count, u64 size, Caller_Info caller ) {
	u8 *allocated = allocator->allocate( allocator, count, size, caller );
	AssertMessage( allocated, "Failed to allocate" );
	return allocated;
}

u8 *allocator_request_reallocate( Allocator *allocator, void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller ) {
	u8 *reallocated = allocator->reallocate( allocator, memory_pointer, old_count, new_count, size, caller );
	AssertMessage( reallocated, "Failed to reallocate" );
	return reallocated;
}

void allocator_request_deallocate( Allocator *allocator, void *memory_pointer, Caller_Info caller ) {
	if ( memory_pointer )
		allocator->deallocate( allocator, memory_pointer, caller );
}

/* System_Allocator */

System_Allocator system_allocator_create( void ) {
	System_Allocator allocator = ( System_Allocator ) {
		.vtable = ( Allocator ) {
			.allocate = system_allocator_allocate,
			.reallocate = system_allocator_reallocate,
			.deallocate = system_allocator_deallocate
		}
	};

	return allocator;
}

u8 *system_allocator_allocate( System_Allocator *allocator, u64 count, u64 size, Caller_Info caller) {
	return ( u8 * ) malloc( count * size );
}

u8 *system_allocator_reallocate( System_Allocator *allocator, void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller ) {
	return ( u8 * ) realloc( memory_pointer, new_count * size );
	// u8 *allocated = do_allocate(new_count, size, caller);
	// memcpy(allocated, memory_pointer, old_count * size);
	// do_free(memory_pointer, caller);
	// return allocated;
}

void system_allocator_deallocate( System_Allocator *allocator, void *memory_pointer, Caller_Info caller ) {
	free( memory_pointer );
}

/* Linear_Allocator */

Linear_Allocator linear_allocator_create( void ) {
	Linear_Allocator allocator = ( Linear_Allocator ) {
		.vtable = ( Allocator ) {
			.allocate = linear_allocator_allocate,
			.reallocate = linear_allocator_reallocate,
			.deallocate = linear_allocator_deallocate
		},
		.memory_start = NULL,
		.memory_end = NULL,
		.cursor = NULL,
		.cursor_max = NULL,
		.allocated = 0
	};

	return allocator;
}

u8 *linear_allocator_allocate( Linear_Allocator *allocator, u64 count, u64 size, Caller_Info caller ) {
	AssertMessage( power_of_2( size ), "Allocation size (alignment) must be power of 2" );
	u8 *aligned = ( u8 * )( ( ( u64 )( allocator->cursor + size - 1 ) ) & ~( size - 1 ) );
	if ( aligned + count > allocator->memory_end )  return NULL;

	allocator->cursor = aligned + count;
	allocator->cursor_max = max( allocator->cursor, allocator->cursor_max );
	return aligned;
}

u8 *linear_allocator_reallocate( Linear_Allocator *allocator, void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller ) {
	if ( !memory_pointer )  return linear_allocator_allocate( allocator, new_count, size, caller ); // Note: Bypasses vtable.

	if ( ( memory_pointer == allocator->cursor - old_count ) && ( ( u64 )memory_pointer % size == 0 ) ) {
		u8 *new_cursor = allocator->cursor - old_count + new_count;

		if ( new_cursor > allocator->memory_end )  return NULL;
		AssertMessage( new_cursor < allocator->cursor, "New cursor is below the current cursor for some reason" );

		allocator->cursor = new_cursor;
		allocator->cursor_max = max( allocator->cursor, allocator->cursor_max );
		return ( u8 * )memory_pointer;
	}

	u8 *new_memory_pointer = linear_allocator_allocate( allocator, new_count, size, caller); // Note: Bypasses vtable.
	if ( !new_memory_pointer )  return NULL;

	memcpy( new_memory_pointer, memory_pointer, old_count );
	return new_memory_pointer;
}

void linear_allocator_deallocate( Linear_Allocator *allocator, void *memory_pointer, Caller_Info caller ) {
	if ( !memory_pointer ) {
		linear_allocator_reset( allocator, false );
		return;
	}

	AssertMessage( false, "Trying to deallocate with 'Linear_Allocator'" );
}

void linear_allocator_init( Linear_Allocator *allocator, void *memory_pointer, u64 size ) {
	AssertMessage( memory_pointer, "Trying to initialize 'Linear_Allocator' with NULL pointer" );

	allocator->memory_start = ( u8 * )memory_pointer;
	allocator->memory_end = allocator->memory_start + size;
	allocator->cursor = allocator->memory_start;
	allocator->cursor_max = allocator->cursor;

	allocator->allocated = size;
}

void linear_allocator_deinit( Linear_Allocator *allocator ) {
	AssertMessage( allocator->memory_start, "Allocator is not initialized or already deinitialized" );
	free( allocator->memory_start );

	allocator->memory_start = NULL;
	allocator->memory_end = NULL;
}

void linear_allocator_reset( Linear_Allocator *allocator, bool zero_memory ) {
	AssertMessage( allocator->memory_start, "Allocator is not initialized or already deinitialized" );
	allocator->cursor = allocator->memory_start;
	// maybe keep max usage info on clear?
	// cursor_max = cursor;

	if ( zero_memory )  memset( allocator->memory_start, 0, allocator->allocated * sizeof( u8 ) );
}

u64 linear_allocator_occupied( Linear_Allocator *allocator ) {
	u64 occupied = allocator->cursor - allocator->memory_start;
	return occupied;
}

#pragma warning( pop )  // Revert C4028 supression.
