#pragma once
#ifndef C_COMPILER__LEXER_H
#define C_COMPILER__LEXER_H

#include "common.h"
#include "array.h"
#include "string_ascii.h"

#include "token.h"

typedef void ( * LexerReportFn )( struct Lexer *lexer, const char *format, ... );

// Default Lexer report functions.
void lexer_report_info( struct Lexer *lexer, const char *format, ... );
void lexer_report_warning( struct Lexer *lexer, const char *format, ... );
void lexer_report_error( struct Lexer *lexer, const char *format, ... );

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

typedef enum LexerResult {
	Lexer_OK = 0,
	Lexer_EOF
} LexerResult;

Lexer lexer_open_file( StringView_ASCII file_path );

bool lexer_is_at_end( Lexer *lexer );
u32 lexer_eat_whitespace( Lexer *lexer );
u32 lexer_eat_to_line_end( Lexer *lexer );

LexerResult lexer_peak_next_token( Lexer *lexer, Token *token );
LexerResult lexer_next_token( Lexer *lexer, Token *token );

bool lexer_expect_many_this_token_kinds( Lexer *lexer, Token *token, DECL_Array_View( TokenKind, expected_kinds ) );
bool lexer_expect_many_next_token_kinds( Lexer *lexer, Token *token, DECL_Array_View( TokenKind, expected_kinds ) );

bool lexer_expect_this_token_kind( Lexer *lexer, Token *token, TokenKind expected_kind );
bool lexer_expect_next_token_kind( Lexer *lexer, Token *token, TokenKind expected_kind );

#endif /* C_COMPILER__LEXER_H */
