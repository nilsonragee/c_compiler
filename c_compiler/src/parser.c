#include <stdlib.h>  // atoll, atof
#include <stdio.h>  // stdout, stderr
#include <stdarg.h>

#include "parser.h"

// Supress const <-> non-const type conversions, in this case:
//    const Lexer *const lexer = &parser->lexer;
//    lexer_next_token( lexer, &token )  // const Lexer *const lexer -> Lexer *
// #pragma warning( push )
// #pragma warning( disable : 4090 )  // warning C4090: 'function': different 'const' qualifiers

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
			AssertMessage( type->id >= TypeId_COUNT, "Cannot evaluate type size: unknown TypeId" );
			return 0;
	}
}

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

StringView_ASCII type_id_name( TypeId id ) {
	switch ( id ) {
		case TypeId_void:  return CStrView( "void" );
		case TypeId_char:  return CStrView( "char" );
		case TypeId_bool:  return CStrView( "bool" );
		case TypeId_short:  return CStrView( "short" );
		case TypeId_int:  return CStrView( "int" );
		case TypeId_long:  return CStrView( "long" );
		case TypeId_longlong:  return CStrView( "long long" );
		case TypeId_float:  return CStrView( "float" );
		case TypeId_double:  return CStrView( "double" );
		case TypeId_long_double:  return CStrView( "long double" );
		case TypeId_enum:  return CStrView( "enum" );
		case TypeId_union:  return CStrView( "union" );
		case TypeId_struct:  return CStrView( "struct" );
		default:
			AssertMessage( id >= TypeId_COUNT, "Invalid TypeId" );
			return CStrView( "(unknown)" );
	}
}

void parser_report_info( Parser *parser, const char *format, ... ) {
	Lexer *const lexer = &parser->lexer;
	fprintf( stdout, "Parser: "FMT_StrView":%u:%u: Info: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stdout, format, args );
	va_end( args );
}

void parser_report_warning( Parser *parser, const char *format, ... ) {
	Lexer *const lexer = &parser->lexer;
	fprintf( stdout, "Parser: "FMT_StrView":%u:%u: Warning: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stdout, format, args );
	va_end( args );
}

void parser_report_error( Parser *parser, const char *format, ... ) {
	Lexer *const lexer = &parser->lexer;
	fprintf( stderr, "Parser: "FMT_StrView":%u:%u: Error: ", ARG_StrView( lexer->file_path ), lexer->line+1, lexer->column+1 );
	va_list args;
	va_start( args, format );
	vfprintf( stderr, format, args );
	va_end( args );
}

Parser parser_create( Lexer *lexer ) {
	Parser parser = { 0 };
	parser.lexer = *lexer;
	parser.report_info = parser_report_info;
	parser.report_warning = parser_report_warning;
	parser.report_error = parser_report_error;
	return parser;
}

bool parser_parse_type( Parser *parser, Token *token, Type *type ) {
	Lexer *const lexer = &parser->lexer;

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

bool parser_parse_argument_list( Parser *parser, DECL_Dynamic_Array( Token, *arguments ) ) {
	Lexer *const lexer = &parser->lexer;
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

bool parser_parse_statement( Parser *parser, Token *token, DECL_Dynamic_Array( Statement *, *statements ) ) {
	Lexer *const lexer = &parser->lexer;
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
		// Zero out memory to ensure there is no uninitialized garbage memory
		//   for lesser type sizes.
		var_decl->value = ( ConstantValue ) { 0 };
		// WARNING: Only primitive type declarations for now.
		if ( token->kind == TokenKind_Semicolon ) {
			// Declaration without assignment.
			var_decl->initialized = false;
			darray_add( statements, &var_decl );
		} else if ( token->kind == TokenKind_Assign ) {
			// Declaration with assigment.
			var_decl->initialized = true;
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
	Lexer *const lexer = &parser->lexer;
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
	Lexer *const lexer = &parser->lexer;

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
	if ( !parser_parse_statement_block( parser, &func_defn->body ) )  return false;

	return true;
}

// #pragma warning( pop )  // Revert C4090 supression.
