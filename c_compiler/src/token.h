#pragma once
#ifndef C_COMPILER__TOKEN_H
#define C_COMPILER__TOKEN_H

#include "common.h"
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
// typedef u32 TokenKind;
typedef TokenKinds TokenKind;

typedef struct Token {
	TokenKind kind;
	StringView_ASCII view;
	u32 line;
	u32 column;
} Token;

void token_kind_names_init();

StringView_ASCII token_kind_name( TokenKind kind );
String_ASCII token_kinds_to_string( Allocator *allocator, u32 initial_capacity, DECL_Array_View( TokenKind, token_kinds ) );

bool token_kind_is_of_declaration( TokenKind kind );

#endif /* C_COMPILER__TOKEN_H */
