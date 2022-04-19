#include "clox_scanner.h"

static bool end_of_source(SCANNER* scanner);
static TOKEN make_token(SCANNER* scanner, TOKEN_TYPE token_type);
static TOKEN error_token(SCANNER* scanner, const char* error_message);
static char advance(SCANNER* scanner);
static bool match(SCANNER* scanner, char match_char);
static void skip_whitespace(SCANNER* scanner);
static char peek(SCANNER* scanner);

void init_scanner(SCANNER* scanner, const char* source)
{
    scanner->start = source;
    scanner->current = source;
    scanner->line = 1;
}

TOKEN scan_token(SCANNER* scanner)
{
    skip_whitespace(scanner);

    scanner->start = scanner->current; // We assume the last scan ended on a complete token

    if(end_of_source(scanner)) return make_token(scanner, TOKEN_EOF);

    char c = advance(scanner);

    switch(c)
    {
        case '(' : return make_token(scanner, TOKEN_LEFT_PAREN);
        case ')' : return make_token(scanner, TOKEN_RIGHT_PAREN);
        case '{' : return make_token(scanner, TOKEN_LEFT_BRACE);
        case '}' : return make_token(scanner, TOKEN_RIGHT_BRACE);
        case ';' : return make_token(scanner, TOKEN_SEMICOLON);
        case ',' : return make_token(scanner, TOKEN_COMMA);
        case '.' : return make_token(scanner, TOKEN_DOT);
        case '+' : return make_token(scanner, TOKEN_PLUS);
        case '-' : return make_token(scanner, TOKEN_MINUS);
        case '*' : return make_token(scanner, TOKEN_STAR);
        case '/' : return make_token(scanner, TOKEN_SLASH);
        case '!' : return make_token(scanner, match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '>' : return make_token(scanner, match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '<' : return make_token(scanner, match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '=' : return make_token(scanner, match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        
    }

    return error_token(scanner, "CLOX unexpected symbol");

}

static TOKEN error_token(SCANNER* scanner, const char* error_message)
{
    TOKEN token;
    token.type = TOKEN_ERROR;
    token.start = error_message;
    token.length = (int)strlne(error_message);
    token.line = scanner->line;

    return token;
}

static TOKEN make_token(SCANNER* scanner, TOKEN_TYPE token_type)
{
    TOKEN token;
    token.type = token_type;
    token.start = scanner->start;
    token.length = (int)(scanner->current - scanner->start);
    token.line = scanner->line;

    return token;
}

static bool end_of_source(SCANNER* scanner)
{
    return *(scanner->current) == '\0';
}

static char advance(SCANNER* scanner)
{
    scanner->current++;
    return scanner->current[-1]; // Return the previous character
}

static bool match(SCANNER* scanner, char match_char)
{
    if(end_of_source(scanner))
    {
        return false;
    }

    if(*scanner->current == match_char)
    {
        advance(scanner);
        return true;
    }

    return false;
}

static void skip_whitespace(SCANNER* scanner)
{
    while(true)
    {
        char c = peek(scanner);
        switch(c)
        {
            case ' ':
            case '\r':
            case '\t':
                advance(scanner);
                break;
            default:
                return;
        }
    }
}

static char peek(SCANNER* scanner)
{
    return *scanner->current;
}

