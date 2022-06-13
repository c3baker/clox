#include "clox_compiler.h"
#include "clox_object.h"
#include "clox_scanner.h"
#include "clox_chunk.h"
#include "clox_value.h"

#ifdef DEBUG_PRINT_CODE
    #include "clox_debug.h"
#endif

#define GET_PARSER(compiler_ptr) ((compiler_ptr)->parser)
#define GET_SCANNER(compiler_ptr) ((compiler_ptr)->scanner)
#define GET_VM(compiler_ptr) ((compiler_ptr)->vm)
#define GET_TYPE(token_ptr) ((token_ptr)->type) 
#define LOCAL_SCOPE(compiler_ptr) (compiler_ptr->scope_depth > 0)
typedef struct
{
    TOKEN name;
    int depth;
}LOCAL;

typedef struct
{
    VM* vm;
    SCANNER* scanner;
    PARSER* parser;
    CHUNK* compiling_chunk;
    int local_count;
    int scope_depth;
    LOCAL locals[UINT8_COUNT];
}COMPILER;


typedef void (*Parse_fn)(COMPILER* compiler, bool can_assign);
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
static size_t emit_jump(COMPILER* compiler, uint8_t jump_instruct);
static void expression(COMPILER* compiler);
static void parse_precedence(COMPILER* compiler, PRECEDENCE precedence);
static void number(COMPILER* compiler, bool can_assign);
static void grouping(COMPILER* compiler, bool can_assign);
static void variable(COMPILER* compiler, bool can_assign);
static void unary(COMPILER* compiler, bool can_assign);
static void binary(COMPILER* compiler, bool can_assign);
static void string(COMPILER* compiler, bool can_assign);
static void literal(COMPILER* compiler, bool can_assign);
static bool check(COMPILER* compiler, TOKEN_TYPE type);
static bool match(COMPILER* compiler, TOKEN_TYPE type);
static void declaration(COMPILER* compiler);
static void statement(COMPILER* compiler);
static void expression_statement(COMPILER* compiler);
static void synchronize(COMPILER* compiler);
static void var_declaration(COMPILER* compiler);
static void define_variable(COMPILER* compiler, int value_index);
static int parse_variable(COMPILER* compiler, char* message);
static int identifier_constant(COMPILER* compiler, TOKEN* ident_token);
static void named_variable(COMPILER* compiler, TOKEN* identifier, bool can_assign);
static void print_statement(COMPILER* compiler);
static bool identifiers_equal(const TOKEN* idnt_1, const TOKEN* idnt_2);
static void declare_variable(COMPILER* compiler);
static void if_statement(COMPILER* compiler);
static void or(COMPILER* compiler);
static void and(COMPILER* compiler);

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
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_PRIMARY},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
    [TOKEN_NUMERIC] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, and, PREC_AND},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, or, PREC_OR},
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
    bool can_assign = precedence <= PREC_ASSIGN;
    advance(compiler);
    prefix_rule = get_rule(GET_TYPE(&parser->previous))->prefix;
    if(prefix_rule == NULL)
    {
        error(parser, "Expected expression");
        return;
    }

    prefix_rule(compiler, can_assign);

    while(precedence <= get_rule(GET_TYPE(&parser->current))->precedence)
    {
        Parse_fn infix_rule;
        advance(compiler);
        infix_rule = get_rule(GET_TYPE(&parser->previous))->infix;
        if(infix_rule == NULL)
        {
            error(parser, "Expected expression");
            return;
        }
        infix_rule(compiler, can_assign);
    }

    if(can_assign && match(compiler, TOKEN_EQUAL))
    {
        error(parser, "Invalid assignment target");
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
        synchronize(compiler);
    }
}

static void begin_scope(COMPILER* compiler)
{
    compiler->scope_depth++;
}

static void end_scope(COMPILER* compiler)
{
    int i = 0;
    int current_depth = compiler->scope_depth;
    for(i = compiler->local_count; i >= 0; i--)
    {
        LOCAL* local = &compiler->locals[i];

        if(local->depth < current_depth)
        {
            break;
        }
        emit_byte(compiler, OP_POP);
        compiler->local_count--;
    }
    compiler->scope_depth--;
}

static void block(COMPILER* compiler)
{
    begin_scope(compiler);
    while(!check(compiler, TOKEN_RIGHT_BRACE) && !check(compiler, TOKEN_EOF))
    {
        declaration(compiler);
    }

    consume(compiler, TOKEN_RIGHT_BRACE, "Expected '}'.");
    end_scope(compiler);
}

static void patch_jump(COMPILER* compiler, size_t jump)
{
    
    size_t diff = compiler->compiling_chunk->count - jump;
    if(UINT16_MAX < diff)
    {
        error(GET_PARSER(compiler), "Jump distance too far. Cannot handle this large an if-block.");
    }

    compiler->compiling_chunk->code[jump - 2] = diff & 0xFF; // Lower 8 bits
    compiler->compiling_chunk->code[jump - 1] = (diff >> 8) & 0xFF; // Upper 8 bits
}

static void if_statement(COMPILER* compiler)
{
    size_t jump = 0;
    size_t else_jump = 0;
    bool else_statement = false;

    consume(compiler, TOKEN_LEFT_PAREN, "Expected '(' after 'if'");
    expression(compiler);
    consume(compiler, TOKEN_RIGHT_PAREN, "Expected ')");

    jump = emit_jump(compiler, OP_JUMP_ON_FALSE);
    emit_byte(compiler, OP_POP); //get rid of if-expression on stack when if-expression is true
    consume(compiler, TOKEN_LEFT_BRACE, "Expected '{' following if-statement");
    block(compiler);

    if(match(compiler, TOKEN_ELSE))
    {
        else_jump = emit_jump(compiler, OP_JUMP);
        else_statement = true;
    }

    patch_jump(compiler, jump);
    emit_byte(compiler, OP_POP);  // Get rid of if-expression on stack in case if-expression was false

    if(else_statement)
    {
        consume(compiler, TOKEN_LEFT_BRACE, "Expected '{' following if-statement");
        block(compiler);   
        patch_jump(compiler, else_jump);    
    }
    
}

static void for_initializer(COMPILER* compiler)
{
    if(match(compiler, TOKEN_SEMICOLON)) // No initializer
    {
        return;
    }
    if(match(compiler, TOKEN_VAR))
    {
        var_declaration(compiler);
    }
    else
    {
        expression_statement(compiler);
        consume(compiler, TOKEN_SEMICOLON, "Expected ; after for initializer");
    }
}

static void for_statement(COMPILER* compiler)
{
    size_t for_loop_start = 0;
    size_t for_loop_end_jump = 0;
    size_t increment_jump = 0;
    size_t for_loop_increment = 0;

    consume(compiler, TOKEN_LEFT_PAREN, "Expected '(' after for");
    begin_scope(compiler);  //Being for-statement scope
    for_initializer(compiler);
    for_loop_start = compiler->compiling_chunk->count;    
    expression(compiler); //for-condition
    consume(compiler, TOKEN_SEMICOLON, "Expected ';");
    for_loop_end_jump = emit_jump(compiler, OP_JUMP_ON_FALSE);
    emit_byte(compiler, OP_POP); // Pop conditional expression result
    increment_jump = emit_jump(compiler, OP_JUMP); // Jump over increment code, to be executed at end of the for-body
    for_loop_increment = compiler->compiling_chunk->count;
    expression(compiler); //Increment expression
    emit_byte(compiler, OP_POP); // Pop increment expression result
    consume(compiler, TOKEN_RIGHT_PAREN, "Expected matching ')' ");
    emit_loop(compiler, for_loop_start);
    patch_jump(compiler, increment_jump);
    block(compiler); // for-loop body
    emit_loop(compiler, for_loop_increment);
    patch_jump(compiler, for_loop_end_jump);
    emit_byte(compiler, OP_POP); // Pop conditional expression result
    end_scope(compiler);
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
   else if(match(compiler, TOKEN_LEFT_BRACE))
   {

       block(compiler);

   }
   else if (match(compiler, TOKEN_IF))
   {
       if_statement(compiler);
   }
   else if(match(compiler, TOKEN_WHILE))
   {
       while_statement(compiler);
   }
   else if(match(compiler, TOKEN_FOR))
   {
       for_statement(compiler);
   }
   else
   {
       expression_statement(compiler);
   }
}

static int identifier_constant(COMPILER* compiler, TOKEN* ident_token)
{
    return add_constant(compiler->compiling_chunk, OBJ_VAL(new_string_object(GET_VM(compiler),ident_token->length,
                                    ident_token->start)));
}

static void add_local(COMPILER* compiler, TOKEN name)
{
       LOCAL* local = NULL;

       if(compiler->local_count == UINT8_COUNT)
       {
           error(GET_PARSER(compiler), "Too many local variables in scope.");
           return;
       }

       local = &compiler->locals[compiler->local_count];

       local->depth = -1; // We consider this variable to be declared but uninitialized
       local->name = name;
       compiler->local_count++;
}

static bool identifiers_equal(const TOKEN* idnt_1, const TOKEN* idnt_2)
{
    if(idnt_1->length != idnt_2->length)
    {
        return false;
    }
    
    return 0 == memcmp(idnt_1->start, idnt_2->start, idnt_1->length);
}

static void declare_variable(COMPILER* compiler)
{
    PARSER* parser = GET_PARSER(compiler);
    TOKEN* name = NULL;
    int i = 0;

    if(!LOCAL_SCOPE(compiler))
    {
        return;
    }

    for(i = compiler->local_count; i >= 0;  i--)
    {
        LOCAL* local = &compiler->locals[i];
        if((local->depth != -1) && (local->depth < compiler->scope_depth))
        {
            break;
        }

        if(identifiers_equal(name, &local->name))
        {
            error(GET_PARSER(compiler), "Previously declared variable with same name in scope.");
        }
    }
    name = &parser->previous;
    add_local(compiler, *name);
}

static int parse_variable(COMPILER* compiler, char* message)
{
    PARSER* parser = GET_PARSER(compiler);
    TOKEN ident_token = {0};

    consume(compiler, TOKEN_IDENTIFIER, message);

    declare_variable(compiler);

    if(LOCAL_SCOPE(compiler))
    {
        return 0; // Local variable no need to store identifier
    }

    ident_token = parser->previous;

    return identifier_constant(compiler, &ident_token);
}

static void define_variable(COMPILER* compiler, int value_index)
{
    // No need to do anything for local variables
    // the value of the local variable is already on the stack
    // and its existence recorded
    if(LOCAL_SCOPE(compiler))
    {
        compiler->locals[compiler->local_count - 1].depth = compiler->scope_depth; // Now define the local variable
        return;
    }

    if(value_index > MAX_SHORT_CONST_INDEX)
    {
        // Little Endian byte storage
        emit_byte(compiler, OP_DEFINE_GLOBAL_LONG);
    }
    else
    {
        emit_byte(compiler, OP_DEFINE_GLOBAL);
    }

    write_constant_index(compiler->compiling_chunk, value_index, compiler->parser->previous.line);
}

static void expression_statement(COMPILER* compiler)
{
    expression(compiler);
    emit_byte(compiler, OP_POP);
}

static void print_statement(COMPILER* compiler)
{
    expression(compiler);
    consume(compiler, TOKEN_SEMICOLON, "expected ; at end of statement");
    emit_byte(compiler, OP_PRINT);
}

static void var_declaration(COMPILER* compiler)
{
    int global = parse_variable(compiler, "Expected variable name");

    if(match(compiler, TOKEN_EQUAL))
    {
        // There's an assignment
        expression(compiler);
    }
    else
    {
        emit_byte(compiler, OP_NIL);
    }
    consume(compiler, TOKEN_SEMICOLON, "Expected ; at end of statement");
    define_variable(compiler, global);
}

static void expression(COMPILER* compiler)
{
    parse_precedence(compiler, PREC_ASSIGN); //Parse all expressions of precedence PERC_ASSIGN or higher
}

static void number(COMPILER* compiler, bool can_assign)
{
    double value = strtod(GET_PARSER(compiler)->previous.start, NULL);
    emit_constant(compiler, NUMERIC_VAL(value));
}

static void string(COMPILER* compiler, bool can_assign)
{
   char* str = GET_PARSER(compiler)->previous.start + 1;  //Skip the leading double-quote
   size_t str_len = GET_PARSER(compiler)->previous.length - 2; // Remove the start and end double-quotes of the string
   CLOX_STRING* str_obj = NEW_STRING(GET_VM(compiler), str_len, str);
   emit_constant(compiler, OBJ_VAL(str_obj));
}

static void emit_loop(COMPILER* compiler, size_t loop_start)
{
    uint16_t offset = (compiler->compiling_chunk->count - loop_start) + 2;

    if(offset > UINT16_MAX)
    {
        error(GET_PARSER(compiler), "Loop too large, cannot make jump to the specified address.");
    }
    emit_byte(compiler, OP_LOOP_BACK);
    emit_byte(compiler, offset & 0xFF);
    emit_byte(compiler, (offset>>8) & 0xFF);
}

static void while_statement(COMPILER* compiler)
{
    size_t while_jump = 0;
    size_t jump_back = 0;
    size_t loop_start = compiler->compiling_chunk->count;

    consume(compiler, TOKEN_LEFT_PAREN, "Expected '(' following while");
    expression(compiler);
    consume(compiler, TOKEN_RIGHT_PAREN, "Expected ')' following while condition");
    while_jump = emit_jump(compiler, OP_JUMP_ON_FALSE);
    emit_byte(compiler, OP_POP);  // Pop while-condition result
    consume(compiler, TOKEN_LEFT_BRACE, "Expected '{' following while-statement");
    block(compiler);
    emit_loop(compiler, loop_start);
    patch_jump(compiler, while_jump);
    emit_byte(compiler, OP_POP;
}

static void or(COMPILER* compiler)
{
    size_t jump_1 = 0;
    size_t jump_2 = 0;
    jump_1 = emit_jump(compiler, OP_JUMP_ON_FALSE); // No need to evaluate the OR futher if the first expression was true 
    jump_2 = emit_jump(compiler, OP_JUMP);
    patch_jump(compiler, jump_1);
    emit_byte(compiler, OP_POP);
    parse_precedence(compiler, PREC_OR);
    patch_jump(compile, jump_2);
}

static void and(COMPILER* compiler)
{
   size_t jump = emit_jump(compiler, OP_JUMP_ON_FALSE); // No need to evaluate the AND further if the first expression is false
   emit_byte(compiler, OP_POP);
   parse_precedence(compiler, PREC_AND);
   patch_jump(compiler, jump);
}

static void grouping(COMPILER* compiler, bool can_assign)
{
    expression(compiler);
    consume(compiler, TOKEN_RIGHT_PAREN, "Could not find matching ')'");
}

static void unary(COMPILER* compiler, bool can_assign)
{
    TOKEN* operator = &GET_PARSER(compiler)->previous;

    parse_precedence(compiler, PREC_UNARY); // First push the expression onto the VM stack

    if(GET_TYPE(operator) == TOKEN_MINUS)
    {
        emit_byte(compiler, OP_NEGATE); //Negation will pop the stack, negate and push on the stack again
    }
}

static void variable(COMPILER* compiler, bool can_assign)
{
    PARSER* parser = GET_PARSER(compiler);

    named_variable(compiler, &parser->previous, can_assign);
}

static int resolve_local(COMPILER* compiler, TOKEN* identifier)
{
   int i = 0;

   for(i = compiler->local_count; i >= 0; i--)
   {
       if(identifiers_equal(&compiler->locals[i].name, identifier))
       {
            if(compiler->locals[i].depth == -1)
            {
                error(GET_PARSER(compiler), "Cannot read local variable in its own initialization.");
            }
           return i;
       }
   }

   return -1;
}


static void named_variable(COMPILER* compiler, TOKEN* identifier, bool can_assign)
{
    uint8_t get_op = 0;
    uint8_t set_op = 0;
    int index = resolve_local(compiler, identifier);

    if(index != -1)
    {
        get_op = OP_GET_LOCAL;
        set_op = OP_SET_LOCAL;
    }
    else
    {
        index = identifier_constant(compiler, identifier);
        get_op = index > MAX_SHORT_CONST_INDEX ? OP_SET_GLOBAL_LONG : OP_SET_GLOBAL;
        set_op = index > MAX_SHORT_CONST_INDEX ? OP_GET_GLOBAL_LONG : OP_GET_GLOBAL;
    }
    
    if(can_assign && match(compiler, TOKEN_EQUAL))
    {
        expression(compiler);
        emit_byte(compiler, set_op);
    }
    else
    {
        emit_byte(compiler, get_op);
    }
    write_constant_index(compiler->compiling_chunk, index, compiler->parser->previous.line);
}

static void binary(COMPILER* compiler, bool can_assign)
{
    TOKEN operator = GET_PARSER(compiler)->previous;
    PARSE_RULE* rule = get_rule(GET_TYPE(&operator));

    parse_precedence(compiler, (PRECEDENCE)rule->precedence + 1);
    switch(GET_TYPE(&operator))
    {
        case TOKEN_PLUS:
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

static void literal(COMPILER* compiler, bool can_assign)
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
    compiler->local_count = 0;
    compiler->scope_depth = GLOBAL_SCOPE_DEPTH;
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

static size_t emit_jump(COMPILER* compiler, uint8_t jump_instruct)
{
    emit_byte(compiler, jump_instruct);
    emit_byte(compiler, 0xFF); // Dummy offset, must be patched later when we know how far to jump
    emit_byte(compiler, 0xFF);

    return compiler->compiling_chunk->count;    
}