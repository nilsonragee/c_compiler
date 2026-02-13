#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// c_compiler [options...] <files...>
// Options:
// -out --output-file-name

#include "common.h"
#include "console.h"

#include "token.h"
#include "lexer.h"
#include "parser.h"

int main( int arguments_count, char *arguments[] ) {

	console_init( /* code_page */ CP_UTF8, /* title */ L"C Compiler" );
	allocators_init();
	token_kind_names_init();

	DECL_Dynamic_Array( StringView_ASCII, args ) = darray_new( g_sys_allocator, sizeof( StringView_ASCII ), arguments_count );

	// Convert all C-style string arguments to `StringView`s
	ForIt ( s32, char *, arguments, arguments_count ) {
		StringView_ASCII arg_view = CStrView( it );
		darray_add( &args, &arg_view );
	}}

	console_print( "Arguments:\n" );
	ForIt ( u32, StringView_ASCII, args.data, args.item_count ) {
		console_print( "[%d]: " FMT_StrView "\n", it_index, ARG_StrView( it ) );
	}}

	FunctionDefinition func_defn = ( FunctionDefinition ) { 0 };
	/* Per each file... */
	ForItFrom ( u32, StringView_ASCII, args.data, 1, args.item_count ) {
		Lexer lexer = lexer_open_file( it );
		Parser parser = parser_create( &lexer );
		parser_parse_function_definition( &parser, &func_defn );
	}}

	/* Print function definition */

	StringView_ASCII func_name = string_view( &func_defn.name, 0, 0 );
	printf( "Parsed function: "FMT_StrView"\n", ARG_StrView( func_name ) );

	printf( "Body:\n" );
	ForIt ( u32, Statement *, func_defn.body.data, func_defn.body.item_count ) {
		if ( it->kind == StatementKind_FunctionCall ) {
			Statement_FunctionCall *s = ( Statement_FunctionCall * )it;
			StringView_ASCII callee_name = string_view( &s->function_name, 0, 0 );
			printf( "#%u: (FunctionCall) "FMT_StrView"() with args:\n",
				it_index+1, ARG_StrView( callee_name ) );
			ForIt2 ( u32, Token, s->arguments.data, s->arguments.item_count ) {
				printf( "  #%u: "FMT_StrView"\n", it2_index+1, ARG_StrView( it2.view ) );
			}}
		} else if ( it->kind == StatementKind_VariableDeclaration ) {
			Statement_VariableDeclaration *s = ( Statement_VariableDeclaration * )it;
			StringView_ASCII typeid_name = type_id_name( s->type.id );
			StringView_ASCII var_name = string_view( &s->name, 0, 0 );
			printf( "#%u: (VariableDeclaration) "FMT_StrView" "FMT_StrView"",
				it_index+1, ARG_StrView( typeid_name ), ARG_StrView( var_name ) );
			if ( s->initialized ) {
				switch ( s->type.id ) {
					case TypeId_char:  printf( " = '%c';\n", s->value.value.as_char ); break;
					case TypeId_bool:  printf( " = %s;\n", ( s->value.value.as_bool ) ? "true" : "false" ); break;
					case TypeId_short:  // Fall-through
					case TypeId_int:
					case TypeId_long:
					case TypeId_longlong:  printf( " = %lld;\n", s->value.value.as_int ); break;
					case TypeId_float:  // Fall-through
					case TypeId_double:
					case TypeId_long_double:  printf( " = %f;\n", s->value.value.as_long_double ); break;
					case TypeId_enum:  printf( " = %llu;\n", s->value.value.as_uint ); break;
					default: printf( " = (unknown);\n" ); break;
				}
			} else {
				printf( ";\n" );
			}
		} else if ( it->kind == StatementKind_Return ) {
			Statement_Return *s = ( Statement_Return * )it;
			printf( "#%u: (Return) "FMT_StrView";\n",
				it_index+1, ARG_StrView( s->expr.view ) );
		}
	}}

	return 0;
}
