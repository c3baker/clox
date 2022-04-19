#include "common.h"
#include "clox_chunk.h"
#include "clox_vm.h"

int main(int argc, const char* argv[])
{

    VM* vm = init_vm();

    if(vm == NULL)
    {
        exit(139);
    }

    if (argc == 1)
    {
        repl();
    }
    else if (argc == 2)
    {
        runFile(argv[1]);
    }
    else
    {
        fprintf(stderr, "Usage: Clox [path]]\n");
        exit(64);
    }

   free_vm(&vm);
   return 0;
}

