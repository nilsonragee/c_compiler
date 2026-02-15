#pragma once
#ifndef C_COMPILER__PARSER_H
#define C_COMPILER__PARSER_H

#include "common.h"
#include "array.h"
#include "string_ascii.h"

#include "token.h"
#include "lexer.h"

typedef void ( * ParserReportFn )( struct Parser *parser, const char *format, ... );

typedef struct Parser {
	Lexer lexer;

	ParserReportFn report_info;
	ParserReportFn report_warning;
	ParserReportFn report_error;

	// Current state flags
	unsigned int
	in_global_scope : 1;
} Parser;

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
// typedef u16 TypeId;
typedef TypeIds TypeId;

typedef enum ETypeModifierBits {
	TypeModifierBit_Pointer = ( 1 << 0 ),
	TypeModifierBit_ConstPointer = ( 1 << 1 ),
	TypeModifierBit_Signed = ( 1 << 2 ),
	TypeModifierBit_Unsigned = ( 1 << 3 ),
	TypeModifierBit_Static = ( 1 << 4 ),
	TypeModifierBit_Const = ( 1 << 5 ),
	TypeModifierBit_Volatile = ( 1 << 6 ),
	TypeModifierBit_Restrict = ( 1 << 7 )
} ETypeModifierBits;
// typedef u32 TypeModifierBits;
typedef ETypeModifierBits TypeModifierBits;

typedef struct Type {
	String_ASCII symbol_name;
	TypeId id;
	union {
		TypeModifierBits modifiers;
		unsigned int
		is_pointer : 1,  // `const T *`
		is_const_pointer : 1,  // `T *const `
		is_signed : 1,  // `signed` is specified explicitly.
		is_unsigned : 1,

		is_static : 1,
		is_const : 1,
		is_volatile : 1,
		is_restrict : 1;
	};
	// Type *pointed_type;
} Type;

typedef enum StatementKinds {
	StatementKind_FunctionCall = 0,
	StatementKind_Return,
	StatementKind_VariableDeclaration,
} StatementKinds;
// typedef u32 StatementKind;
typedef StatementKinds StatementKind;

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
	bool initialized;
} Statement_VariableDeclaration;

typedef struct Expression {
	int _;
} Expression;

typedef struct FunctionDefinition {
	String_ASCII name;
	DECL_Dynamic_Array( Statement *, body );
	Type return_type;
} FunctionDefinition;

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
u32 type_size( Type *type );
bool token_kind_is_type_id( TokenKind kind );
bool token_kind_is_type_modifier( TokenKind kind );
TypeModifierBits token_kind_to_type_modifier_bit( TokenKind kind );
TokenKind type_modifier_bit_to_token_kind( TypeModifierBits modifiers );
bool type_id_is_of_compound( TypeId id );
bool type_id_is_of_integer( TypeId id );
bool type_id_is_of_float( TypeId id );
StringView_ASCII type_id_name( TypeId id );

void parser_report_info( Parser *parser, const char *format, ... );
void parser_report_warning( Parser *parser, const char *format, ... );
void parser_report_error( Parser *parser, const char *format, ... );

Parser parser_create( Lexer *lexer );

bool parser_parse_type( Parser *parser, Token *token, Type *type );
bool parser_parse_argument_list( Parser *parser, DECL_Dynamic_Array( Token, *arguments ) );
bool parser_parse_statement( Parser *parser, Token *token, DECL_Dynamic_Array( Statement *, *statements ) );
bool parser_parse_statement_block( Parser *parser, DECL_Dynamic_Array( Statement *, *statements ) );
bool parser_parse_function_definition( Parser *parser, FunctionDefinition *func_defn );

#endif /* C_COMPILER__PARSER_H */
