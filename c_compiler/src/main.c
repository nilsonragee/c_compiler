#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// c_compiler [options...] <files...>
// Options:
// -out --output-file-name

#include "common.h"
#include "console.h"
#include "array.h"
#include "string_ascii.h"

typedef enum TokenKinds {

	/* Group 1: Basic */

	TokenKind_Identifier,
	TokenKind_LiteralNumber,
	TokenKind_LiteralChar,
	TokenKind_LiteralString,

	/* Group 2: Keywords */

	// Types
	TokenKind_void, TokenKind__KeywordsBegin = TokenKind_void, TokenKind__DeclarablesBegin = TokenKind_void,
	TokenKind_bool, // C23
	TokenKind__Bool, // C99-C23
	TokenKind_char,
	TokenKind_int,
	TokenKind_float,
	TokenKind_double,
	TokenKind__BitInt, // C23
	TokenKind__Decimal32, // C23
	TokenKind__Decimal64, // C23
	TokenKind__Decimal128, // C23

	// Type specifiers
	TokenKind_short,
	TokenKind_long,
	TokenKind_unsigned,
	TokenKind_signed,
	TokenKind__Imaginary, // C11
	TokenKind__Complex, // C99
	TokenKind__Atomic, // C11
	// Additional type qualifiers
	TokenKind_const,
	TokenKind_volatile,
	TokenKind_restrict, // C99

	// User-definable types
	TokenKind_enum,
	TokenKind_struct,
	TokenKind_union,
	TokenKind_typedef, TokenKind__DeclarablesEnd = TokenKind_typedef,

	// if-else branching
	TokenKind_if,
	TokenKind_else,
	TokenKind_Ternary,

	// switch-case branching
	TokenKind_switch,
	TokenKind_case,
	TokenKind_default,

	// Iteration
	TokenKind_for,
	TokenKind_do,
	TokenKind_while,

	// Execution flow control
	TokenKind_break,
	TokenKind_continue,
	TokenKind_return,
	TokenKind_goto,
	TokenKind_GotoLabel,

	// Storage specifiers
	TokenKind_auto,
	TokenKind_thread_local, // C23
	TokenKind__Thread_local, // C11-C23
	TokenKind_register,

	// Linkage specifiers
	TokenKind_extern,
	TokenKind_static,

	TokenKind_alignas, // C23
	TokenKind_alignof, // C23
	TokenKind_constexpr, // C23
	TokenKind_false, // C23
	TokenKind_inline, // C99
	TokenKind_nullptr, // C23
	TokenKind_sizeof,
	TokenKind_static_assert, // C23
	TokenKind_true, // C23
	TokenKind_typeof, // C23
	TokenKind_typeof_unequal, // C23
	TokenKind__Alignas, // C11-C23
	TokenKind__Alignof, // C11-C23
	TokenKind__Generic, // C11
	TokenKind__Noreturn, // C11-C23
	TokenKind__Static_assert, // C11-C23

	/* Preprocessor keywords */

	// TokenKind_if
	TokenKind_elif,
	// TokenKind_else,
	TokenKind_endif,
	TokenKind_ifdef,
	TokenKind_ifndef,
	TokenKind_elifdef, // C23
	TokenKind_elifndef, // C23
	TokenKind_define,
	TokenKind_undef,
	TokenKind_include,
	TokenKind_embed, // C23
	TokenKind_line,
	TokenKind_error,
	TokenKind_warning, // C23
	TokenKind_pragma,
	TokenKind_defined,
	TokenKind__has_include, // C23
	TokenKind__has_embed, // C23
	TokenKind__has_c_attribute, // 23

	// Recognized outside of preprocessor
	TokenKind__Pragma, // C99

	/* Conditionally-supported keywords */

	TokenKind_asm,
	TokenKind_fortran,

	/* Group 3: Operators */

	// Assignment operators
	TokenKind_Assign,  // "="
	TokenKind_PlusAssign,  // "+="
	TokenKind_MinusAssign,  // "-="
	TokenKind_TimesAssign,  // "*="
	TokenKind_DivideAssign,  // "/="
	TokenKind_ModuloAssign,  // "%="
	TokenKind_BitwiseAndAssign,  // "&="
	TokenKind_BitwiseOrAssign,  // "|="
	TokenKind_BitwiseXorAssign,  // "^="
	TokenKind_BitwiseNotAssign,  // "~="
	TokenKind_BitwiseShiftLeftAssign,  // "<<="
	TokenKind_BitwiseShiftRightAssign,  // ">>="

	// Arithmetic operators
	TokenKind_Plus,  // "+" (Add or Positive)
	TokenKind_Minus,  // "-" (Substract or Negative)
	TokenKind_Star,  // "*" (Multiply or Pointer or Pointer dereference)
	TokenKind_Divide,  // "/"
	TokenKind_Modulo,  // "%"
	TokenKind_Increment,  // "++"
	TokenKind_Decrement,  // "--"

	// Comparison operators
	TokenKind_Equals,  // "=="
	TokenKind_NotEquals,  // "!="
	TokenKind_Less,  // "<"
	TokenKind_LessEquals,  // "<="
	TokenKind_Greater,  // ">"
	TokenKind_GreaterEquals,  // ">="

	// Logical operators
	TokenKind_LogicalAnd,  // "&&"
	TokenKind_LogicalOr,  // "||"
	TokenKind_LogicalNot,  // "!"

	// Bitwise operators
	TokenKind_BitwiseAnd,  // "&" (alternatively: TokenKind_Ampersand)
	TokenKind_BitwiseOr,  // "|"
	TokenKind_BitwiseXor,  // "^"
	TokenKind_BitwiseNot,  // "~"
	TokenKind_BitwiseShiftLeft,  // "<<"
	TokenKind_BitwiseShiftRight,  // ">>"

	// Access operators
	TokenKind_Dot,  // "." (Access by value)
	TokenKind_Arrow,  // "->" (Access by pointer)
	TokenKind_Ampersand,  // "&" (Address of, alternatively: TokenKind_BitwiseAnd)

	// Grouping & Separator punctuators
	TokenKind_LeftParenthesis,  // "("
	TokenKind_RightParenthesis,  // ")"
	TokenKind_LeftBracket,  // "["
	TokenKind_RightBracket,  // "]"
	TokenKind_LeftCurlyBrace,  // "{"
	TokenKind_RightCurlyBrace,  // "}"
	TokenKind_Colon,  // ":" (used in: "(condition) ? true : false", "case:", "label:")
	TokenKind_Semicolon,  // ";"
	TokenKind_Comma,  // ","

	TokenKind_Pointer, // "*"
	TokenKind_AddressOf, // "&"
	TokenKind_PointerDereference, /* "*" */ TokenKind_KEYWORDS_END = TokenKind_PointerDereference,

	TokenKind_COUNT
} TokenKinds;
typedef u32 TokenKind;

bool token_kind_is_type_id( TokenKind kind ) {
	switch ( kind ) {
		case TokenKind_void:
		case TokenKind_bool:
		case TokenKind__Bool:
		case TokenKind_char:
		case TokenKind_int:
		case TokenKind_float:
		case TokenKind_double:
		case TokenKind__BitInt:
		case TokenKind__Decimal32:
		case TokenKind__Decimal64:
		case TokenKind__Decimal128:
		case TokenKind_short:
		case TokenKind_long:
			return true;
		default:
			return false;
	}
}

static StringView_ASCII g_token_kind_names[ TokenKind_COUNT ];

static void token_kind_names_init() {
	// Basic
	g_token_kind_names[ TokenKind_Identifier ] = CStrView( "Identifier" );
	g_token_kind_names[ TokenKind_LiteralNumber ] = CStrView( "LiteralNumber" );
	g_token_kind_names[ TokenKind_LiteralChar ] = CStrView( "LiteralChar" );
	g_token_kind_names[ TokenKind_LiteralString ] = CStrView( "LiteralString" );

	// Types
	g_token_kind_names[ TokenKind_void ] = CStrView( "void" );
	g_token_kind_names[ TokenKind_bool ] = CStrView( "bool" );
	g_token_kind_names[ TokenKind__Bool ] = CStrView( "_Bool" );
	g_token_kind_names[ TokenKind_char ] = CStrView( "char" );
	g_token_kind_names[ TokenKind_int ] = CStrView( "int" );
	g_token_kind_names[ TokenKind_float ] = CStrView( "float" );
	g_token_kind_names[ TokenKind_double ] = CStrView( "double" );
	g_token_kind_names[ TokenKind__BitInt ] = CStrView( "_BitInt" );
	g_token_kind_names[ TokenKind__Decimal32 ] = CStrView( "_Decimal32" );
	g_token_kind_names[ TokenKind__Decimal64 ] = CStrView( "_Decimal64" );
	g_token_kind_names[ TokenKind__Decimal128 ] = CStrView( "_Decimal128" );

	// Type specifiers
	g_token_kind_names[ TokenKind_short ] = CStrView( "short" );
	g_token_kind_names[ TokenKind_long ] = CStrView( "long" );
	g_token_kind_names[ TokenKind_unsigned ] = CStrView( "unsigned" );
	g_token_kind_names[ TokenKind_signed ] = CStrView( "signed" );
	g_token_kind_names[ TokenKind__Imaginary ] = CStrView( "_Imaginary" );
	g_token_kind_names[ TokenKind__Complex ] = CStrView( "_Complex" );
	g_token_kind_names[ TokenKind__Atomic ] = CStrView( "_Atomic" );
	// Additional type qualifiers
	g_token_kind_names[ TokenKind_const ] = CStrView( "const" );
	g_token_kind_names[ TokenKind_volatile ] = CStrView( "volatile" );
	g_token_kind_names[ TokenKind_restrict ] = CStrView( "restrict" );

	// User-definable types
	g_token_kind_names[ TokenKind_enum ] = CStrView( "enum" );
	g_token_kind_names[ TokenKind_struct ] = CStrView( "struct" );
	g_token_kind_names[ TokenKind_union ] = CStrView( "union" );
	g_token_kind_names[ TokenKind_typedef ] = CStrView( "typedef" );

	// if-else branching
	g_token_kind_names[ TokenKind_if ] = CStrView( "if" );
	g_token_kind_names[ TokenKind_else ] = CStrView( "else" );
	g_token_kind_names[ TokenKind_Ternary ] = CStrView( "?" );

	// switch-case branching
	g_token_kind_names[ TokenKind_switch ] = CStrView( "switch" );
	g_token_kind_names[ TokenKind_case ] = CStrView( "case" );
	g_token_kind_names[ TokenKind_default ] = CStrView( "default" );

	// Iteration
	g_token_kind_names[ TokenKind_for ] = CStrView( "for" );
	g_token_kind_names[ TokenKind_do ] = CStrView( "do" );
	g_token_kind_names[ TokenKind_while ] = CStrView( "while" );

	// Execution flow control
	g_token_kind_names[ TokenKind_break ] = CStrView( "break" );
	g_token_kind_names[ TokenKind_continue ] = CStrView( "continue" );
	g_token_kind_names[ TokenKind_return ] = CStrView( "return" );
	g_token_kind_names[ TokenKind_goto ] = CStrView( "goto" );
	g_token_kind_names[ TokenKind_GotoLabel ] = CStrView( "GotoLabel" );

	// Storage specifiers
	g_token_kind_names[ TokenKind_auto ] = CStrView( "auto" );
	g_token_kind_names[ TokenKind_thread_local ] = CStrView( "thread_local" );
	g_token_kind_names[ TokenKind__Thread_local ] = CStrView( "_Thread_local" );
	g_token_kind_names[ TokenKind_register ] = CStrView( "register" );

	// Linkage specifiers
	g_token_kind_names[ TokenKind_extern ] = CStrView( "extern" );
	g_token_kind_names[ TokenKind_static ] = CStrView( "static" );

	g_token_kind_names[ TokenKind_alignas ] = CStrView( "alignas" );
	g_token_kind_names[ TokenKind_alignof ] = CStrView( "alignof" );
	g_token_kind_names[ TokenKind_constexpr ] = CStrView( "constexpr" );
	g_token_kind_names[ TokenKind_false ] = CStrView( "false" );
	g_token_kind_names[ TokenKind_inline ] = CStrView( "inline" );
	g_token_kind_names[ TokenKind_nullptr ] = CStrView( "nullptr" );
	g_token_kind_names[ TokenKind_sizeof ] = CStrView( "sizeof" );
	g_token_kind_names[ TokenKind_static_assert ] = CStrView( "static_assert" );
	g_token_kind_names[ TokenKind_true ] = CStrView( "true" );
	g_token_kind_names[ TokenKind_typeof ] = CStrView( "typeof" );
	g_token_kind_names[ TokenKind_typeof_unequal ] = CStrView( "typeof_unequal" );
	g_token_kind_names[ TokenKind__Alignas ] = CStrView( "_Alignas" );
	g_token_kind_names[ TokenKind__Alignof ] = CStrView( "_Alignof" );
	g_token_kind_names[ TokenKind__Generic ] = CStrView( "_Generic" );
	g_token_kind_names[ TokenKind__Noreturn ] = CStrView( "_Noreturn" );
	g_token_kind_names[ TokenKind__Static_assert ] = CStrView( "_Static_assert" );

	/* Preprocessor keywords */

	// TokenKind_if
	g_token_kind_names[ TokenKind_elif ] = CStrView( "elif" );
	// TokenKind_else,
	g_token_kind_names[ TokenKind_endif ] = CStrView( "endif" );
	g_token_kind_names[ TokenKind_ifdef ] = CStrView( "ifdef" );
	g_token_kind_names[ TokenKind_ifndef ] = CStrView( "ifndef" );
	g_token_kind_names[ TokenKind_elifdef ] = CStrView( "elifdef" );
	g_token_kind_names[ TokenKind_elifndef ] = CStrView( "elifndef" );
	g_token_kind_names[ TokenKind_define ] = CStrView( "define" );
	g_token_kind_names[ TokenKind_undef ] = CStrView( "undef" );
	g_token_kind_names[ TokenKind_include ] = CStrView( "include" );
	g_token_kind_names[ TokenKind_embed ] = CStrView( "embed" );
	g_token_kind_names[ TokenKind_line ] = CStrView( "line" );
	g_token_kind_names[ TokenKind_error ] = CStrView( "error" );
	g_token_kind_names[ TokenKind_warning ] = CStrView( "warning" );
	g_token_kind_names[ TokenKind_pragma ] = CStrView( "pragma" );
	g_token_kind_names[ TokenKind_defined ] = CStrView( "defined" );
	g_token_kind_names[ TokenKind__has_include ] = CStrView( "_has_include" );
	g_token_kind_names[ TokenKind__has_embed ] = CStrView( "_has_embed" );
	g_token_kind_names[ TokenKind__has_c_attribute ] = CStrView( "_has_c_attribute" );

	// Recognized outside of the preprocessor
	g_token_kind_names[ TokenKind__Pragma ] = CStrView( "_Pragma" );

	/* Conditionally-supported keywords */

	g_token_kind_names[ TokenKind_asm ] = CStrView( "asm" );
	g_token_kind_names[ TokenKind_fortran ] = CStrView( "fortran" );

	/* Group 2: Operators */

	// Assignment operators
	g_token_kind_names[ TokenKind_Assign ] = CStrView( "=" );
	g_token_kind_names[ TokenKind_PlusAssign ] = CStrView( "+=" );
	g_token_kind_names[ TokenKind_MinusAssign ] = CStrView( "-=" );
	g_token_kind_names[ TokenKind_TimesAssign ] = CStrView( "*=" );
	g_token_kind_names[ TokenKind_DivideAssign ] = CStrView( "/=" );
	g_token_kind_names[ TokenKind_ModuloAssign ] = CStrView( "&=" );
	g_token_kind_names[ TokenKind_BitwiseAndAssign ] = CStrView( "&=" );
	g_token_kind_names[ TokenKind_BitwiseOrAssign ] = CStrView( "!=" );
	g_token_kind_names[ TokenKind_BitwiseXorAssign ] = CStrView( "^=" );
	g_token_kind_names[ TokenKind_BitwiseNotAssign ] = CStrView( "~=" );
	g_token_kind_names[ TokenKind_BitwiseShiftLeftAssign ] = CStrView( "<<=" );
	g_token_kind_names[ TokenKind_BitwiseShiftRightAssign ] = CStrView( ">>=" );

	// Arithmetic operators
	g_token_kind_names[ TokenKind_Plus ] = CStrView( "+" );
	g_token_kind_names[ TokenKind_Minus ] = CStrView( "-" );
	g_token_kind_names[ TokenKind_Star ] = CStrView( "*" );
	g_token_kind_names[ TokenKind_Divide ] = CStrView( "/" );
	g_token_kind_names[ TokenKind_Modulo ] = CStrView( "%" );
	g_token_kind_names[ TokenKind_Increment ] = CStrView( "++" );
	g_token_kind_names[ TokenKind_Decrement ] = CStrView( "--" );

	// Comparison operators
	g_token_kind_names[ TokenKind_Equals ] = CStrView( "==" );
	g_token_kind_names[ TokenKind_NotEquals ] = CStrView( "!=" );
	g_token_kind_names[ TokenKind_Less ] = CStrView( "<" );
	g_token_kind_names[ TokenKind_LessEquals ] = CStrView( "<=" );
	g_token_kind_names[ TokenKind_Greater ] = CStrView( ">" );
	g_token_kind_names[ TokenKind_GreaterEquals ] = CStrView( ">=" );

	// Logical operators
	g_token_kind_names[ TokenKind_LogicalAnd ] = CStrView( "&&" );
	g_token_kind_names[ TokenKind_LogicalOr ] = CStrView( "||" );
	g_token_kind_names[ TokenKind_LogicalNot ] = CStrView( "!" );

	// Bitwise operators
	g_token_kind_names[ TokenKind_BitwiseAnd ] = CStrView( "&" );
	g_token_kind_names[ TokenKind_BitwiseOr ] = CStrView( "|" );
	g_token_kind_names[ TokenKind_BitwiseXor ] = CStrView( "^" );
	g_token_kind_names[ TokenKind_BitwiseNot ] = CStrView( "~" );
	g_token_kind_names[ TokenKind_BitwiseShiftLeft ] = CStrView( "<<" );
	g_token_kind_names[ TokenKind_BitwiseShiftRight ] = CStrView( ">>" );

	// Access operators
	g_token_kind_names[ TokenKind_Dot ] = CStrView( "." );
	g_token_kind_names[ TokenKind_Arrow ] = CStrView( "->" );
	g_token_kind_names[ TokenKind_Ampersand ] = CStrView( "&" );

	// Grouping & Separator punctuators
	g_token_kind_names[ TokenKind_LeftParenthesis ] = CStrView( "(" );
	g_token_kind_names[ TokenKind_RightParenthesis ] = CStrView( ")" );
	g_token_kind_names[ TokenKind_LeftBracket ] = CStrView( "[" );
	g_token_kind_names[ TokenKind_RightBracket ] = CStrView( "]" );
	g_token_kind_names[ TokenKind_LeftCurlyBrace ] = CStrView( "{" );
	g_token_kind_names[ TokenKind_RightCurlyBrace ] = CStrView( "}" );
	g_token_kind_names[ TokenKind_Colon ] = CStrView( ":" );
	g_token_kind_names[ TokenKind_Semicolon ] = CStrView( ";" );
	g_token_kind_names[ TokenKind_Comma ] = CStrView( "," );

	g_token_kind_names[ TokenKind_Pointer ] = CStrView( "*" );
	g_token_kind_names[ TokenKind_AddressOf ] = CStrView( "&" );
	g_token_kind_names[ TokenKind_PointerDereference ] = CStrView( "*" );
}

StringView_ASCII token_kind_name( TokenKind kind ) {
	return g_token_kind_names[ kind ];
}

typedef struct Token {
	TokenKind kind;
	StringView_ASCII view;
	u32 line;
	u32 column;
} Token;

typedef void ( * LexerReportFn )( struct Lexer *lexer, const char *format, ... );

// Default Lexer report functions.
void lexer_report_info( struct Lexer *lexer, const char *format, ... );
void lexer_report_warning( struct Lexer *lexer, const char *format, ... );
void lexer_report_error( struct Lexer *lexer, const char *format, ... );

typedef void ( * ParserReportFn )( struct Parser *parser, const char *format, ... );

// Default Parser report funcitons.
void parser_report_info( struct Parser *parser, const char *format, ... );
void parser_report_warning( struct Parser *parser, const char *format, ... );
void parser_report_error( struct Parser *parser, const char *format, ... );

typedef struct Lexer {
	StringView_ASCII file_path;
	String_ASCII source;
	StringView_ASCII cursor;
	u32 line;  // Index, 0-based.
	u32 column;  // Index, 0-based.

	LexerReportFn report_info;
	LexerReportFn report_warning;
	LexerReportFn report_error;
} Lexer;

typedef struct Parser {
	Lexer lexer;

	ParserReportFn report_info;
	ParserReportFn report_warning;
	ParserReportFn report_error;

	// Current state flags
	unsigned int
	in_global_scope : 1;
} Parser;

Parser parser_create( Lexer *lexer ) {
	Parser parser = { 0 };
	parser.lexer = *lexer;
	parser.report_info = parser_report_info;
	parser.report_warning = parser_report_warning;
	parser.report_error = parser_report_error;
	return parser;
}

// void parser_parse_file( Parser *parser, StringView_ASCII file_path ) {
// 	parser->lexer = lexer_open_file( file_path );
// }

void parser_report_info( Parser *parser, const char *format, ... ) {
	const Lexer *const lexer = &parser->lexer;
	fprintf( stdout, "Parser: "FMT_StrView":%u:%u: Info: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stdout, format, args );
	va_end( args );
}

void parser_report_warning( Parser *parser, const char *format, ... ) {
	const Lexer *const lexer = &parser->lexer;
	fprintf( stdout, "Parser: "FMT_StrView":%u:%u: Warning: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stdout, format, args );
	va_end( args );
}

void parser_report_error( Parser *parser, const char *format, ... ) {
	const Lexer *const lexer = &parser->lexer;
	fprintf( stderr, "Parser: "FMT_StrView":%u:%u: Error: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stderr, format, args );
	va_end( args );
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

// ASCII
bool string_is_alpha( char c ) {
	bool alpha = ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' );
	return alpha;
}

// ASCII
bool string_is_numeric( char c ) {
	bool numeric = ( c >= '0' && c <= '9' );
	return numeric;
}

// ASCII
bool string_is_alpha_numeric( char c ) {
	bool alpha_numeric = ( c >= '0' && c <= '9' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= 'a' && c <= 'z' );
	return alpha_numeric;
}

// ASCII
bool string_is_whitespace( char c ) {
	bool whitespace = ( c == ' ' || c == '\t' || c == '\r' || c == '\n' );
	return whitespace;
}

bool lexer_is_at_end( Lexer *lexer ) {
	// Is the current character at source cursor pointer an End-Of-File or a null-terminator ('\0')?
	char c = lexer->cursor.base.data[ 0 ];
	bool at_end = ( c == EOF || c == '\0' );
	return at_end;
}

static bool lexer__is_at_end( const char *cursor ) {
	// Is the current character at source cursor pointer an End-Of-File or a null-terminator ('\0')?
	char c = cursor[ 0 ];
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

typedef enum LexerResult {
	Lexer_OK = 0,
	Lexer_EOF
} LexerResult;

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

// Returns parsed Identifier length.
static LexerResult lexer__parse_token_identifier( Lexer *lexer, Token *token, bool eat_token ) {
	const char *cursor = lexer->cursor.base.data;

	cursor += 1;
	while ( string_is_alpha_numeric( *cursor ) || *cursor == '_' ) {
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

// Returns parsed number literal length.
static LexerResult lexer__parse_token_literal_number( Lexer *lexer, Token *token, bool eat_token ) {
	const char *cursor = lexer->cursor.base.data;

	// lexer__eat_char_unchecked( lexer );
	cursor += 1;
	while ( string_is_numeric( *cursor ) || *cursor == '.' || *cursor == 'f' || *cursor == 'u' || *cursor == 'l' ) {
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

// Returns parsed string literal length.
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
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "true" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_true, 4, eat_token );
	} else if ( string_view_starts_with( lexer->cursor, CStrView( "false" ) ) ) {
		return lexer__eat_token_keyword( lexer, token, TokenKind_false, 5, eat_token );
	} else if ( string_is_numeric( **c ) ) {
		return lexer__parse_token_literal_number( lexer, token, eat_token );
	} else if ( string_is_alpha( **c ) ) {
		return lexer__parse_token_identifier( lexer, token, eat_token );
	}

	return Lexer_EOF;
}

LexerResult lexer_peak_next_token( Lexer *lexer, Token *token ) {
	return lexer__next_token( lexer, token, /* eat_token */ false );
}

LexerResult lexer_next_token( Lexer *lexer, Token *token ) {
	return lexer__next_token( lexer, token, /* eat_token */ true );
}

typedef enum TypeIds {
	// Primitive types
	TypeId_void = 0,
	TypeId_char,
	TypeId_bool,
	TypeId_short,  // short int
	TypeId_int,
	TypeId_long,  // long int
	TypeId_longlong,  // long long int
	TypeId_float,
	TypeId_double,
	TypeId_long_double,
	TypeId_enum,

	// Compound types
	TypeId_union,
	TypeId_struct,

	TypeId_COUNT
} TypeIds;
typedef u16 TypeId;

typedef enum ETypeModifierBits {
	TypeModifierBit_Pointer = ( 1 << 0 ),
	TypeModifietBit_ConstPointer = ( 1 << 1 ),
	TypeModifietBit_Unsigned = ( 1 << 2 ),
	TypeModifietBit_Static = ( 1 << 3 ),
	TypeModifietBit_Const = ( 1 << 4 ),
	TypeModifietBit_Volatile = ( 1 << 5 ),
	TypeModifietBit_Restrict = ( 1 << 6 )
} ETypeModifierBits;
typedef u32 TypeModifierBits;

typedef struct Type {
	TypeId id;
	union {
		TypeModifierBits modifiers;
		unsigned int
		is_pointer : 1,  // `const T *`
		is_const_pointer : 1,  // `T *const `
		is_unsigned : 1,

		is_static : 1,
		is_const : 1,
		is_volatile : 1,
		is_restrict : 1;
	};
} Type;

/*
	ONLY on Windows x86_64 LLP64
	Type           ILP64   LP64   LLP64
	char              8      8       8
	short            16     16      16
	int              64     32      32
	long             64     64      32
	long long        64     64      64
	pointer          64     64      64
*/
u32 type_size( Type *type ) {
	if ( type->is_pointer )  return 8; // x86_64
	switch ( type->id ) {
		case TypeId_void:
			return 0;  // void
		case TypeId_char:
		case TypeId_bool:
			return 1;  // char, bool, _Bool
		case TypeId_short:
			return 2;
		case TypeId_int:
		case TypeId_long:
		case TypeId_float:
			return 4;
		case TypeId_longlong:
		case TypeId_double:
			return 8;
		case TypeId_long_double:
			return 16;
		default:
			AssertMessage( type->id > TypeId_COUNT, "Cannot evaluate type size: unknown TypeId" );
			return 0;
	}
}

typedef enum StatementKinds {
	StatementKind_FunctionCall = 0,
	StatementKind_Return,
	StatementKind_VariableDeclaration,
} StatementKinds;
typedef u32 StatementKind;

typedef struct Statement {
	StatementKind kind;
} Statement;

typedef struct Statement_Return {
	/* Base */
	Statement statement;

	Token expr;
} Statement_Return;

typedef struct Statement_FunctionCall {
	/* Base */
	Statement statement;

	String_ASCII function_name;
	DECL_Dynamic_Array( Token, arguments );
} Statement_FunctionCall;

// x84_64
typedef struct ConstantValue {
	union {
		union {
			bool as_bool;  // 1 byte
			char as_char;  // 1 byte
			s64 as_int;  // 8 bytes
			u64 as_uint;  // 8 bytes
			float as_float;  // 4 bytes
			double as_double;  // 8 bytes
			long double as_long_double;  // 8 bytes on Windows, 16 bytes(?) on Linux
		} value;  // Max: 8 bytes (Windows) or 16 bytes (Linux)
		struct {
			const char *data;  // 8 bytes
			u64 size;  // 8 bytes
		} string;  // Total: 16 bytes
	};
} ConstantValue;

typedef struct Statement_VariableDeclaration {
	/* Base */
	Statement statement;

	Type type;
	String_ASCII name;
	ConstantValue value;
} Statement_VariableDeclaration;

typedef struct Expression {
	int _;
} Expression;

typedef struct FunctionDefinition {
	String_ASCII name;
	DECL_Dynamic_Array( Statement *, body );
	Type return_type;
} FunctionDefinition;

String_ASCII new_token_kind_list_string( Allocator *allocator, u32 initial_capacity, DECL_Array_View( TokenKind, token_kinds ) ) {
	Assert( token_kinds.item_size == sizeof( TokenKind ) );
	if ( allocator == NULL )  allocator = g_sys_allocator;
	if ( initial_capacity == 0 )  initial_capacity = 128;
	String_ASCII string = string_new( allocator, initial_capacity );
	string_add_char( &string, '[' );
	if ( token_kinds.item_count > 0 ) {
		// First item.  At this point, there is at least one.
		string_add_char( &string, '\'' );
		string_add_view( &string, token_kind_name( token_kinds.data[ 0 ] ) );
		string_add_char( &string, '\'' );
		// The rest of the items, appended after a comma.
		ForItFrom ( u32, TokenKind, token_kinds.data, 1, token_kinds.item_count ) {
			string_add_view( &string, CStrView( ", '" ) );
			string_add_view( &string, token_kind_name( it ) );
			string_add_char( &string, '\'' );
		}}
	}
	string_add_char( &string, ']' );
	return string;
}

bool lexer_expect_many_this_token_kinds( Lexer *lexer, Token *token, DECL_Array_View( TokenKind, expected_kinds ) ) {
	// Check if this token kind is any of expected kinds.
	ForIt ( u32, TokenKind, expected_kinds.data, expected_kinds.item_count ) {
		if ( token->kind == it ) {
			return true;
		}
	}}

	// If this token kind does not match any of expected kinds, report an error.
	String_ASCII token_kind_list_string = new_token_kind_list_string( g_sys_allocator, 0, expected_kinds );
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
			String_ASCII token_kind_list_string = new_token_kind_list_string( g_sys_allocator, 0, expected_kinds );
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

bool parser_parse_type( Parser *parser, Token *token, Type *type ) {
	const Lexer *const lexer = &parser->lexer;

	/*
	TokenKind type_modifier_tokens[] = {
		TokenKind_short, TokenKind_long,
		TokenKind_signed, TokenKind_unsigned,
		TokenKind_const,
		TokenKind_volatile,
		TokenKind_restrict
	};
	DECL_Array_View( TokenKind, type_modifier_tokens_view ) = array_view( type_modifier_tokens, sizeof( TokenKind ), ARRAY_SIZE( type_modifier_tokens ) );

	TokenKind type_id_tokens[] = {
		TokenKind_void,
		TokenKind_bool,
		TokenKind__Bool,
		TokenKind_char,
		TokenKind_int,
		TokenKind_float,
		TokenKind_double,
		TokenKind_enum,
		TokenKind_struct
	};
	DECL_Array_View( TokenKind, type_id_tokens_view ) = array_view( type_modifier_tokens, sizeof( TokenKind ), ARRAY_SIZE( type_modifier_tokens ) );
	*/

	Type parsed_type = { 0 };
	bool type_parsed = false;
	// ForIt( u32, TokenKind, type_modifier_tokens_view.data, type_modifier_tokens_view.item_count ) {
	// 	if ( it == TokenKind_short || it == TokenKind_long )  type_parsed = true;

	// }}

	// bool is_modifier = false;
	// ForIt( u32, TokenKind, type_modifier_tokens_view.data, type_modifier_tokens_view.item_count ) {
	// 	is_modifier |= ( token.kind == it );
	// 	if ( is_modifier )  break;
	// }}
	switch ( token->kind ) {
		case TokenKind_void:  parsed_type.id = TypeId_void; break;
		case TokenKind_bool:  parsed_type.id = TypeId_bool; break;
		case TokenKind__Bool:  parsed_type.id = TypeId_bool; break;
		case TokenKind_char:  parsed_type.id = TypeId_char; break;
		case TokenKind_int:  parsed_type.id = TypeId_int; break;
		case TokenKind_float:  parsed_type.id = TypeId_float; break;
		case TokenKind_double:  parsed_type.id = TypeId_double; break;
		case TokenKind_enum:  parsed_type.id = TypeId_enum; break;
		case TokenKind_union:  parsed_type.id = TypeId_union; break;
		case TokenKind_struct:  parsed_type.id = TypeId_struct; break;
		default:
			parser->report_error( parser, "Unexpected type '"FMT_StrView"'.", ARG_StrView( token->view ) );
			return false;
	}

	if ( lexer_next_token( lexer, token ) != Lexer_OK )  return false;  // Eat type tokens
	*type = parsed_type;
	return true;
}

/*
bool parse_expression( Lexer *lexer ) {
	Token expr;
	TokenKind expected_tokens[] = { TokenKind_LiteralNumber, TokenKind_LiteralString };
	ARRAY_TYPE( TokenKind )
	Array_View expected_tokens_view = array_view( expected_tokens, sizeof( TokenKind ), ARRAY_SIZE( expected_tokens ) );
	if ( !lexer_expect_many_token_kinds( lexer, expected_tokend_view, &expr ) )  return false;
}
*/

bool parser_parse_argument_list( Parser *parser, DECL_Dynamic_Array( Token, *arguments ) ) {
	const Lexer *const lexer = &parser->lexer;
	Token token;
	// Beginning of argument list
	if ( !lexer_expect_next_token_kind( lexer, &token, TokenKind_LeftParenthesis ) )  return false;

	// TokenKind expected_tokens[] = { TokenKind_LiteralNumber, TokenKind_LiteralString, TokenKind_RightParenthesis };
	// DECL_Array_View( TokenKind, expected_tokens_view ) = array_view( expected_tokens, sizeof( TokenKind ), ARRAY_SIZE( expected_tokens ) );
	Token next_token;
	if ( lexer_peak_next_token( lexer, &next_token ) != Lexer_OK )  return false;
	// if ( !lexer_expect_many_token_kinds( lexer, expected_tokens_view, &token ) )  return false;
	while ( next_token.kind != TokenKind_RightParenthesis ) {  // End of argument list
		if ( lexer_next_token( lexer, &token ) != Lexer_OK )  return false;
		darray_add( arguments, &token );  // Add argument token (Only ConstantValue for now).

		// Advance to next argument
		// if ( !lexer_expect_many_token_kinds( lexer, expected_tokens_view, &token ) )  return false;
		if ( lexer_peak_next_token( lexer, &next_token ) != Lexer_OK )  return false;
	}

	if ( lexer_next_token( lexer, &token ) != Lexer_OK )  return false;  // Eat ')'

	return true;
}

bool token_kind_is_of_declaration( TokenKind kind ) {
	return ( kind >= TokenKind__DeclarablesBegin && kind <= TokenKind__DeclarablesEnd );
}

bool type_id_is_of_integer( TypeId id ) {
	switch ( id ) {
		case TypeId_char:
		case TypeId_bool:
		case TypeId_short:
		case TypeId_int:
		case TypeId_long:
		case TypeId_longlong:
		case TypeId_enum:
			return true;
		default:
			return false;
	}
}

bool type_id_is_of_float( TypeId id ) {
	switch ( id ) {
		case TypeId_float:
		case TypeId_double:
		case TypeId_long_double:
			return true;
		default:
			return false;
	}
}

bool parser_parse_statement( Parser *parser, Token *token, DECL_Dynamic_Array( Statement *, *statements ) ) {
	const Lexer *const lexer = &parser->lexer;
	// It is expected that `token` is already acquired with `lexer_next_token()` or similar.
	if ( token_kind_is_of_declaration( token->kind ) ) {
		// Beginning of declaration statement.
		Type type;
		if ( !parser_parse_type( parser, token, &type ) )  return false;
		if ( !lexer_expect_this_token_kind( lexer, token, TokenKind_Identifier ) )  return false;
		Statement_VariableDeclaration *var_decl = CC_Allocate( g_sys_allocator, 1, Statement_VariableDeclaration );
		var_decl->statement.kind = StatementKind_VariableDeclaration;
		var_decl->type = type;
		var_decl->name = string_new_copy( g_sys_allocator, token->view );

		if ( lexer_next_token( lexer, token ) != Lexer_OK )  return false;
		// WARNING: Only primitive type declarations for now.
		if ( token->kind == TokenKind_Semicolon ) {
			// Declaration without assignment.
			var_decl->value = ( ConstantValue ) { 0 };
			darray_add( statements, &var_decl );
		} else if ( token->kind == TokenKind_Assign ) {
			// Declaration with assigment.
			if ( type.id == TypeId_char ) {
				if ( lexer_next_token( lexer, token ) != Lexer_OK )  return false;
				if ( token->kind == TokenKind_LiteralChar ) {
					char char_value = *( char * )token->view.base.data;
					var_decl->value.value.as_char = char_value;
				} else if ( token->kind == TokenKind_LiteralNumber ) {
					s64 int_value = atoll( token->view.base.data );
					char char_value = ( char )int_value;
					var_decl->value.value.as_char = char_value;
				} else {
					return false;
				}
			} else if ( type.id == TypeId_bool ) {
				if ( lexer_next_token( lexer, token ) != Lexer_OK )  return false;
				if ( token->kind == TokenKind_true ) {
					var_decl->value.value.as_bool = true;
				} else if ( token->kind == TokenKind_false ) {
					var_decl->value.value.as_bool = false;
				} else if ( token->kind == TokenKind_LiteralChar ) {
					bool bool_value = ( token->view.base.data ) ? true : false;
					var_decl->value.value.as_bool = bool_value;
				} else if ( token->kind == TokenKind_LiteralString ) {
					bool bool_value = ( token->view.base.data ) ? true : false;
					var_decl->value.value.as_bool = bool_value;
				} else if ( token->kind == TokenKind_LiteralNumber ) {
					s64 int_value = atoll( token->view.base.data );
					bool bool_value = ( int_value != 0 ) ? true : false;
					var_decl->value.value.as_bool = bool_value;
				} else {
					return false;
				}
			} else if ( type.id == TypeId_char ) {
				if ( lexer_next_token( lexer, token ) != Lexer_OK )  return false;
				if ( token->kind == TokenKind_true ) {
					var_decl->value.value.as_char = true;
				} else if ( token->kind == TokenKind_false ) {
					var_decl->value.value.as_char = false;
				} else if ( token->kind == TokenKind_LiteralChar ) {
					char char_value = *( char * )token->view.base.data;
					var_decl->value.value.as_char = char_value;
				} else if ( token->kind == TokenKind_LiteralString ) {
					char char_value = *( char * )token->view.base.data;
					var_decl->value.value.as_char = char_value;
				} else if ( token->kind == TokenKind_LiteralNumber ) {
					char char_value = ( char )atoll( token->view.base.data );
					var_decl->value.value.as_char = char_value;
				} else {
					return false;
				}
			} else if ( type_id_is_of_integer( type.id ) ) {
				// if ( lexer_next_token( lexer, token ) != Lexer_OK )  return false;
				if ( !lexer_expect_next_token_kind( lexer, token, TokenKind_LiteralNumber ) )  return false;
				s64 int_value = atoll( token->view.base.data );
				var_decl->value.value.as_int = int_value;
			} else if ( type_id_is_of_float( type.id ) ) {
				// if ( lexer_next_token( lexer, token ) != Lexer_OK )  return false;
				if ( !lexer_expect_next_token_kind( lexer, token, TokenKind_LiteralNumber ) )  return false;
				double float_value = atof( token->view.base.data );
				var_decl->value.value.as_double = float_value;
			}

			// TODO: Deallocate in any other error case too!
			if ( !lexer_expect_next_token_kind( lexer, token, TokenKind_Semicolon ) ) {
				CC_Deallocate( g_sys_allocator, var_decl );
				return false;
			}

			darray_add( statements, &var_decl );
		} else {
			// Declaration is not ended with ';' (semicolon)
			StringView_ASCII token_name = token_kind_name( token->kind );
			parser->report_error( parser, "Variable declaration is not ended with '"FMT_StrView"'.",
				ARG_StrView( token_name ) );

			// TODO: Deallocate in any other error case too!
			CC_Deallocate( g_sys_allocator, var_decl );
			return false;
		}
	} else if ( token->kind == TokenKind_return ) {
		// Return statement.
		TokenKind return_tokens[] = { TokenKind_LiteralNumber, TokenKind_LiteralString };
		DECL_Array_View( TokenKind, return_tokens_view ) = array_view( return_tokens, sizeof( TokenKind ), ARRAY_SIZE( return_tokens ) );
		if ( !lexer_expect_many_next_token_kinds( lexer, token, return_tokens_view ) )  return false;
		Statement_Return *return_statement = CC_Allocate( g_sys_allocator, 1, Statement_Return );
		return_statement->statement.kind = StatementKind_Return;
		return_statement->expr = *token;

		// TODO: Deallocate in any other error case too!
		if ( !lexer_expect_next_token_kind( lexer, token, TokenKind_Semicolon ) ) {
			CC_Deallocate( g_sys_allocator, return_statement );
			return false;
		}

		darray_add( statements, &return_statement );
	} else if ( token->kind == TokenKind_Identifier ) {
		// Function call statement.
		DECL_Dynamic_Array( Token, arguments ) = DARRAY_NEW( g_sys_allocator, Token, 4 );
		if ( !parser_parse_argument_list( parser, &arguments ) ) return false;
		Statement_FunctionCall *func_call_statement = CC_Allocate( g_sys_allocator, 1, Statement_FunctionCall );
		func_call_statement->statement.kind = StatementKind_FunctionCall;
		func_call_statement->function_name = string_new_copy( g_sys_allocator, token->view );
		func_call_statement->arguments = arguments;

		// TODO: Deallocate in any other error case too!
		if ( !lexer_expect_next_token_kind( lexer, token, TokenKind_Semicolon ) ) {
			CC_Deallocate( g_sys_allocator, func_call_statement );
			return false;
		}

		darray_add( statements, &func_call_statement );
	}

	return true;
}

bool parser_parse_statement_block( Parser *parser, DECL_Dynamic_Array( Statement *, *statements ) ) {
	const Lexer *const lexer = &parser->lexer;
	Token token;
	if ( !lexer_expect_next_token_kind( lexer, &token, TokenKind_LeftCurlyBrace ) )  return false;

	Token next_token;
	if ( lexer_peak_next_token( lexer, &next_token ) != Lexer_OK )  return false;
	while ( next_token.kind != TokenKind_RightCurlyBrace ) {
		if ( lexer_next_token( lexer, &token ) != Lexer_OK )  return false;
		if ( !parser_parse_statement( parser, &token, statements ) )  return false;

		// Advance to next statement
		if ( lexer_peak_next_token( lexer, &next_token ) != Lexer_OK )  return false;
	}

	if ( lexer_next_token( lexer, &token ) != Lexer_OK )  return false;  // Eat '}'

	return true;
}

bool parser_parse_function_definition( Parser *parser, FunctionDefinition *func_defn ) {
	const Lexer *const lexer = &parser->lexer;

	Token token;
	// Return type
	if ( lexer_next_token( lexer, &token ) != Lexer_OK )  return false;
	if ( !parser_parse_type( parser, &token, &func_defn->return_type ) )  return false;

	// Name
	if ( !lexer_expect_this_token_kind( lexer, &token, TokenKind_Identifier ) )  return false;
	func_defn->name = string_new_copy( g_sys_allocator, token.view );

	// Argument/parameter list
	if ( !lexer_expect_next_token_kind( lexer, &token, TokenKind_LeftParenthesis ) )  return false;
	if ( !lexer_expect_next_token_kind( lexer, &token, TokenKind_RightParenthesis ) )  return false;

	// Body
	func_defn->body = DARRAY_NEW( g_sys_allocator, Statement *, 8 );
	if ( !parser_parse_statement_block( lexer, &func_defn->body ) )  return false;

	return true;
}

int main( int arguments_count, char *arguments[] ) {

	console_init( /* code_page */ CP_UTF8, /* title */ L"C Compiler" );
	allocators_init();
	token_kind_names_init();

	DECL_Dynamic_Array( StringView_ASCII, args ) = darray_new( g_sys_allocator, sizeof( StringView_ASCII ), arguments_count );

	// Convert all C-style string arguments to `StringView`s
	ForIt( s32, char *, arguments, arguments_count ) {
		StringView_ASCII arg_view = CStrView( it );
		darray_add( &args, &arg_view );
	}}

	console_print( "Arguments:\n" );
	ForIt( u32, StringView_ASCII, args.data, args.item_count ) {
		console_print( "[%d]: " FMT_StrView "\n", it_index, ARG_StrView( it ) );
	}}

	FunctionDefinition func_defn = ( FunctionDefinition ) { 0 };
	/* Per each file... */
	ForItFrom( u32, StringView_ASCII, args.data, 1, args.item_count ) {
		Lexer lexer = lexer_open_file( it );

		Parser parser = parser_create( &lexer );

		parser_parse_function_definition( &parser, &func_defn );
		/*
		u32 result = lexer_next_token( &lexer, &token );
		while ( result == Lexer_OK ) {
			StringView_ASCII token_name = g_token_kind_names[ token.kind ].na;
			printf( FMT_StrView":%u:%u: ("FMT_StrView") "FMT_StrView"\n",
				ARG_StrView( lexer.file_path ), token.line + 1, token.column + 1, ARG_StrView( token_name ), ARG_StrView( token.view ) );
			result = lexer_next_token( &lexer, &token );
		}
		*/
		int k = 123;
	}}

/*
	DECL_Dynamic_Array( Statement *, statements ) = DARRAY_NEW( g_sys_allocator, Statement *, 3 );
	Statement_FunctionCall *func_call = CC_Allocate( g_sys_allocator, 1, Statement_FunctionCall );
	func_call->statement.kind = StatementKind_FunctionCall;
	func_call->function_name = string_new_copy( g_sys_allocator, CStrView( "test" ) );
	func_call->arguments = DARRAY_NEW( g_sys_allocator, Token, 2 );
	Token func_call_arg = ( Token ) {
		.kind = TokenKind_LiteralString,
		.view = string_view( &func_call->function_name, 0, 0 ),
		.line = 123,
		.column = 321
	};
	darray_add( &func_call->arguments, &func_call_arg );

	darray_add( &statements, &func_call );

	Statement_VariableDeclaration *var_decl = CC_Allocate( g_sys_allocator, 1, Statement_VariableDeclaration );
	var_decl->statement.kind = StatementKind_VariableDeclaration;
	var_decl->type = ( Type ) {
		.id = TypeId_int,
		.modifiers = 0
	};
	var_decl->value = ( ConstantValue ) { 0 };
	var_decl->value.value.as_int = 5;
	darray_add( &statements, &var_decl );

	Statement_Return *return_statement = CC_Allocate( g_sys_allocator, 1, Statement_Return );
	return_statement->statement.kind = StatementKind_Return;
	return_statement->expr = ( Token ) {
		.kind = TokenKind_LiteralNumber,
		.view = CStrView( "0" ),
		.line = 11,
		.column = 22
	};
	darray_add( &statements, &return_statement );

	ForIt( u32, Statement *, statements.data, statements.item_count ) {
		if ( it->kind == StatementKind_FunctionCall ) {
			Statement_FunctionCall *s = it;
			int k = 123;
		} else if ( it->kind == StatementKind_VariableDeclaration ) {
			Statement_VariableDeclaration *s = it;
			int k = 123;
		} else if ( it->kind == StatementKind_Return ) {
			Statement_Return *s = it;
			int k = 123;
		}
	}}
	*/

	ForIt( u32, Statement *, func_defn.body.data, func_defn.body.item_count ) {
		if ( it->kind == StatementKind_FunctionCall ) {
			Statement_FunctionCall *s = ( Statement_FunctionCall * )it;
			volatile int k = 123;
		} else if ( it->kind == StatementKind_VariableDeclaration ) {
			Statement_VariableDeclaration *s = ( Statement_VariableDeclaration * )it;
			StatementKinds statement_kind = s->statement.kind;
			TypeIds type_id = s->type.id;
			volatile int k = 123;
		} else if ( it->kind == StatementKind_Return ) {
			Statement_Return *s = ( Statement_Return * )it;
			volatile int k = 123;
		}
	}}

	return 0;
}
