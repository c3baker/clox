#include "clox_compiler.h"
#include "clox_object.h"

#ifdef DEBUG_PRINT_CODE
    #include "clox_debug.h"
#endif

#define GET_PARSER(compiler_ptr) ((compiler_ptr)->parser)
#define GET_SCANNER(compiler_ptr) ((compiler_ptr)->scanner)
#define GET_VM(compiler_ptr) ((compiler_ptr)->vm)
#define GET_TYPE(token_ptr) ((token_ptr)->type) 

typedef void (*Parse_fn)(COMPILER* compiler);
typedef struct 
{
    Parse_fn prefix;
    Parse_fn infix;
    PRECEDENCE precedence;
}PARSE_RULE;

static PARSE_RULE* get_rule(TOKEN_TYPE type);
static void advance(COMPILER* compiler);
static void error_at(PARSER* parser, TOKEN* token, const char* message);
static void error(PARSER* parser, const char* message);
static void consume(COMPILER* compiler, TOKEN_TYPE type, const char* message);
static void emit_byte(COMPILER* compiler, uint8_t byte);
static void emit_return(COMPILER* compiler);
static void emit_bytes(COMPILER* compiler, uint8_t byte_1, uint8_t byte_2);
static void emit_constant(COMPILER* compiler, Value constant_value);
static void expression(COMPILER* compiler);
static void parse_precedence(COMPILER* compiler, PRECEDENCE precedence);
static void number(COMPILER* compiler);
static void grouping(COMPILER* compiler);
static void unary(COMPILER* compiler);
static void binary(COMPILER* compiler);
static void string(COMPILER* compiler);
static void literal(COMPILER* compiler);
static bool check(COMPILER* compiler, TOKEN_TYPE type);
static bool match(COMPILER* compiler, TOKEN_TYPE type);
static void declaration(COMPILER* compiler);
static void statement(COMPILER* compiler);
static void expression_statement(COMPILER* compiler);
static void synchronize(COMPILER* compiler);
static void var_declaration(COMPILER* compiler);


/* PRATT PARSER TABLE 
 */
PARSE_RULE rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_UNARY},
    [TOKEN_BANG_EQUAL] ={NULL, binary, PREC_COMPARISON},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_PRIMARY},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMERIC] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static void parse_precedence(COMPILER* compiler, PRECEDENCE precedence)
{
    PARSER* parser = GET_PARSER(compiler);
    Parse_fn prefix_rule;
    advance(compiler);
    prefix_rule = get_rule(GET_TYPE(&parser->previous))->prefix;
    if(prefix_rule == NULL)
    {
        error(parser, "Expected expression");
        return;
    }

    prefix_rule(compiler);

    while(precedence <= get_rule(GET_TYPE(&parser->current))->precedence)
    {
        Parse_fn infix_rule;
        advance(compiler);
        printf("Infix Token %d\n", GET_TYPE(&parser->previous));
        infix_rule = get_rule(GET_TYPE(&parser->previous))->infix;
        if(infix_rule == NULL)
        {
            error(parser, "Expected expression");
            return;
        }
        infix_rule(compiler);
    }
}

static PARSE_RULE* get_rule(TOKEN_TYPE type)
{
    return &rules[type];
}

static void synchronize(COMPILER* compiler)
{
    PARSER* parser = GET_PARSER(compiler);

    parser->panic_mode = false;

    while(parser->current.type != TOKEN_EOF)
    {
        if(parser->current.type == TOKEN_SEMICOLON) return; // We can synchronize at the end of the statement

        switch(parser->current.type)
        {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;
            default:
                ; // Do nothing
        }
        advance(compiler);
    }
}

static void declaration(COMPILER* compiler)
{
    statement(compiler);

    if(GET_PARSER(compiler)->panic_mode)
    {
        // Synchronize the compiler
        synchronize(GET_PARSER(compiler));
    }
}

static void statement(COMPILER* compiler)
{
   if(match(compiler, TOKEN_PRINT))
   {
       print_statement(compiler);

   }
   else if(match(compiler, TOKEN_VAR))
   {
       var_declaration(compiler);
   }
   else
   {
       expression_statement(compiler);
   }
}

uint8_t parse_variable(COMPILER* compiler, char* message)
{
    PARSER* parser = GET_PARSER(compiler);

    if(match(compiler, TOKEN_IDENTIFIER))
    {

    }
    else
    {
        error_at(parser, &parser->current, message);
    }

}


static void expression_statement(COMPILER* compiler)
{
    expression();
    consume(compiler, TOKEN_SEMICOLON, "expected ; at end of statement");
    emit_byte(compiler, OP_POP);
}

static void print_statement(COMPILER* compiler)
{
    expression();
    consume(compiler, TOKEN_SEMICOLON, "expected ; at end of statement");
    emit_byte(compiler, OP_PRINT);
}

static void var_declaration(COMPILER* compiler)
{
    uint8_t global = parse_variable(compiler, "Expected variable name");

    if(match(TOKEN_EQUAL))
    {
        // There's an assignment
    }
    else
    {
        emit_byte(OP_NIL);
    }
    consume(compiler, TOKEN_SEMICOLON, "Expected ; at end of statement");
    define_variable(global);
}

static void expression(COMPILER* compiler)
{
    parse_precedence(compiler, PREC_ASSIGN); //Parse all expressions of precedence PERC_ASSIGN or higher
}

static void number(COMPILER* compiler)
{
    double value = strtod(GET_PARSER(compiler)->previous.start, NULL);
    printf("Number Double %f\n", value);
    emit_constant(compiler, NUMERIC_VAL(value));
}

static void string(COMPILER* compiler)
{
   char* str = GET_PARSER(compiler)->previous.start + 1;  //Skip the leading double-quote
   size_t str_len = GET_PARSER(compiler)->previous.length - 2; // Remove the start and end double-quotes of the string
   CLOX_STRING* str_obj = NEW_STRING(GET_VM(compiler), str_len, str);
   printf("String\n");
   emit_constant(compiler, OBJ_VAL(str_obj));
}

static void grouping(COMPILER* compiler)
{
    expression(compiler);
    consume(compiler, TOKEN_RIGHT_PAREN, "Could not find matching ')'");
}

static void unary(COMPILER* compiler)
{
    TOKEN* operator = &GET_PARSER(compiler)->previous;

    parse_precedence(compiler, PREC_UNARY); // First push the expression onto the VM stack

    if(GET_TYPE(operator) == TOKEN_MINUS)
    {
        emit_byte(compiler, OP_NEGATE); //Negation will pop the stack, negate and push on the stack again
    }

}

static void binary(COMPILER* compiler)
{
    TOKEN operator = GET_PARSER(compiler)->previous;
    PARSE_RULE* rule = get_rule(GET_TYPE(&operator));
    printf("Operator %d\n", operator.type);
    parse_precedence(compiler, (PRECEDENCE)rule->precedence + 1);
         
    switch(GET_TYPE(&operator))
    {
        case TOKEN_PLUS:
            printf("OP ADD\n");
            emit_byte(compiler, OP_ADD);
            break;
        case TOKEN_MINUS:
            emit_byte(compiler, OP_SUB);
            break;
        case TOKEN_SLASH:
           emit_byte(compiler, OP_DIV);
           break;
        case TOKEN_STAR:
           emit_byte(compiler, OP_MULTIPLY);
           break;
        case TOKEN_EQUAL_EQUAL:
           emit_byte(compiler, OP_EQUAL);
           break;
        case TOKEN_LESS:
           emit_byte(compiler, OP_LESS);
           break;
        case TOKEN_GREATER:
           emit_byte(compiler, OP_GREATER);
           break;
        case TOKEN_BANG_EQUAL:
           emit_bytes(compiler, OP_EQUAL, OP_NOT);
           break;
        case TOKEN_GREATER_EQUAL:
           emit_bytes(compiler, OP_LESS, OP_NOT); // >= --> Not Less
           break;
        case TOKEN_LESS_EQUAL:
           emit_bytes(compiler, OP_GREATER, OP_NOT); // <= --> Not Greater
           break;
        default:
           break;
    }

}

static void literal(COMPILER* compiler)
{
    TOKEN prev = GET_PARSER(compiler)->previous;
    switch(GET_TYPE(&prev))
    {
        case TOKEN_NIL:
            emit_byte(compiler, OP_NIL);
            break;
        case TOKEN_TRUE:
            emit_byte(compiler, OP_TRUE);
            break;
        case TOKEN_FALSE:
            emit_byte(compiler, OP_FALSE);
            break;
        default:
            return;           
    };
}

static bool check(COMPILER* compiler, TOKEN_TYPE type)
{
   return GET_PARSER(compiler)->current.type == type;
}

static bool match(COMPILER* compiler, TOKEN_TYPE type)
{
   if(!check(compiler, type)) return false;
   advance(compiler);
   return true;
}

static void consume(COMPILER* compiler, TOKEN_TYPE type, const char* message)
{
    if(GET_PARSER(compiler)->current.type == type)
    {
        advance(compiler);
        return;
    }

    error_at(GET_PARSER(compiler), &GET_PARSER(compiler)->current, message);
}

static void error_at(PARSER* parser, TOKEN* token, const char* message)
{
    if(parser->panic_mode)  return; // We do not need to re-report errors if already in panic mode
    parser->panic_mode = true;
    fprintf(stderr, "[line %d] Error", token->line);
    if(token->type == TOKEN_EOF)
    {
        fprintf(stderr, " at end ");

    }
    else if(token->type == TOKEN_ERROR)
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", message);
    parser->had_error = true;
}

static void error(PARSER* parser, const char* message)
{
   error_at(parser, &parser->previous, message);
}

static void advance(COMPILER* compiler)
{
    PARSER* parser = GET_PARSER(compiler);
    SCANNER* scanner = GET_SCANNER(compiler);

    parser->previous = parser->current;

    while(true)
    {
        parser->current = scan_token(scanner);
        if(parser->current.type != TOKEN_ERROR) break;
        error_at(parser, &parser->current, parser->current.start);
    }
}


static void init_compiler(COMPILER* compiler, VM* vm, SCANNER* scanner, PARSER* parser, const char* source)
{
    init_scanner(scanner, source);    
    compiler->parser = parser;
    compiler->scanner = scanner;
    compiler->vm = vm;
}

bool compile(VM* vm, const char* source, CHUNK* chunk)
{
    SCANNER scanner = {0};
    PARSER parser = {0};
    COMPILER compiler = {0};

    init_compiler(&compiler, vm, &scanner, &parser, source);
    compiler.compiling_chunk = chunk;
    advance(&compiler);
    while(!match(&compiler, TOKEN_EOF))
    {
        declaration(&compiler);
    }
    emit_return(&compiler);
    #ifdef DEBUG_PRINT_CODE
       if (!parser.had_error)
       {
           disassemble_chunk(compiler.compiling_chunk, "code");
       }
    #endif
    return !parser.had_error;
}

static void emit_constant(COMPILER* compiler, Value constant_value)
{
    write_constant(compiler->compiling_chunk, constant_value, GET_PARSER(compiler)->previous.line);
}

static void emit_bytes(COMPILER* compiler, uint8_t byte_1, uint8_t byte_2)
{
    emit_byte(compiler, byte_1);
    emit_byte(compiler, byte_2);
}

static void emit_return(COMPILER* compiler)
{
    emit_byte(compiler, OP_RETURN);
}

static void emit_byte(COMPILER* compiler, uint8_t byte)
{
    write_chunk(compiler->compiling_chunk, byte, GET_PARSER(compiler)->previous.line);
}
