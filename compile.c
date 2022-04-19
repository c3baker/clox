#include "clox_compiler.h"
#include "clox_scanner.h"

void compile(const char* source)
{
    SCANNER scanner = {0};
    int line = -1;

    init_scanner(&scanner, source);

    while(true)
    {
        TOKEN token = scan_token(&scanner);
        if(token.line != line)
        {
            printf("%4d", token.line);
            line = token.line;
        }
        else
        {
            printf("    |");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start); // %.*s means token.length is passed as the "precision" argument to tell how many chars to print

        if(token.type == TOKEN_EOF) break;
    }


}