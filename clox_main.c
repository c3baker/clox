#include "common.h"
#include "clox_chunk.h"
#include "clox_vm.h"
#include <stdlib.h>
#include <stdio.h>

static void repl(VM* vm);
static void run_file(VM* vm, const char* file_path);
static char* read_file(const char* file_path);

int main(int argc, const char* argv[])
{

    VM* vm = init_VM();

    if(vm == NULL)
    {
        exit(139);
    }

    if (argc == 1)
    {
        repl(vm);
    }
    else if (argc == 2)
    {
        run_file(vm, argv[1]);
    }
    else
    {
        fprintf(stderr, "Usage: Clox [path]]\n");
        exit(ERROR_EXIT_CLOX_USAGE);
    }

   free_VM(&vm);
   return 0;
}


static void repl(VM* vm)
{
    char line[1024];
    while(true)
    {
        printf(">> ");
        if(!fgets(line, sizeof(line), stdin))
        {
            printf("\n");
            break;
        }

        interpret(vm, line);

    }
}


static void run_file(VM* vm, const char* file_path)
{
    char* source_code = read_file(file_path);
    INTERPRET_RESULT result = interpret(vm, source_code);

    free(source_code);

    if(result == INTERPRET_COMPILE_ERROR) exit(65);
    if(result == INTERPRET_RUNTIME_ERROR) exit(70);    
}

static char* read_file(const char* file_path)
{
    FILE* file = fopen(file_path, "rb");
    size_t file_size = 0;
    char* src_buffer = NULL;
    size_t bytes_read = 0;

    if(file == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", file_path);
        exit(ERROR_EXIT_SOURCE_FILE_ERROR);
    }

    fseek(file, 0L, SEEK_END); // Use to get size of file
    file_size = ftell(file);
    rewind(file);
    
    if((src_buffer = (char*)malloc(file_size + 1)) == NULL)
    {
        fprintf(stderr, "Insufficient memory to map file: %s!\n", file_path);
        exit(ERROR_EXIT_SOURCE_FILE_ERROR);
    } 

    bytes_read = fread(src_buffer, sizeof(char), file_size, file);

    if(bytes_read != file_size)
    {
        fprintf(stderr, "Failed to read entire source file. Read only %ld of %ld bytes.", bytes_read, file_size);
        exit(ERROR_EXIT_SOURCE_FILE_ERROR);
    }
    src_buffer[file_size] = '\0';
    
    return src_buffer;
}
