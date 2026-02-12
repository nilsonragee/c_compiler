#include "array.h"

#include <string.h>

/* Array_View */

Array_View array_view( void *data, u32 item_size, u32 item_count ) {
	Array_View view = ( Array_View ) {
		.data = data,
		.item_size = item_size,
		.item_count = item_count
	};

	return view;
}

/* Dynamic_Array */

Dynamic_Array darray_new( Allocator *allocator, u32 item_size, u32 initial_capacity ) {
	Dynamic_Array darray = ( Dynamic_Array ) {
		.allocator = allocator,
		.data = NULL,
		.item_size = item_size,
		.item_count = 0,
		.capacity = initial_capacity
	};

	darray.data = ( initial_capacity > 0 ) ? CC_Allocate( allocator, initial_capacity * item_size, u8 ) : NULL;
	return darray;
}

Dynamic_Array darray_new_copy( Allocator *allocator, Array_View view ) {
	Dynamic_Array darray = ( Dynamic_Array ) {
		.allocator = allocator,
		// .data = NULL,
		.item_size = view.item_size,
		.item_count = 0,
		.capacity = view.item_count
	};

	if ( view.item_count > 0 ) {
		darray.data = ( darray.capacity > 0 ) ? CC_Allocate( allocator, darray.capacity * darray.item_size, u8 ) : NULL;
		darray_add_many( &darray, view );
	} else {
		darray.data = NULL;
	}

	return darray;
}

Array_View darray_view( Dynamic_Array *darray, u32 item_offset, u32 item_count ) {
	AssertMessage( darray, "Dynamic_Array pointer is NULL" );
	AssertMessage( item_offset <= darray->item_count, "Dynamic_Array item offset is out of bounds" );
	if ( item_offset > darray->item_count ) {
		// If array offset is out of bounds, return everything zeroed.
		Array_View view = array_view( NULL, darray->item_size, 0 );
		return view;
	}

	u8 *view_data = darray->data + item_offset * darray->item_size;

	const u32 view_size_available = darray->item_count - item_offset;
	const bool whole_size = ( item_count == DARRAY_WHOLE_SIZE );
	u32 view_item_count;
	if ( whole_size ) {
		view_item_count = view_size_available;
	} else {
		AssertMessage( item_count <= view_size_available, "Array_View size is out of bounds" );
		// If view size is overflowing, clamp it to the end of array.
		view_item_count = ( item_count <= view_size_available) ? item_count : view_size_available;
	}

	Array_View view = array_view( view_data, darray->item_size, view_item_count );
	return view;
}

u32 darray_resize( Dynamic_Array *darray, u32 new_capacity ) {
	if ( new_capacity <= darray->item_count ) {
		// Shrink down the size, but keep capacity the same.
		darray->item_count = new_capacity;
		return darray->capacity;
	}

	if ( new_capacity <= darray->capacity ) {
		// Shrink down the size, but keep capacity the same.
		darray->item_count = new_capacity;
		return darray->capacity;
	}

	darray->data = CC_Reallocate( darray->allocator, darray->data, darray->capacity * darray->item_size, new_capacity * darray->item_size, u8 );
	darray->capacity = new_capacity;
	return darray->capacity;
}

// Returns newly added item's index.
u32 darray_add( Dynamic_Array *darray, void *item ) {
	if ( darray->item_count + 1 > darray->capacity ) {
		darray_resize( darray, darray->capacity * 2 );
	}

	const u32 current_item_index = darray->item_count;
	const u32 byte_offset = current_item_index * darray->item_size;
	memcpy( &darray->data[ byte_offset ], item, darray->item_size );
	darray->item_count++;

	return current_item_index;
}

// Returns number of added items.
u32 darray_add_many( Dynamic_Array *darray, Array_View source ) {
	// 1 enlargement might be not enough.
	// TODO(nilsonragee): Sufficient size might be calculated once, but right now I'm lazy.
	AssertMessage( source.item_size == darray->item_size, "Both Dynamic_Arrays should have the same item size" );
	while ( darray->item_count + source.item_count > darray->capacity ) {
		darray_resize( darray, darray->capacity * 2 );
	}

	const u32 current_item_index = darray->item_count;
	const u32 byte_offset = current_item_index * darray->item_size;
	const u32 copy_size = source.item_count * source.item_size;
	memcpy( &darray->data[ byte_offset ], source.data, copy_size );
	darray->item_count += source.item_count;

	// Assume we always add all items since this is a dynamic array case.
	return source.item_count;
}

u32 darray_add_repeat( Dynamic_Array *darray, void *item, u32 count ) {
	const u32 space_left = darray->capacity - darray->item_count;
	// const u32 items_to_add = (space_left <= count) ? space_left : count - space_left;
	const u32 items_to_add = (space_left <= count) ? space_left : count;
	if (items_to_add < 1)
		return items_to_add;

	u32 current_index = darray->item_count * darray->item_size;
	const u32 last_index = current_index + items_to_add;
	while ( current_index < last_index ) {
		memcpy( &darray->data[ current_index ], item, darray->item_size );
		current_index += 1;
	}

	darray->item_count += items_to_add;
	return items_to_add;
}

u32 darray_add_from_array( Dynamic_Array *destination, Dynamic_Array *source, u32 source_item_offset, u32 item_count ) {
	// Already checks inside `darray_add_many`
	// AssertMessage( source->item_size == destination->item_size, "Both Dynamic_Arrays should have the same item size" );

	u8 *source_data = source->data + source_item_offset * source->item_size;
	Array_View source_view = array_view( source_data, source->item_size, item_count );
	return darray_add_many( destination, source_view );
	/*
	const bool within_source_array = (source->size >= source_offset + count);
	const u32 source_items_to_add = (within_source_array) ? count : source->size - source_offset - count;
	if (source_items_to_add < 1)
	return source_items_to_add;

	const u32 space_left = destination->capacity - destination->size;
	// const u32 items_to_add = (space_left >= source_items_to_add) ? source_items_to_add : source_items_to_add - space_left;
	const u32 items_to_add = (space_left <= source_items_to_add) ? space_left : source_items_to_add;
	if (items_to_add < 1)
	return items_to_add;

	u32 destination_item_idx = destination->size;
	u32 source_item_idx = source_offset;
	for (u32 item_idx = 0; item_idx < items_to_add; item_idx += 1) {
	destination->data[destination_item_idx] = source->data[source_item_idx];
	destination_item_idx += 1;
	source_item_idx += 1;
	}

	destination->size += items_to_add;
	return items_to_add;
	return 0;
	*/
}

bool darray_pop( Dynamic_Array *darray, void *out_item ) {
	if ( darray->item_count < 1 )
		return false;

	if ( out_item ) {
		const u32 current_index = darray->item_count * darray->item_size;
		memcpy( out_item, &darray->data[ current_index - 1], darray->item_size );
	}

	darray->item_count -= 1;
	return true;
}

// Make it optional to choose the clear region?
// DArray_ClearBySize
// DArray_ClearByCapacity
// Currently clears by capacity.
void darray_clear( Dynamic_Array *darray, bool zero_memory ) {
	darray->item_count = 0;
	if ( zero_memory )
		memset( darray->data, 0, ( u64 )darray->capacity * darray->item_size );
}

bool darray_free( Dynamic_Array *darray, bool zero_memory ) {
	if ( !darray->data )         return false;
	if ( darray->capacity < 1 )  return false;

	darray_clear( darray, zero_memory );
	CC_Deallocate( darray->allocator, darray->data );
	darray->capacity = 0;
	return true;
}

bool darray_contains( Dynamic_Array *darray, void *item ) {
	for ( u32 item_idx = 0; item_idx < darray->item_count; item_idx += 1 ) {
		const void *array_item = &darray->data[ item_idx * darray->item_size ];
		const int memory_difference = memcmp( array_item, item, darray->item_size );
		if ( memory_difference == 0 )  return true;
	}
	return false;
}

void * darray_find( Dynamic_Array *darray, void *item ) {
	for ( u32 item_idx = 0; item_idx < darray->item_count; item_idx += 1 ) {
		void *array_item = &darray->data[ item_idx * darray->item_size ];
		const int memory_difference = memcmp( array_item, item, darray->item_size );
		if ( memory_difference == 0 )  return array_item;
	}
	return NULL;
}

// @TODO
/* THESE DO NOT UPDATE ITEM COUNT! */

bool darray_remove( Dynamic_Array *darray, void *item ) {
	void *array_item = darray_find( darray, item );
	if ( !array_item )
		return false;

	memset( array_item, 0, darray->item_size );
	return true;
}

void darray_remove_at_pointer( Dynamic_Array *darray, void *array_item ) {
	bool in_min_bounds = ( ( u8 * )array_item >= darray->data );
	bool in_max_bounds = ( ( u8 * )array_item < ( darray->data + darray->item_count * darray->item_size ) );
	Assert( in_min_bounds && in_max_bounds );
	if ( !in_min_bounds || !in_max_bounds )
		return;

	memset( array_item, 0, darray->item_size );
}

void darray_remove_at_index( Dynamic_Array *darray, u32 item_index ) {
	bool in_min_bounds = ( item_index >= 0 );
	bool in_max_bounds = ( item_index < darray->item_count );
	Assert( in_min_bounds && in_max_bounds );
	if ( !in_min_bounds || !in_max_bounds )
		return;

	void *array_item = darray_at( darray, item_index );
	memset( array_item, 0, darray->item_size );
}
