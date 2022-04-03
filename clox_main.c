#include "common.h"
#include "clox_chunk.h"

int main(int argc, const char* argv[])
{
    CHUNK chunk;
    init_chunk(&chunk);
    write_chunk(&chunk, OP_RETURN);
    free_chunk(&chunk);

   return 0;
}

