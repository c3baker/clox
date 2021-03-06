#ifndef clox_scanner_h
#define clox_scanner_h

#include "common.h"

typedef enum 
{
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE, TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS, TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
    
    TOKEN_BANG, TOKEN_BANG_EQUAL, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL,

    TOKEN_AND, TOKEN_FOR, TOKEN_WHILE, TOKEN_IF, TOKEN_ELSE, TOKEN_FALSE, TOKEN_TRUE, TOKEN_NIL, TOKEN_OR, TOKEN_PRINT, TOKEN_SUPER, TOKEN_CLASS, TOKEN_THIS, TOKEN_VAR, TOKEN_BREAK, TOKEN_RETURN,
    TOKEN_FUN,

    TOKEN_IDENTIFIER, TOKEN_NUMERIC, TOKEN_STRING,

    TOKEN_ERROR, TOKEN_EOF
    /* data */
}TOKEN_TYPE;

typedef struct
{
    const char* start;
    const char* current;
    int line;
}SCANNER;

typedef struct 
{
    TOKEN_TYPE type;
    const char* start;
    int length;
    int line;
}TOKEN;


typedef struct
{
    TOKEN current;
    TOKEN previous;
    bool had_error;
    bool panic_mode;
}PARSER;

void init_scanner(SCANNER* scanner, const char* source);
TOKEN scan_token(SCANNER* scanner);

#endif 
