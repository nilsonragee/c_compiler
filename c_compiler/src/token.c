#include "token.h"

static struct G_Token {
	StringView_ASCII kind_names[ TokenKind_COUNT ];
} g_token;

void token_kind_names_init() {
	// Basic
	g_token.kind_names[ TokenKind_Identifier ] = CStrView( "Identifier" );
	g_token.kind_names[ TokenKind_LiteralNumber ] = CStrView( "LiteralNumber" );
	g_token.kind_names[ TokenKind_LiteralChar ] = CStrView( "LiteralChar" );
	g_token.kind_names[ TokenKind_LiteralString ] = CStrView( "LiteralString" );

	// Types
	g_token.kind_names[ TokenKind_void ] = CStrView( "void" );
	g_token.kind_names[ TokenKind_bool ] = CStrView( "bool" );
	g_token.kind_names[ TokenKind__Bool ] = CStrView( "_Bool" );
	g_token.kind_names[ TokenKind_char ] = CStrView( "char" );
	g_token.kind_names[ TokenKind_int ] = CStrView( "int" );
	g_token.kind_names[ TokenKind_float ] = CStrView( "float" );
	g_token.kind_names[ TokenKind_double ] = CStrView( "double" );
	g_token.kind_names[ TokenKind__BitInt ] = CStrView( "_BitInt" );
	g_token.kind_names[ TokenKind__Decimal32 ] = CStrView( "_Decimal32" );
	g_token.kind_names[ TokenKind__Decimal64 ] = CStrView( "_Decimal64" );
	g_token.kind_names[ TokenKind__Decimal128 ] = CStrView( "_Decimal128" );

	// Type specifiers
	g_token.kind_names[ TokenKind_short ] = CStrView( "short" );
	g_token.kind_names[ TokenKind_long ] = CStrView( "long" );
	g_token.kind_names[ TokenKind_unsigned ] = CStrView( "unsigned" );
	g_token.kind_names[ TokenKind_signed ] = CStrView( "signed" );
	g_token.kind_names[ TokenKind__Imaginary ] = CStrView( "_Imaginary" );
	g_token.kind_names[ TokenKind__Complex ] = CStrView( "_Complex" );
	g_token.kind_names[ TokenKind__Atomic ] = CStrView( "_Atomic" );
	// Additional type qualifiers
	g_token.kind_names[ TokenKind_const ] = CStrView( "const" );
	g_token.kind_names[ TokenKind_volatile ] = CStrView( "volatile" );
	g_token.kind_names[ TokenKind_restrict ] = CStrView( "restrict" );

	// User-definable types
	g_token.kind_names[ TokenKind_enum ] = CStrView( "enum" );
	g_token.kind_names[ TokenKind_struct ] = CStrView( "struct" );
	g_token.kind_names[ TokenKind_union ] = CStrView( "union" );
	g_token.kind_names[ TokenKind_typedef ] = CStrView( "typedef" );

	// if-else branching
	g_token.kind_names[ TokenKind_if ] = CStrView( "if" );
	g_token.kind_names[ TokenKind_else ] = CStrView( "else" );
	g_token.kind_names[ TokenKind_Ternary ] = CStrView( "?" );

	// switch-case branching
	g_token.kind_names[ TokenKind_switch ] = CStrView( "switch" );
	g_token.kind_names[ TokenKind_case ] = CStrView( "case" );
	g_token.kind_names[ TokenKind_default ] = CStrView( "default" );

	// Iteration
	g_token.kind_names[ TokenKind_for ] = CStrView( "for" );
	g_token.kind_names[ TokenKind_do ] = CStrView( "do" );
	g_token.kind_names[ TokenKind_while ] = CStrView( "while" );

	// Execution flow control
	g_token.kind_names[ TokenKind_break ] = CStrView( "break" );
	g_token.kind_names[ TokenKind_continue ] = CStrView( "continue" );
	g_token.kind_names[ TokenKind_return ] = CStrView( "return" );
	g_token.kind_names[ TokenKind_goto ] = CStrView( "goto" );
	g_token.kind_names[ TokenKind_GotoLabel ] = CStrView( "GotoLabel" );

	// Storage specifiers
	g_token.kind_names[ TokenKind_auto ] = CStrView( "auto" );
	g_token.kind_names[ TokenKind_thread_local ] = CStrView( "thread_local" );
	g_token.kind_names[ TokenKind__Thread_local ] = CStrView( "_Thread_local" );
	g_token.kind_names[ TokenKind_register ] = CStrView( "register" );

	// Linkage specifiers
	g_token.kind_names[ TokenKind_extern ] = CStrView( "extern" );
	g_token.kind_names[ TokenKind_static ] = CStrView( "static" );

	g_token.kind_names[ TokenKind_alignas ] = CStrView( "alignas" );
	g_token.kind_names[ TokenKind_alignof ] = CStrView( "alignof" );
	g_token.kind_names[ TokenKind_constexpr ] = CStrView( "constexpr" );
	g_token.kind_names[ TokenKind_false ] = CStrView( "false" );
	g_token.kind_names[ TokenKind_inline ] = CStrView( "inline" );
	g_token.kind_names[ TokenKind_nullptr ] = CStrView( "nullptr" );
	g_token.kind_names[ TokenKind_sizeof ] = CStrView( "sizeof" );
	g_token.kind_names[ TokenKind_static_assert ] = CStrView( "static_assert" );
	g_token.kind_names[ TokenKind_true ] = CStrView( "true" );
	g_token.kind_names[ TokenKind_typeof ] = CStrView( "typeof" );
	g_token.kind_names[ TokenKind_typeof_unequal ] = CStrView( "typeof_unequal" );
	g_token.kind_names[ TokenKind__Alignas ] = CStrView( "_Alignas" );
	g_token.kind_names[ TokenKind__Alignof ] = CStrView( "_Alignof" );
	g_token.kind_names[ TokenKind__Generic ] = CStrView( "_Generic" );
	g_token.kind_names[ TokenKind__Noreturn ] = CStrView( "_Noreturn" );
	g_token.kind_names[ TokenKind__Static_assert ] = CStrView( "_Static_assert" );

	/* Preprocessor keywords */

	// TokenKind_if
	g_token.kind_names[ TokenKind_elif ] = CStrView( "elif" );
	// TokenKind_else,
	g_token.kind_names[ TokenKind_endif ] = CStrView( "endif" );
	g_token.kind_names[ TokenKind_ifdef ] = CStrView( "ifdef" );
	g_token.kind_names[ TokenKind_ifndef ] = CStrView( "ifndef" );
	g_token.kind_names[ TokenKind_elifdef ] = CStrView( "elifdef" );
	g_token.kind_names[ TokenKind_elifndef ] = CStrView( "elifndef" );
	g_token.kind_names[ TokenKind_define ] = CStrView( "define" );
	g_token.kind_names[ TokenKind_undef ] = CStrView( "undef" );
	g_token.kind_names[ TokenKind_include ] = CStrView( "include" );
	g_token.kind_names[ TokenKind_embed ] = CStrView( "embed" );
	g_token.kind_names[ TokenKind_line ] = CStrView( "line" );
	g_token.kind_names[ TokenKind_error ] = CStrView( "error" );
	g_token.kind_names[ TokenKind_warning ] = CStrView( "warning" );
	g_token.kind_names[ TokenKind_pragma ] = CStrView( "pragma" );
	g_token.kind_names[ TokenKind_defined ] = CStrView( "defined" );
	g_token.kind_names[ TokenKind__has_include ] = CStrView( "_has_include" );
	g_token.kind_names[ TokenKind__has_embed ] = CStrView( "_has_embed" );
	g_token.kind_names[ TokenKind__has_c_attribute ] = CStrView( "_has_c_attribute" );

	// Recognized outside of the preprocessor
	g_token.kind_names[ TokenKind__Pragma ] = CStrView( "_Pragma" );

	/* Conditionally-supported keywords */

	g_token.kind_names[ TokenKind_asm ] = CStrView( "asm" );
	g_token.kind_names[ TokenKind_fortran ] = CStrView( "fortran" );

	/* Group 2: Operators */

	// Assignment operators
	g_token.kind_names[ TokenKind_Assign ] = CStrView( "=" );
	g_token.kind_names[ TokenKind_PlusAssign ] = CStrView( "+=" );
	g_token.kind_names[ TokenKind_MinusAssign ] = CStrView( "-=" );
	g_token.kind_names[ TokenKind_TimesAssign ] = CStrView( "*=" );
	g_token.kind_names[ TokenKind_DivideAssign ] = CStrView( "/=" );
	g_token.kind_names[ TokenKind_ModuloAssign ] = CStrView( "&=" );
	g_token.kind_names[ TokenKind_BitwiseAndAssign ] = CStrView( "&=" );
	g_token.kind_names[ TokenKind_BitwiseOrAssign ] = CStrView( "!=" );
	g_token.kind_names[ TokenKind_BitwiseXorAssign ] = CStrView( "^=" );
	g_token.kind_names[ TokenKind_BitwiseNotAssign ] = CStrView( "~=" );
	g_token.kind_names[ TokenKind_BitwiseShiftLeftAssign ] = CStrView( "<<=" );
	g_token.kind_names[ TokenKind_BitwiseShiftRightAssign ] = CStrView( ">>=" );

	// Arithmetic operators
	g_token.kind_names[ TokenKind_Plus ] = CStrView( "+" );
	g_token.kind_names[ TokenKind_Minus ] = CStrView( "-" );
	g_token.kind_names[ TokenKind_Star ] = CStrView( "*" );
	g_token.kind_names[ TokenKind_Divide ] = CStrView( "/" );
	g_token.kind_names[ TokenKind_Modulo ] = CStrView( "%" );
	g_token.kind_names[ TokenKind_Increment ] = CStrView( "++" );
	g_token.kind_names[ TokenKind_Decrement ] = CStrView( "--" );

	// Comparison operators
	g_token.kind_names[ TokenKind_Equals ] = CStrView( "==" );
	g_token.kind_names[ TokenKind_NotEquals ] = CStrView( "!=" );
	g_token.kind_names[ TokenKind_Less ] = CStrView( "<" );
	g_token.kind_names[ TokenKind_LessEquals ] = CStrView( "<=" );
	g_token.kind_names[ TokenKind_Greater ] = CStrView( ">" );
	g_token.kind_names[ TokenKind_GreaterEquals ] = CStrView( ">=" );

	// Logical operators
	g_token.kind_names[ TokenKind_LogicalAnd ] = CStrView( "&&" );
	g_token.kind_names[ TokenKind_LogicalOr ] = CStrView( "||" );
	g_token.kind_names[ TokenKind_LogicalNot ] = CStrView( "!" );

	// Bitwise operators
	g_token.kind_names[ TokenKind_BitwiseAnd ] = CStrView( "&" );
	g_token.kind_names[ TokenKind_BitwiseOr ] = CStrView( "|" );
	g_token.kind_names[ TokenKind_BitwiseXor ] = CStrView( "^" );
	g_token.kind_names[ TokenKind_BitwiseNot ] = CStrView( "~" );
	g_token.kind_names[ TokenKind_BitwiseShiftLeft ] = CStrView( "<<" );
	g_token.kind_names[ TokenKind_BitwiseShiftRight ] = CStrView( ">>" );

	// Access operators
	g_token.kind_names[ TokenKind_Dot ] = CStrView( "." );
	g_token.kind_names[ TokenKind_Arrow ] = CStrView( "->" );
	g_token.kind_names[ TokenKind_Ampersand ] = CStrView( "&" );

	// Grouping & Separator punctuators
	g_token.kind_names[ TokenKind_LeftParenthesis ] = CStrView( "(" );
	g_token.kind_names[ TokenKind_RightParenthesis ] = CStrView( ")" );
	g_token.kind_names[ TokenKind_LeftBracket ] = CStrView( "[" );
	g_token.kind_names[ TokenKind_RightBracket ] = CStrView( "]" );
	g_token.kind_names[ TokenKind_LeftCurlyBrace ] = CStrView( "{" );
	g_token.kind_names[ TokenKind_RightCurlyBrace ] = CStrView( "}" );
	g_token.kind_names[ TokenKind_Colon ] = CStrView( ":" );
	g_token.kind_names[ TokenKind_Semicolon ] = CStrView( ";" );
	g_token.kind_names[ TokenKind_Comma ] = CStrView( "," );

	g_token.kind_names[ TokenKind_Pointer ] = CStrView( "*" );
	g_token.kind_names[ TokenKind_AddressOf ] = CStrView( "&" );
	g_token.kind_names[ TokenKind_PointerDereference ] = CStrView( "*" );
}

StringView_ASCII token_kind_name( TokenKind kind ) {
	return g_token.kind_names[ kind ];
}

String_ASCII token_kinds_to_string( Allocator *allocator, u32 initial_capacity, DECL_Array_View( TokenKind, token_kinds ) ) {
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

bool token_kind_is_of_declaration( TokenKind kind ) {
	return ( kind >= TokenKind__DeclarablesBegin && kind <= TokenKind__DeclarablesEnd );
}
