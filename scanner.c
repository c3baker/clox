#include "clox_scanner.h"
#include <string.h>

static bool end_of_source(SCANNER* scanner);
static TOKEN make_token(SCANNER* scanner, TOKEN_TYPE token_type);
static TOKEN error_token(SCANNER* scanner, const char* error_message);
static char advance(SCANNER* scanner);
static bool match(SCANNER* scanner, char match_char);
static void skip_whitespace(SCANNER* scanner);
static char peek(SCANNER* scanner);
static bool is_alpha(char c);
static bool is_numeric(char c);
static TOKEN numeric_token(SCANNER* scanner);
static bool is_alpha_numeric(char c);
static TOKEN alpha_token(SCANNER* scanner);
static TOKEN_TYPE determine_identifier_type(SCANNER* scanner);
static TOKEN string_token(SCANNER* scanner);
static bool check_keyword(SCANNER* scanner, const char* remaining_keyword, int start, int remaining_len);

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
    if(is_alpha(c))
    {
        // Could be identifier or keyword
        return alpha_token(scanner);
    } 
    if(is_numeric(c))
    {
        return numeric_token(scanner);
    }

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
        case '"' : return string_token(scanner);
        case NULL_TERMINATOR: return make_token(scanner, TOKEN_EOF);
    }
    printf("Error token %d\n", (int)c);
    return error_token(scanner, "CLOX unexpected symbol");

}

static TOKEN error_token(SCANNER* scanner, const char* error_message)
{
    TOKEN token;
    token.type = TOKEN_ERROR;
    token.start = error_message;
    token.length = (int)strlen(error_message);
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

static TOKEN string_token(SCANNER* scanner)
{
    char c;
    while((c = advance(scanner)) != '"')
    {
        if(c == '\0')
        {
            return error_token(scanner, "Reached EOF without finding matching '\"'");
        }
    }
    return make_token(scanner, TOKEN_STRING);
}

static TOKEN numeric_token(SCANNER* scanner)
{
    while(is_numeric(peek(scanner))) {advance(scanner);}

    if(peek(scanner) == '.') //Decimal place
    {
        while(is_numeric(peek(scanner))) {advance(scanner);}
    }

    return make_token(scanner, TOKEN_NUMERIC);
}


// Assumed that c has been checked so that it is an alphabetic character
static TOKEN_TYPE determine_identifier_type(SCANNER* scanner)
{
    // Check to see if it's a keyword
    switch(*scanner->start)
    {
        case 'a':
            if(check_keyword(scanner, "nd", 1, 2)) return TOKEN_AND;    
            break;      
        case 'b':
            if(check_keyword(scanner, "break", 1, 4)) return TOKEN_BREAK;
            break;
        case 'c':
            if(check_keyword(scanner, "lass", 1, 4)) return TOKEN_CLASS;
            break;
        case 'e':
            if(check_keyword(scanner, "lse", 1, 3)) return TOKEN_ELSE;
            break;
        case 'f':
            if(scanner->current - scanner->start > 1)
            {
                switch(scanner->start[1])
                {
                    case 'a':
                        if(check_keyword(scanner, "lse", 2, 3)) return TOKEN_FALSE; 
                        break;
                    case 'o':
                        if(check_keyword(scanner, "r", 2, 1)) return TOKEN_FOR;  
                        break;  
                    case 'u':
                        if(check_keyword(scanner, "n", 2, 1)) return TOKEN_FUN;  
                        break; 
                    default:
                        break;  
                }
            }
            break;
        case 'i':
            if(check_keyword(scanner, "f", 1, 1)) return TOKEN_IF;
            break;
        case 'n':
            if(check_keyword(scanner, "il", 1, 2)) return TOKEN_NIL;
            break;
        case 'o':
            if(check_keyword(scanner, "r", 1, 1)) return TOKEN_OR;
            break;
        case 'p':
            if(check_keyword(scanner, "rint", 1, 4)) return TOKEN_PRINT;
            break;
        case 'r':
            if(check_keyword(scanner, "eturn", 1, 5)) return TOKEN_RETURN;
            break;
        case 't':
            if(scanner->current - scanner->start > 1)
            {
                switch(scanner->start[1])
                {
                    case 'r':
                        if(check_keyword(scanner, "ue", 2, 2)) return TOKEN_TRUE; 
                        break;
                    case 'h':
                        if(check_keyword(scanner, "is", 2, 2)) return TOKEN_THIS;  
                        break;  
                    default:
                        break;  
                }
            }
            break;
        case 'w':
            if(check_keyword(scanner, "hile", 1, 4)) return TOKEN_WHILE;
            break;
        case 'v':
            if(check_keyword(scanner, "ar", 1, 2)) return TOKEN_VAR;
            break;
        default:
            break;
    }
    
    return TOKEN_IDENTIFIER; // Not a keyword, normal identifier
}
static TOKEN alpha_token(SCANNER* scanner)
{
    while (is_alpha_numeric(peek(scanner)))
    {
         advance(scanner);
    }

    return make_token(scanner, determine_identifier_type(scanner));
}
static bool check_keyword(SCANNER* scanner, const char* remaining_keyword, int start, int remaining_len)
{
    int cur_indx = 0;
    char c = peek(scanner);

    if((start + remaining_len) == (scanner->current - scanner->start)) // Keyword and token are the same length
    {
        if(0 == memcmp(scanner->start + start, remaining_keyword, remaining_len))
        {
            return true;
        }
    }
    return false;
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
            case '\n':
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

static bool is_alpha(char c)
{
    return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || (c == '_');
}

static bool is_numeric(char c)
{
    return ((c >= '0') && (c <= '9'));
}

static bool is_alpha_numeric(char c)
{
    return is_alpha(c) || is_numeric(c);
}
