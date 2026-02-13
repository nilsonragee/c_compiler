#ifndef C_COMPILER_STRING_ASCII_H
#define C_COMPILER_STRING_ASCII_H

// #include "string_common.h"
#include "common.h"
#include "array.h"

#define STRING_WHOLE_LENGTH 0

#define CStrView( c_string )  c_string_view( c_string, 0, STRING_WHOLE_LENGTH )

#define FMT_StrView  "%.*s"
#define ARG_StrView( view )  view.base.item_count, view.base.data
#define ARG_StrViewPtr( view )  view->base.item_count, view->base.data

typedef struct String_ASCII {
	/* Dynamic_Array */
	ARRAY_TYPE( char )
	Dynamic_Array darray;
	/* String_ASCII */
} String_ASCII;

typedef struct StringView_ASCII {
	/* Array_View */
	ARRAY_TYPE( char )
	Array_View base;
	/* StringView_ASCII */
} StringView_ASCII;

// Returns the length of the `c_string`.
u32 c_string_length( const char *c_string );

// Returns a read-only `view` inside of the other `view`, `string`, or `c_string`.
StringView_ASCII string_subview( StringView_ASCII view, u32 offset, u32 length );
StringView_ASCII string_view( String_ASCII *string, u32 offset, u32 length );
StringView_ASCII c_string_view( const char *c_string, u32 offset, u32 length );

// Creates an ASCII String with `initial_capacity_in_bytes` and
// memory managed by `allocator`.
String_ASCII string_new( Allocator *allocator, u32 initial_capacity_in_bytes );
String_ASCII string_new_copy( Allocator *allocator, StringView_ASCII view );

inline String_ASCII string_new_copy_string( Allocator *allocator, String_ASCII *string ) {
	return string_new_copy( allocator, string_view( string, 0, STRING_WHOLE_LENGTH ) );
}

inline String_ASCII string_new_copy_c_string( Allocator *allocator, const char *c_string ) {
	return string_new_copy( allocator, c_string_view( c_string, 0, STRING_WHOLE_LENGTH ) );
}

u32 string_resize( String_ASCII *string, u32 new_capacity );

// Appends an ASCII (1 byte long) character `ascii_char` to the `string`.
// Returns the index of the `ascii_char`'s first byte inside the `string`.
u32 string_add_char( String_ASCII *string, char ascii_char );

// Appends an ASCII `source` string to the `destination` string.
// Returns the index of the `source` string's first byte inside the `destination` string.
u32 string_add_view( String_ASCII *destination, StringView_ASCII source );

inline u32 string_add_from_string( String_ASCII *destination, String_ASCII *source ) {
	return string_add_view( destination, string_view( source, 0, STRING_WHOLE_LENGTH ) );
}

inline u32 string_add_from_c_string( String_ASCII *destination, const char *source ) {
	return string_add_view( destination, c_string_view( source, 0, STRING_WHOLE_LENGTH ) );
}

// Pops the last character from the `string`.
// The `do_pop` argument determines whether to actually pop the character.
// In the case of `false` value, the behaviour is similar to the `string_peek` from the end.
// Returns either a single `char` or a `UnicodeChar` structure.
// In the seconds case, the 1st byte is the popped character followed by zeroes.
char string_pop( String_ASCII *string, bool do_pop );
// UnicodeChar string_pop_unicode( String_ASCII *string, bool do_pop = true );

// Peeks a character from either the `view`, `string`, or `c_string` with an `offset_in_bytes`.
// Returns either a single `char` or the `UnicodeChar` structure.
char string_view_peek( StringView_ASCII view, u32 offset );
char string_peek( String_ASCII *string, u32 offset );
char c_string_peek( const char *c_string, u32 offset );
// UnicodeChar string_peek_unicode( String_ASCII *string, u32 offset_in_bytes );
// UnicodeChar string_peek_unicode( StringView_ASCII view, u32 offset_in_bytes );
// UnicodeChar string_peek_unicode( const char *c_string, u32 offset_in_bytes );

// Returns whether two `StringView`s, `String`s, or C strings are equal.
bool string_view_equals( StringView_ASCII a, StringView_ASCII b );

inline bool string_equals( String_ASCII *a, String_ASCII *b ) {
	return string_view_equals( string_view( a, 0, STRING_WHOLE_LENGTH ), string_view( b, 0, STRING_WHOLE_LENGTH ) );
}

inline bool c_string_equals( const char *a, const char *b ) {
	return string_view_equals( c_string_view( a, 0, STRING_WHOLE_LENGTH ), c_string_view( b, 0, STRING_WHOLE_LENGTH ) );
}

// Returns whether the `search` string was found at the beginning of the `source` string.
bool string_starts_with( String_ASCII source, StringView_ASCII search );
bool string_view_starts_with( StringView_ASCII source, StringView_ASCII search );

// Returns whether the `search` string was found at the end of the `source` string.
bool string_ends_with( String_ASCII source, StringView_ASCII search );
bool string_view_ends_with( StringView_ASCII source, StringView_ASCII search );

// Returns the offset to the beginning of the `search` string inside of `source` string, in bytes.
// If `search` string was not found, -1 is returned.
s64 string_contains( String_ASCII source, StringView_ASCII search );
s64 string_view_contains( StringView_ASCII source, StringView_ASCII search );

// Clears the `string`'s contents.
// The `zero_memory` argument determines whether to fill left out memory with zeroes.
void string_clear( String_ASCII *string, bool zero_memory );

// Destroys the `string` by clearing its contents and freeing the allocated memory.
// The `zero_memory` argument determines whether to fill the memory with zeroes before deallocation.
bool string_free( String_ASCII *string, bool zero_memory );

bool char_is_alpha( char c );
bool char_is_numeric( char c );
bool char_is_alpha_numeric( char c );
bool char_is_whitespace( char c );

#endif /* C_COMPILER__STRING_ASCII_H */
