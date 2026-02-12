#include "string_ascii.h"

u64 c_string_length( const char *c_string ) {
	u64 size = 0;
	if ( !c_string )
		return size;

	char c = *c_string;
	while ( c != '\0' ) {
		c_string += sizeof( char );
		size += sizeof( char );
		c = *c_string;
	}

	return size;
}

static StringView_ASCII string_view_impl( const char *data, u32 original_length, u32 offset, u32 view_length ) {
	if ( view_length == STRING_WHOLE_LENGTH ) {
		u32 available_length = original_length - offset;
		view_length = min( original_length, available_length );
	}

	StringView_ASCII view;
	const char *view_data = data + offset;
	view.base = array_view( view_data, sizeof( char ), view_length );

	return view;
}

StringView_ASCII string_subview( StringView_ASCII view, u32 offset, u32 length ) {
	return string_view_impl( view.base.data, view.base.item_count, offset, length );
}

StringView_ASCII string_view( String_ASCII *string, u32 offset, u32 length ) {
	return string_view_impl( string->darray.data, string->darray.item_count, offset, length );
}

StringView_ASCII c_string_view( const char *c_string, u32 offset, u32 length ) {
	return string_view_impl( c_string, c_string_length( c_string ), offset, length );
}

String_ASCII string_new( Allocator *allocator, u32 initial_capacity_in_bytes ) {
	u32 capacity = initial_capacity_in_bytes;
	String_ASCII string = ( String_ASCII ) {
		.darray = ( Dynamic_Array ) {
			.allocator = allocator,
			// .data = NULL,
			.item_size = sizeof( char ),
			.item_count = 0,
			.capacity = capacity
		}
	};

	string.darray.data = ( capacity > 0 ) ? CC_Allocate( allocator, capacity * sizeof( char ), char ) : NULL;
	return string;
}

String_ASCII string_new_copy( Allocator *allocator, StringView_ASCII view ) {
	u32 capacity = view.base.item_count;
	String_ASCII string = ( String_ASCII ) {
		.darray = ( Dynamic_Array ) {
			.allocator = allocator,
			// .data = NULL,
			.item_size = sizeof( char ),
			.item_count = 0,
			.capacity = capacity
		}
	};

	if ( view.base.item_count > 0 ) {
		string.darray.data = ( capacity > 0 ) ? CC_Allocate( allocator, capacity * sizeof( char ), char ) : NULL;
		string_add_view( &string, view );
	} else {
		string.darray.data = NULL;
	}

	return string;
}

u32 string_resize( String_ASCII *string, u32 new_capacity ) {
	Dynamic_Array *darray = &string->darray;
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

	darray->data = CC_Reallocate( darray->allocator, darray->data, darray->capacity * darray->item_size, new_capacity * darray->item_size, char );
	darray->capacity = new_capacity;
	return darray->capacity;
}

u32 string_add_char( String_ASCII *string, char ascii_char ) {
	const u32 last_char_idx = string->darray.item_count;  // `* item_size` is redundant because it is always `sizeof( char )`
	if ( last_char_idx + 1 > string->darray.capacity ) {
		string_resize( string, string->darray.capacity * 2 );
	}

	string->darray.data[ last_char_idx ] = ascii_char;
	string->darray.item_size += 1;

	return last_char_idx;
}

u32 string_add_view( String_ASCII *destination, StringView_ASCII source ) {
	const u32 last_char_idx = destination->darray.item_count;  // `* item_size` is redundant because it is always `sizeof( char )`
	if ( last_char_idx + source.base.item_count > destination->darray.capacity ) {
		string_resize( destination, destination->darray.capacity * 2 );
	}

	u32 dst_idx = last_char_idx;
	u32 src_idx = 0;
	while ( src_idx < source.base.item_count ) {
		destination->darray.data[ dst_idx ] = source.base.data[ src_idx ];
		destination->darray.item_count += 1;
		dst_idx += 1;
		src_idx += 1;
	}

	return last_char_idx;
}

char string_pop( String_ASCII *string, bool do_pop ) {
	char popped = string->darray.data[ string->darray.item_count - 1 ];
	if ( do_pop )
		string->darray.item_count -= 1;

	return popped;
}

/*
UnicodeChar string_pop_unicode( String_ASCII *string, bool do_pop ) {
	char popped = string_pop( string, do_pop );
	UnicodeChar variable = {
		.size_in_bytes = 1,
		.bytes = { static_cast< char8_t >( popped ), 0x00, 0x00, 0x00 }
	};

	return variable;
}
*/

static char string_peek_impl( const char *data, u32 original_length, u32 offset ) {
	if ( offset > original_length )
		return '\0';

	return data[ offset ];
}

char string_view_peek( StringView_ASCII view, u32 offset ) {
	return string_peek_impl( view.base.data, view.base.item_count, offset );
}

char string_peek( String_ASCII *string, u32 offset ) {
	return string_peek_impl( string->darray.data, string->darray.item_count, offset );
}

char c_string_peek( const char *c_string, u32 offset ) {
	return string_peek_impl( c_string, c_string_length( c_string ), offset );
}

/*
static UnicodeChar string_peek_unicode_impl( u32 size, const char *data, u32 offset_in_bytes ) {
	char peeked = string_peek_impl( size, data, offset_in_bytes );
	UnicodeChar unicode = {
		.size_in_bytes = 1,
		.bytes = { static_cast< char8_t >( peeked ), 0x00, 0x00, 0x00 }
	};

	return unicode;
};

UnicodeChar string_peek_unicode( StringView_ASCII view, u32 offset_in_bytes ) {
	return string_peek_unicode_impl( view.size, view.data, offset_in_bytes );
}

UnicodeChar string_peek_unicode( String_ASCII *string, u32 offset_in_bytes ) {
	return string_peek_unicode_impl( string->size, string->data, offset_in_bytes );
}

UnicodeChar string_peek_unicode( const char *c_string, u32 offset_in_bytes ) {
	return string_peek_unicode_impl( string_length( c_string ), c_string, offset_in_bytes );
}
*/

bool string_view_equals( StringView_ASCII a, StringView_ASCII b ) {
	const u32 length = a.base.item_count;
	if ( length != b.base.item_count )
		return false;

	for ( u32 char_idx = 0; char_idx < length; char_idx += 1 ) {
		if ( a.base.data[ char_idx ] != b.base.data[ char_idx ] )
			return false;
	}

	return true;
}

bool string_view_starts_with( StringView_ASCII source, StringView_ASCII search ) {
	if ( search.base.item_count > source.base.item_count )
		return false;

	u32 char_idx = 0;
	while ( source.base.data[ char_idx ] == search.base.data[ char_idx ] && char_idx < search.base.item_count ) {
		char_idx += 1;
	}

	bool starts = ( char_idx == search.base.item_count );
	return starts;
}

bool string_view_ends_with( StringView_ASCII source, StringView_ASCII search ) {
	if ( search.base.item_count > source.base.item_count )
		return false;

	u32 char_idx = source.base.item_count - search.base.item_count;
	while ( source.base.data[ char_idx ] == search.base.data[ char_idx ] && char_idx < search.base.item_count ) {
		char_idx += 1;
	}

	bool ends = ( char_idx == search.base.item_count );
	return ends;
}

// Ok for short strings, terrible for long ones.
// TODO: Use sophisticated search method for strings with size bigger than some constant.
s64 string_view_contains( StringView_ASCII source, StringView_ASCII search ) {
	if ( search.base.item_count > source.base.item_count )
		return -1;

	u32 consecutive_matches = 0;
	for ( u32 char_idx = 0; char_idx < search.base.item_count; char_idx += 1 ) {
		if ( source.base.data[ char_idx ] == search.base.data[ char_idx ] ) {
			consecutive_matches += 1;
		} else {
			consecutive_matches = 0;
			continue;
		}

		if ( consecutive_matches == search.base.item_count )
			return char_idx - consecutive_matches;
	}

	return -1;
}

void string_clear( String_ASCII *string, bool zero_memory ) {
	darray_clear( &string->darray, zero_memory );
}

bool string_free( String_ASCII *string, bool zero_memory ) {
	return darray_free( &string->darray, zero_memory );
}

/*
u32 string_add_multibyte_char(String *string, char8_t *utf8_character_array, u32 character_size_in_bytes) {
if (string->size_in_bytes + character_size_in_bytes > string->capacity) {
array_resize(string, string->capacity * 2);
}

const u32 utf8_character_index = string->size_in_bytes;
for (u32 utf8_char_byte_index = 0;
utf8_char_byte_index < character_size_in_bytes;
utf8_char_byte_index += 1)
{
string->data[string->size_in_bytes] = utf8_character_array[utf8_char_byte_index];
string->size_in_bytes += 1;
}

string->size += 1;

return utf8_character_index;
}
*/

/*
u32 string_add(String *string, char8_t *utf8_bytes_array, u32 array_size_in_bytes, u32 *utf8_characters_added) {
// Reset shift state. (Reentrant function variant of `mblen`)
// https://www.gnu.org/software/libc/manual/html_node/Shift-State.html
mbstate_t multibyte_shift_state = { };
// mbrlen(NULL, 0, &multibyte_shift_state);
u32 array_byte_index = 0;
u32 chars_added = 0;
u32 last_added_utf8_character_index = 0;
while (array_byte_index < array_size_in_bytes) {
const char *c_utf8_character_pointer = (const char *) &utf8_bytes_array[array_byte_index];
// MB_CUR_MAX - maximum length of a multibyte character in the current locale.
// https://www.man7.org/linux/man-pages/man3/MB_CUR_MAX.3.html
//
// // Don't care about locale, look up to 4 bytes. (basically just Unicode)
// const size_t c_utf8_character_max_size = MB_CUR_MAX;
const size_t c_utf8_character_max_size = QLIGHT_UNICODE_STRING_MAX_BYTES;
int utf8_char_size_in_bytes = mbrlen(c_utf8_character_pointer, c_utf8_character_max_size, &multibyte_shift_state);
if (utf8_char_size_in_bytes < 0) {
AssertMessage(utf8_char_size_in_bytes < 0, "Trying to add an invalid UTF-8 character to string.");
if (utf8_characters_added)
*utf8_characters_added = chars_added;

return last_added_utf8_character_index;
}

last_added_utf8_character_index = string_add_multibyte_char(string, &utf8_bytes_array[array_byte_index], utf8_char_size_in_bytes);
array_byte_index += utf8_char_size_in_bytes;
chars_added += 1;
}

if (utf8_characters_added)
*utf8_characters_added = chars_added;

return last_added_utf8_character_index;
}
*/

/*
StringChar string_pop(String *string, bool do_pop) {
StringChar result = { };

if (!string) {
// Return everything zeroed.
return result;
}

if (string->size < 1) {
// Return everything zeroed.
return result;
}

mbstate_t multibyte_shift_state = { };
const size_t c_utf8_character_max_size = QLIGHT_UNICODE_STRING_MAX_BYTES;
u32 byte_backward_index = 1;
while (result.size_in_bytes <= 0 && byte_backward_index < string->size_in_bytes) {
// multibyte_shift_state = { };
const char *c_utf8_character_pointer = (const char *) &string->data[string->size_in_bytes - byte_backward_index];
result.size_in_bytes = mbrlen(c_utf8_character_pointer, c_utf8_character_max_size, &multibyte_shift_state);
if (result.size_in_bytes > 0) {
break;
}

byte_backward_index += 1;
}

// Copy character bytes from source string.
for (u32 byte_index = 0; byte_index < result.size_in_bytes; byte_index += 1) {
result.bytes[byte_index] = string->data[string->size_in_bytes - byte_backward_index];
byte_backward_index -= 1;
}

if (do_pop) {
string->size -= 1;
string->size_in_bytes -= result.size_in_bytes;
}

return result;
}
*/

/*
StringChar string_peek(String *string, u32 offset_in_bytes) {
mbstate_t multibyte_shift_state = { };
return string_peek(string, offset_in_bytes, &multibyte_shift_state);
}
*/

/*
StringChar string_peek(String *string, u32 offset_in_bytes, mbstate_t *multibyte_shift_state) {
StringChar result = { };

if (!string) {
// Return everything zeroed.
return result;
}

if (offset_in_bytes >= string->size_in_bytes) {
// Return everything zeroed.
return result;
}

const char *c_utf8_character_pointer = (const char *) &string->data[offset_in_bytes];
const size_t c_utf8_character_max_size = QLIGHT_UNICODE_STRING_MAX_BYTES;

// Get character size using standard C library.
result.size_in_bytes = static_cast<s32>(mbrlen(
c_utf8_character_pointer,
c_utf8_character_max_size,
multibyte_shift_state
));

// Check whether the character has a valid UTF-8 encoding.
if (result.size_in_bytes < 0) {
AssertMessage(result.size_in_bytes < 0, "Trying to add an invalid UTF-8 character to string.");

// Set `size_in_bytes` to -1 to let the function caller know the character was invalid.
result.size_in_bytes = -1;
return result;
}

// Copy character bytes from source string.
for (u32 byte_index = 0; byte_index < result.size_in_bytes; byte_index += 1) {
result.bytes[byte_index] = string->data[offset_in_bytes + byte_index];
}

return result;
}
*/
