#ifndef C_COMPILER__ALLOCATOR_H
#define C_COMPILER__ALLOCATOR_H

#include "platform.h"

#if defined( C_COMPILER_PLATFORM_WINDOWS )
	#define CC_AllocatorEmptyCaller()      ( Caller_Info ) { "(none)", __FUNCSIG__, __FILE__, __LINE__ }
	#define CC_AllocatorTypeCaller( T )    ( Caller_Info ) { #T, __FUNCSIG__, __FILE__, __LINE__ }
#elif defined( C_COMPILER_PLATFORM_LINUX )
	#define CC_AllocatorEmptyCaller()      ( Caller_Info ) { "(none)", __PRETTY_FUNCTION__, __FILE__, __LINE__ }
	#define CC_AllocatorTypeCaller( T )    ( Caller_Info ) { #T, __PRETTY_FUNCTION__, __FILE__, __LINE__ }
#endif

/*
	Base allocator functions macros.
	These are designed to be wrapped around with another macro.
	Avoid using these macros directly.
*/

#define CC_RequestAllocate( allocator, count, size, caller ) \
	allocator_request_allocate( \
		allocator, \
		count, \
		size, \
		caller \
	)

#define CC_RequestReallocate( allocator, memory_pointer, old_count, new_count, size, caller ) \
	allocator_request_reallocate( \
		allocator, \
		memory_pointer, \
		old_count, \
		new_count, \
		size, \
		caller \
	)

#define CC_RequestDeallocate( allocator, memory_pointer, caller ) \
	allocator_request_deallocate( \
		allocator, \
		memory_pointer, \
		caller \
	)

/*
	Macro wrappings around base allocator functions macros.
	These are designed to be used in source code.
	Use these macros to work with allocators.
*/

#define CC_Allocate( allocator, count, T ) \
	( T * ) CC_RequestAllocate( allocator, count, sizeof( T ), CC_AllocatorTypeCaller( T ) )

#define CC_Reallocate( allocator, memory_pointer, old_count, new_count, T ) \
	( T * ) CC_RequestReallocate( allocator, memory_pointer, old_count, new_count, sizeof( T ), CC_AllocatorTypeCaller( T ) )

#define CC_Deallocate( allocator, memory_pointer ) \
	CC_RequestDeallocate( allocator, memory_pointer, CC_AllocatorEmptyCaller() )

typedef struct Caller_Info {
	const char *type;
	const char *function;
	const char *file;
	long line;
} Caller_Info;

/* Allocator */

typedef u8 * ( *PFN_Allocator_allocate )( struct Allocator *allocator, u64 count, u64 size, Caller_Info caller );
typedef u8 * ( *PFN_Allocator_reallocate )( struct Allocator *allocator, void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller );
typedef void ( *PFN_Allocator_deallocate )( struct Allocator *allocator, void *memory_pointer, Caller_Info caller );

typedef struct Allocator {
	PFN_Allocator_allocate allocate;
	PFN_Allocator_reallocate reallocate;
	PFN_Allocator_deallocate deallocate;
} Allocator;

Allocator allocator_create(
	PFN_Allocator_allocate pfn_allocate,
	PFN_Allocator_reallocate pfn_reallocate,
	PFN_Allocator_deallocate pfn_deallocate
);

u8 * allocator_request_allocate( Allocator *allocator, u64 count, u64 size, Caller_Info caller );
u8 * allocator_request_reallocate( Allocator *allocator, void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller );
void allocator_request_deallocate( Allocator *allocator, void *memory_pointer, Caller_Info caller );

/* System_Allocator */

typedef struct System_Allocator {
	/* Allocator */
	Allocator vtable;
	/* System_Allocator */
} System_Allocator;

System_Allocator system_allocator_create( void );

u8 * system_allocator_allocate( System_Allocator *allocator, u64 count, u64 size, Caller_Info caller );
u8 * system_allocator_reallocate( System_Allocator *allocator, void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller );
void system_allocator_deallocate( System_Allocator *allocator, void *memory_pointer, Caller_Info caller );

/* Linear_Allocator */

typedef struct Linear_Allocator {
	/* Allocator */
	Allocator vtable;
	/* Linear_Allocator */
	u8 *memory_start;
	u8 *memory_end;
	u8 *cursor;
	u8 *cursor_max;
	u64 allocated;
} Linear_Allocator;

Linear_Allocator linear_allocator_create( void );

u8 * linear_allocator_allocate( Linear_Allocator *allocator, u64 count, u64 size, Caller_Info caller );
u8 * linear_allocator_reallocate( Linear_Allocator *allocator, void *memory_pointer, u64 old_count, u64 new_count, u64 size, Caller_Info caller );
void linear_allocator_deallocate( Linear_Allocator *allocator, void *memory_pointer, Caller_Info caller );

void linear_allocator_init( Linear_Allocator *allocator, void *memory_pointer, u64 size );
void linear_allocator_deinit( Linear_Allocator *allocator );
void linear_allocator_reset( Linear_Allocator *allocator, bool zero_memory );
u64  linear_allocator_occupied( Linear_Allocator *allocator );

/* Module initialization */

bool allocators_init( void );
void allocators_shutdown( void );

// Global System_Allocator
extern Allocator *g_sys_allocator;

#endif /* C_COMPILER__ALLOCATOR_H */
