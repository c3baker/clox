#ifndef clox_scanner_h
#define clox_scanner_h

#include "common.h"

typedef enum 
{
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN, TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE, TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS, TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
    
    TOKEN_BANG, TOKEN_BANG_EQUAL, TOKEN_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL,

    TOKEN_AND, TOKEN_FOR, TOKEN_WHILE, TOKEN_IF, TOKEN_ELSE, TOKEN_FALSE, TOKEN_TRUE, TOKEN_NIL, TOKEN_OR, TOKEN_PRINT, TOKEN_SUPER, TOKEN_CLASS, TOKEN_THIS, TOKEN_VAR,

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

void init_scanner(const char* source);
TOKEN scan_token(SCANNER* scanner);

#endif 