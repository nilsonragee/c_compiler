#ifndef C_COMPILER_ARRAY_H
#define C_COMPILER_ARRAY_H

#include "common.h"

// Count to the end of the array's size.
#define DARRAY_WHOLE_SIZE 0

#define CStringViewFormat "%.*s"
#define CStringViewArgument( array_view ) array_view.size, array_view.data

/* Array_View */

typedef struct Array_View {
	u8 *data;
	u32 item_size;
	u32 item_count;
} Array_View;

Array_View array_view( void *data, u32 item_size, u32 item_count );

/* Dynamic_Array */

#define ARRAY_TYPE( type )
#define DECL_Dynamic_Array( type, variable )  Dynamic_Array variable
#define DECL_Array_View( type, variable )  Array_View variable
#define DARRAY_NEW( allocator, type, capacity )  darray_new( allocator, sizeof( type ), capacity )
#define DARRAY_AT_DEREF( darray_ptr, index, type )  *( type * )darray_at( darray_ptr, index )

typedef struct Dynamic_Array {
	Allocator *allocator;
	u8 *data;
	u32 item_size;
	u32 item_count;
	u32 capacity;
} Dynamic_Array;

/*
inline u8 * darray_at( Dynamic_Array *darray, u32 item_index );
inline u32  darray_index_of( Dynamic_Array *darray, void *item );
*/

inline void * darray_at( Dynamic_Array *darray, u32 item_index ) {
	Assert( item_index < darray->item_count );
	return &darray->data[ item_index * darray->item_size ];
};

inline u32 darray_index_of( Dynamic_Array *darray, void *item ) {
	Assert( ( u8 * )item >= ( u8 * )darray );
	Assert( ( u8 * )item <= ( u8 * )darray + ( darray->item_count - 1 ) * darray->item_size );
	u32 memory_offset = ( u32 )( ( u8 * )item - ( u8 * )darray->data );
	u32 index = memory_offset / darray->item_size;
	return index;
};

Dynamic_Array darray_new( Allocator *allocator, u32 item_size, u32 initial_capacity );
Dynamic_Array darray_new_copy( Allocator *allocator, Array_View view );
Array_View darray_view( Dynamic_Array *darray, u32 item_offset, u32 item_count );

u32   darray_resize( Dynamic_Array *darray, u32 new_capacity );

// Returns newly added item's index.
u32   darray_add( Dynamic_Array *darray, void *item );
// Returns number of added items.
u32   darray_add_many( Dynamic_Array *darray, Array_View source );

u32   darray_add_repeat( Dynamic_Array *darray, void *item, u32 count );
u32   darray_add_from_array( Dynamic_Array *destination, Dynamic_Array *source, u32 source_item_offset, u32 item_count );
bool   darray_pop( Dynamic_Array *darrayy, void *out_item );
void   darray_clear( Dynamic_Array *darray, bool zero_memory );
bool   darray_free( Dynamic_Array *darray, bool zero_memory );
bool   darray_contains( Dynamic_Array *darray, void *item );
void * darray_find( Dynamic_Array *darray, void *item );
void   darray_remove_at_pointer( Dynamic_Array *darray, void *item );
void   darray_remove_at_index( Dynamic_Array *darray, u32 item_index );

#endif /* C_COMPILER_ARRAY_H */
