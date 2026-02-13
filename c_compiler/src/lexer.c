#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>  // EOF
#include <stdarg.h>

#include "lexer.h"

static bool lexer__is_at_end( const char *cursor );
static void lexer__eat_char_unchecked( Lexer *lexer );
static void lexer__eat_token_string( Lexer *lexer, u32 length );

static LexerResult lexer__eat_token_keyword( Lexer *lexer, Token *token, TokenKind kind, u32 length, bool eat_token );
static LexerResult lexer__parse_token_identifier( Lexer *lexer, Token *token, bool eat_token );
static LexerResult lexer__parse_token_literal_number( Lexer *lexer, Token *token, bool eat_token );
static LexerResult lexer__parse_token_literal_string( Lexer *lexer, Token *token, bool eat_token );
static LexerResult lexer__parse_token_literal_char( Lexer *lexer, Token *token, bool eat_token );

static LexerResult lexer__next_token( Lexer *lexer, Token *token, bool eat_token );


static bool lexer__is_at_end( const char *cursor ) {
	// Is the current character at source cursor pointer an End-Of-File or a null-terminator ('\0')?
	char c = cursor[ 0 ];
	bool at_end = ( c == EOF || c == '\0' );
	return at_end;
}

static void lexer__eat_char_unchecked( Lexer *lexer ) {
	lexer->cursor.base.data += 1;
	lexer->cursor.base.item_count -= 1;
	lexer->column += 1;
}

// No End-Of-File check!
static void lexer__eat_token_string( Lexer *lexer, u32 length ) {
	lexer->cursor.base.data += length;
	lexer->cursor.base.item_count -= length;
	lexer->column += length;
}

// No End-Of-File check!
static LexerResult lexer__eat_token_keyword( Lexer *lexer, Token *token, TokenKind kind, u32 length, bool eat_token ) {
	const char *const cursor = lexer->cursor.base.data;

	token->kind = kind;
	token->view = c_string_view( cursor, 0, length );
	if ( eat_token )  lexer__eat_token_string( lexer, length );
	return Lexer_OK;
}

static LexerResult lexer__parse_token_identifier( Lexer *lexer, Token *token, bool eat_token ) {
	const char *cursor = lexer->cursor.base.data;

	cursor += 1;
	while ( char_is_alpha_numeric( *cursor ) || *cursor == '_' ) {
		// lexer__eat_char_unchecked( lexer );
		if ( lexer__is_at_end( cursor ) )  return Lexer_EOF;
		cursor += 1;
	}

	u32 token_length = ( u32 )( cursor - lexer->cursor.base.data );
	token->kind = TokenKind_Identifier;
	token->view = c_string_view( cursor - token_length, 0, token_length );
	if ( eat_token )  lexer__eat_token_string( lexer, token_length );
	return Lexer_OK;
}

static LexerResult lexer__parse_token_literal_number( Lexer *lexer, Token *token, bool eat_token ) {
	const char *cursor = lexer->cursor.base.data;

	// lexer__eat_char_unchecked( lexer );
	cursor += 1;
	while ( char_is_numeric( *cursor ) || *cursor == '.' || *cursor == 'f' || *cursor == 'u' || *cursor == 'l' ) {
		// lexer__eat_char_unchecked( lexer );
		if ( lexer__is_at_end( cursor ) )  return Lexer_EOF;
		cursor += 1;
	}

	u32 token_length = ( u32 )( cursor - lexer->cursor.base.data );
	token->kind = TokenKind_LiteralNumber;
	token->view = c_string_view( cursor - token_length, 0, token_length );
	if ( eat_token )  lexer__eat_token_string( lexer, token_length );
	return Lexer_OK;
}

static LexerResult lexer__parse_token_literal_string( Lexer *lexer, Token *token, bool eat_token ) {
	const char *cursor = lexer->cursor.base.data;

	// lexer__eat_char_unchecked( lexer );
	cursor += 1;
	while ( *cursor != '\"' ) {
		// lexer__eat_char_unchecked( lexer );
		if ( lexer__is_at_end( cursor ) )  return Lexer_EOF;
		cursor += 1;
	}
	// lexer__eat_char_unchecked( lexer );  // Enclosing " (double-quote)
	cursor += 1;

	u32 length_with_quotes = ( u32 )( cursor - lexer->cursor.base.data );
	u32 token_length = length_with_quotes - 2;
	token->kind = TokenKind_LiteralString;
	token->view = c_string_view( cursor - length_with_quotes, 1, token_length );
	if ( eat_token )  lexer__eat_token_string( lexer, length_with_quotes );
	return Lexer_OK;
}

static LexerResult lexer__parse_token_literal_char( Lexer *lexer, Token *token, bool eat_token ) {
	const char *cursor = lexer->cursor.base.data;

	// lexer__eat_char_unchecked( lexer );
	cursor += 1;
	while ( *cursor != '\'' ) {
		// lexer__eat_char_unchecked( lexer );
		if ( lexer__is_at_end( cursor ) )  return Lexer_EOF;
		cursor += 1;
	}
	// lexer__eat_char_unchecked( lexer );  // Enclosing ' (single-quote)
	cursor += 1;

	u32 length_with_quotes = ( u32 )( cursor - lexer->cursor.base.data );
	u32 token_length = length_with_quotes - 2;
	token->kind = TokenKind_LiteralChar;
	token->view = c_string_view( cursor - length_with_quotes, 1, token_length );
	if ( eat_token )  lexer__eat_token_string( lexer, length_with_quotes );
	return Lexer_OK;
}

static LexerResult lexer__next_token( Lexer *lexer, Token *token, bool eat_token ) {
	lexer_eat_whitespace( lexer );
	char **c = &lexer->cursor.base.data;
	while ( !lexer_is_at_end( lexer ) && **c == '#' ) {
		// Skip macros
		lexer_eat_to_line_end( lexer );
		lexer_eat_whitespace( lexer );
	}

	if ( lexer_is_at_end( lexer ) )  return Lexer_EOF;

comment_skip:
	if ( string_view_starts_with( lexer->cursor, CStrView( "//" ) ) ) {
		lexer_eat_to_line_end( lexer );
		lexer_eat_whitespace( lexer );
		goto comment_skip;
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "/*" ) ) ) {
		while ( !lexer_is_at_end( lexer ) && !string_view_starts_with( lexer->cursor, CStrView( "*/" ) ) ) {
			lexer__eat_char_unchecked( lexer );
		}
		lexer__eat_char_unchecked( lexer );  // '*'
		lexer__eat_char_unchecked( lexer );  // '/'
		lexer_eat_whitespace( lexer );
		goto comment_skip;
	}

	const char *cursor = lexer->cursor.base.data;
	u32 line_begin = lexer->line;
	u32 column_begin = lexer->column;
	token->line = line_begin;
	token->column = column_begin;

	if ( **c == '(' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_LeftParenthesis, 1, eat_token );
	} else if ( **c == ')' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_RightParenthesis, 1, eat_token );
	} else if ( **c == '[' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_LeftBracket, 1, eat_token );
	} else if ( **c == ']' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_RightBracket, 1, eat_token );
	} else if ( **c == '{' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_LeftCurlyBrace, 1, eat_token );
	} else if ( **c == '}' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_RightCurlyBrace, 1, eat_token );
	} else if ( **c == ',' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_Comma, 1, eat_token );
	} else if ( **c == '*' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_Star, 1, eat_token );
	} else if ( **c == '=' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_Assign, 1, eat_token );
	} else if ( **c == ';' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_Semicolon, 1, eat_token );
	} else if ( **c == '+' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_Plus, 1, eat_token );
	} else if ( **c == '-' ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_Minus, 1, eat_token );
	} else if ( **c == '\'' ) {
		return lexer__parse_token_literal_char( lexer, token, eat_token );
	} else if ( **c == '\"' ) {
		return lexer__parse_token_literal_string( lexer, token, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "return" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_return, 6, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "int" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_int, 3, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "char" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_char, 4, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "bool" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_bool, 4, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "float" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_float, 5, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "true" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_true, 4, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "false" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_false, 5, eat_token );
	} else if ( char_is_numeric( **c ) ) {
		return lexer__parse_token_literal_number( lexer, token, eat_token );
	} else if ( char_is_alpha( **c ) ) {
		return lexer__parse_token_identifier( lexer, token, eat_token );
	}

	return Lexer_EOF;
}


void lexer_report_info( Lexer *lexer, const char *format, ... ) {
	fprintf( stdout, "Lexer: "FMT_StrView":%u:%u: Info: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stdout, format, args );
	va_end( args );
}

void lexer_report_warning( Lexer *lexer, const char *format, ... ) {
	fprintf( stdout, "Lexer: "FMT_StrView":%u:%u: Warning: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stdout, format, args );
	va_end( args );
}

void lexer_report_error( Lexer *lexer, const char *format, ... ) {
	fprintf( stderr, "Lexer: "FMT_StrView":%u:%u: Error: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stderr, format, args );
	va_end( args );
}

Lexer lexer_open_file( StringView_ASCII file_path ) {
	Lexer lexer;
	lexer.file_path = file_path;
	lexer.line = 0;
	lexer.column = 0;
	lexer.report_info = lexer_report_info;
	lexer.report_warning = lexer_report_warning;
	lexer.report_error = lexer_report_error;

	// Open file
	FILE *file = fopen( lexer.file_path.base.data, "r" );

	// Get file size
	fseek( file, 0, SEEK_END );
	size_t file_size = ftell( file );
	fseek( file, 0, SEEK_SET );

	// Allocate memory
	lexer.source = string_new( g_sys_allocator, ( u32 )file_size );
	// Read file
	size_t file_bytes_read = fread( lexer.source.darray.data, sizeof( char ), file_size, file );
	if ( file_bytes_read == 0 ) {
		Assert( feof( file ) );
		Assert( file_size < U32_MAX );
		lexer.source.darray.item_count = ( u32 )file_size;
	} else {
		Assert( file_bytes_read < U32_MAX );
		lexer.source.darray.item_count = ( u32 )file_bytes_read;
	}
	lexer.cursor = string_view( &lexer.source, 0, 0 );

	// Close file
	fclose( file );
	return lexer;
}

bool lexer_is_at_end( Lexer *lexer ) {
	// Is the current character at source cursor pointer an End-Of-File or a null-terminator ('\0')?
	char c = lexer->cursor.base.data[ 0 ];
	bool at_end = ( c == EOF || c == '\0' );
	return at_end;
}

u32 lexer_eat_whitespace( Lexer *lexer ) {
	char *c = lexer->cursor.base.data;
	// While there are characters left:
	while ( lexer->cursor.base.item_count > 0 ) {
		switch ( *c ) {
			case '\n':  // Fall-through
				// In case of new-line, additionally advance line index:
				lexer->line += 1;
				lexer->column = 0;
				lexer->column -= 1;  // Compensate for a later + 1.
				// and fall-through to the next case...
			case ' ':  // Fall-through
			case '\t':
			case '\r':
				// It is whitespace:
				// Advance cursor, update remaining size, continue while-loop.
				c += 1;
				lexer->cursor.base.item_count -= 1;
				lexer->column += 1;
				continue;
		}
		break;  // It is not whitespace: break out of while-loop.
	}
	// Calculate the amount of characters eaten by getting difference
	// between an end and begin cursor pointers.
	u32 eaten = ( u32 )( c - lexer->cursor.base.data );
	// Update the `lexer`s source cursor pointer.
	lexer->cursor.base.data = c;
	return eaten;
}

u32 lexer_eat_to_line_end( Lexer *lexer ) {
	char *c = lexer->cursor.base.data;
	// While there are characters left:
	while ( lexer->cursor.base.item_count > 0 ) {
		if ( *c == '\n' ) {
			// It is at a new-line ('\n'):
			// Advance line index;
			lexer->line += 1;
			lexer->column = 0;
			// Advance cursor, update remaining size, break out of while-loop.
			c += 1;
			lexer->cursor.base.item_count -= 1;
			break;
		} else {
			// Advance cursor, update remaining size, continue while-loop.
			c += 1;
			lexer->cursor.base.item_count -= 1;
			// lexer->column += 1;
		}
	}
	// Calculate the amount of characters eaten by getting difference
	// between an end and begin cursor pointers.
	u32 eaten = ( u32 )( c - lexer->cursor.base.data );
	// Update the `lexer`s source cursor pointer.
	lexer->cursor.base.data = c;
	return eaten;
}

LexerResult lexer_peak_next_token( Lexer *lexer, Token *token ) {
	return lexer__next_token( lexer, token, /* eat_token */ false );
}

LexerResult lexer_next_token( Lexer *lexer, Token *token ) {
	return lexer__next_token( lexer, token, /* eat_token */ true );
}

bool lexer_expect_many_this_token_kinds( Lexer *lexer, Token *token, DECL_Array_View( TokenKind, expected_kinds ) ) {
	// Check if this token kind is any of expected kinds.
	ForIt ( u32, TokenKind, expected_kinds.data, expected_kinds.item_count ) {
		if ( token->kind == it ) {
			return true;
		}
	}}

	// If this token kind does not match any of expected kinds, report an error.
	String_ASCII token_kind_list_string = token_kinds_to_string( g_sys_allocator, 0, expected_kinds );
	StringView_ASCII token_kind_list_view = string_view( &token_kind_list_string, 0, 0 );
	StringView_ASCII got_token_name = token_kind_name( token->kind );
	lexer->report_error( lexer, "Expected tokens "FMT_StrView", but got '"FMT_StrView"'.",
		ARG_StrView( token_kind_list_view ), ARG_StrView( got_token_name ) );
	string_free( &token_kind_list_string, false );

	return false;
}

bool lexer_expect_many_next_token_kinds( Lexer *lexer, Token *token, DECL_Array_View( TokenKind, expected_kinds ) ) {
	Assert( expected_kinds.item_size == sizeof( TokenKind ) );
	// Eat next token.
	LexerResult result = lexer_next_token( lexer, token );
	if ( result != Lexer_OK ) {
		if ( result == Lexer_EOF ) {
			String_ASCII token_kind_list_string = token_kinds_to_string( g_sys_allocator, 0, expected_kinds );
			StringView_ASCII token_kind_list_view = string_view( &token_kind_list_string, 0, 0 );
			StringView_ASCII got_token_name = token_kind_name( token->kind );
			lexer->report_error( lexer, "Expected tokens "FMT_StrView", but got '"FMT_StrView"'.",
				ARG_StrView( token_kind_list_view ), ARG_StrView( got_token_name ) );
			string_free( &token_kind_list_string, false );
		} else {
			lexer->report_error( lexer, "Unexpected error while parsing next token.", NULL );
		}
		return false;
	}

	// Compare this token kind to array of expected kinds.
	return lexer_expect_many_this_token_kinds( lexer, token, expected_kinds );
}

bool lexer_expect_this_token_kind( Lexer *lexer, Token *token, TokenKind expected_kind ) {
	// Check if this token kind is of expected kind.
	if ( token->kind == expected_kind )  return true;

	// If this token kind does not match expected kind, report an error.
	StringView_ASCII expected_token_name = token_kind_name( expected_kind );
	StringView_ASCII got_token_name = token_kind_name( token->kind );
	lexer->report_error( lexer, "Expected token '"FMT_StrView"', but got '"FMT_StrView"'.",
		ARG_StrView( expected_token_name ), ARG_StrView( got_token_name ) );

	return false;
}

bool lexer_expect_next_token_kind( Lexer *lexer, Token *token, TokenKind expected_kind ) {
	// Eat next token.
	LexerResult result = lexer_next_token( lexer, token );
	if ( result != Lexer_OK ) {
		if ( result == Lexer_EOF ) {
			StringView_ASCII expected_token_name = token_kind_name( expected_kind );
			StringView_ASCII eof_name = CStrView( "End-Of-File" );
			lexer->report_error( lexer, "Expected token '"FMT_StrView"', but got '"FMT_StrView"'.",
				ARG_StrView( expected_token_name ), ARG_StrView( eof_name ) );
		} else {
			lexer->report_error( lexer, "Unexpected error while parsing next token.", NULL );
		}
		return false;
	}

	// Compare this token kind to expected kind.
	return lexer_expect_this_token_kind( lexer, token, expected_kind );
}
