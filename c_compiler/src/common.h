#ifndef C_COMPILER_COMMON_H
#define C_COMPILER_COMMON_H

#include "types.h"
#include "allocator.h"

/*
	ForNamedFrom( u32, i, 1, 3 ) {
		log_info( "i=%u", i );
	}

	is equivalent to:

	for ( u32 i = 1; i < 3; i += 1 ) {
		log_info( "i=%u", i );
	}
*/
#define ForNamedFrom( type, variable, begin, count )  for ( type variable = begin; variable < count; variable += 1 )

/*
	ForNamed( u32, i, 3 ) {
		log_info( "i=%u", i );
	}

	is equivalent to:

	for ( u32 i = 0; i < 3; i += 1 ) {
		log_info( "i=%u", i );
	}
*/
#define ForNamed( type, variable, count )  ForNamedFrom( type, variable, 0, count )


/*
	ForNamedBackwards( s32, i, 3 ) {
		log_info( "i=%d", i );
	}

	is equivalent to:

	for ( s32 i = 3 - 1; i >= 0; i -= 1 ) {
		log_info( "i=%d", i );
	}
*/
#define ForNamedBackwards( type, variable, count )  for ( type variable = count - 1; variable >= 0; variable -= 1 )

/*
	For( u32, 3 ) {
		log_info( "it_index=%u", it_index );
	}

	is equivalent to:

	for ( u32 it_index = 0; it_index < 3; it_index += 1 ) {
		log_info( "it_index=%u", it_index );
	}
*/
#define For( type, count )  ForNamed( type, it_index, count )

/*
	ForFrom( u32, 1, 3 ) {
		log_info( "it_index=%u", it_index );
	}

	is equivalent to:

	for ( u32 it_index = 1; it_index < 3; it_index += 1 ) {
		log_info( "it_index=%u", it_index );
	}
*/
#define ForFrom( type, begin, count )  ForNamedFrom( type, it_index, begin, count )

/*
	ForBackwards( s32, 3 ) {
		log_info( "it_index=%d", it_index );
	}

	is equivalent to:

	for ( s32 it_index = 3 - 1; it_index >= 0; it_index -= 1 ) {
		log_info( "it_index=%d", it_index );
	}
*/
#define ForBackwards( type, count )  ForNamedBackwards( type, it_index, count )

#define For2( type, count )  ForNamed( type, it2_index, count )
#define For2Backwards( type, count )  ForNamedBackwards( type, it2_index, count )

#define For3( type, count )  ForNamed( type, it3_index, count )
#define For3Backwards( type, count )  ForNamedBackwards( type, it3_index, count )

/*
	StringView_ASCII str = "Hello World";
	ForIt( u32, char, str.base.data, str.base.item_count ) {
		log_info( "str[%2u]: '%c'", it_index, *it );
	}}

	is equivalent to:

	StringView_ASCII str = "Hello World";
	for ( u32 it_index = 0; it_index < str.base.item_count; it_index += 1 ) {
		char *it = &pointer[ it_index ];
		{
			log_info( "str[%2u]: '%c'", it_index, *it );
		}
	}
	// WARNING: Enclose with 2 curly braces! Take a look at example above.
*/
#define ForIt( counter_type, it_type, pointer, count )  \
		For( counter_type, count ) {  \
			it_type it = ( ( it_type *)pointer )[ it_index ];

/*
	StringView_ASCII str = "Hello World";
	ForItFrom( u32, char, str.base.data, 1, str.base.item_count ) {
		log_info( "str[%2u]: '%c'", it_index, *it );
	}}

	is equivalent to:

	StringView_ASCII str = "Hello World";
	for ( u32 it_index = 1; it_index < str.base.item_count; it_index += 1 ) {
		char *it = &pointer[ it_index ];
		{
			log_info( "str[%2u]: '%c'", it_index, *it );
		}
	}
	// WARNING: Enclose with 2 curly braces! Take a look at example above.
*/
#define ForItFrom( counter_type, it_type, pointer, begin, count )  \
		ForFrom( counter_type, begin, count ) {  \
			it_type it = ( ( it_type *)pointer )[ it_index ];

/*
	StringView_ASCII str = "Hello World";
	ForItDeref( u32, char, str.base.data, str.base.item_count ) {
		log_info( "str[%2u]: '%c'", it_index, it );
	}}

	is equivalent to:

	StringView_ASCII str = "Hello World";
	for ( u32 it_index = 0; it_index < str.base.item_count; it_index += 1 ) {
		char it = *( char * )&pointer[ it_index ];
		{
			log_info( "str[%2u]: '%c'", it_index, it );
		}
	}
	// WARNING: Enclose with 2 curly braces! Take a look at example above.
*/
#define ForItDeref( counter_type, it_type, pointer, count )  \
		For( counter_type, count ) {  \
			it_type it = *( ( it_type * )pointer )[ it_index ];

/*
	StringView_ASCII str = "Hello World";
	ForItBackwards( s32, char, str.base.data, str.base.item_count ) {
		log_info( "str[%2u]: '%c'", it_index, *it );
	}}

	is equivalent to:

	StringView_ASCII str = "Hello World";
	for ( s32 it_index = str.base.item_count - 1; it_index >= 0; it_index -= 1 ) {
		char *it = &pointer[ it_index ];
		{
			log_info( "str[%2u]: '%c'", it_index, *it );
		}
	}
	// WARNING: Enclose with 2 curly braces! Take a look at example above.
*/
#define ForItBackwards( counter_type, it_type, pointer, count )  \
		ForBackwards( counter_type, count ) {  \
			it_type it = pointer[ it_index ];

/*
	StringView_ASCII str = "Hello World";
	ForItDerefBackwards( s32, char, str.base.data, str.base.item_count ) {
		log_info( "str[%2u]: '%c'", it_index, it );
	}}

	is equivalent to:

	StringView_ASCII str = "Hello World";
	for ( s32 it_index = str.base.item_count - 1; it_index >= 0; it_index -= 1 ) {
		char it = *( char * )&pointer[ it_index ];
		{
			log_info( "str[%2u]: '%c'", it_index, *it );
		}
	}
	// WARNING: Enclose with 2 curly braces! Take a look at example above.
*/
#define ForItDerefBackwards( counter_type, it_type, pointer, count )  \
		ForBackwards( counter_type, count ) {  \
			it_type it = *( it_type * )pointer[ it_index ];


#define ForIt2( counter_type, it_type, pointer, count )  \
		For2( counter_type, count ) {  \
			it_type it2 = pointer[ it2_index ];

#define ForIt2Deref( counter_type, it_type, pointer, count )  \
		For( counter_type, count ) {  \
			it_type it2 = *( it_type * )pointer[ it2_index ];

#define ForIt2Backwards( counter_type, it_type, pointer, count )  \
		For2Backwards( counter_type, count ) {  \
			it_type it2 = pointer[ it2_index ];

#define ForItDeref2Backwards( counter_type, it_type, pointer, count )  \
		ForBackwards( counter_type, count ) {  \
			it_type it2 = *( it_type * )pointer[ it2_index ];


#define ForIt3( counter_type, it_type, pointer, count )  \
		For3( counter_type, count ) {  \
			it_type it3 = pointer[ it3_index ];

#define ForIt3Deref( counter_type, it_type, pointer, count )  \
		For( counter_type, count ) {  \
			it_type it3 = *( it_type * )pointer[ it3_index ];

#define ForIt3Backwards( counter_type, it_type, pointer, count )  \
		For3Backwards( counter_type, count ) {  \
			it_type it3 = pointer[ it3_index ];

#define ForItDeref3Backwards( counter_type, it_type, pointer, count )  \
		ForBackwards( counter_type, count ) {  \
			it_type it3 = *( it_type * )pointer[ it3_index ];


#ifndef min
	#define min( a, b )  ( a < b ) ? a : b
#endif

#ifndef max
	#define max( a, b )  ( a > b ) ? a : b
#endif

#define ARRAY_SIZE( array )  sizeof( array ) / sizeof( array[ 0 ] )

#endif /* C_COMPILER_COMMON_H */
