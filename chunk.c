#include "clox_chunk.h"

void init_chunk_lines(LINE* lines)
{
    lines->line_counts = NULL;
    lines->start_line = LINE_INIT;
    lines->max_lines = 0;
    lines->current_offset = 0;
}

void write_line(LINE* lines, int line)
{
    int offset = 0;

    if(lines->current_offset >= lines->max_lines)
    {
        size_t new_capacity = GROW_ARRAY_SIZE(lines->max_lines);
        size_t old_capacity = lines->max_lines;
        lines->line_counts =  GROW_ARRAY(int, lines->line_counts, lines->max_lines, new_capacity);
        lines->max_lines = new_capacity;
        lines->line_counts = memset(lines->line_counts + old_capacity, 0, new_capacity - old_capacity);
        if (lines->start_line == LINE_INIT)
        {
            lines->start_line = line;
        }
    }

    offset = line - lines->start_line;
    lines->line_counts[offset]++;
}

void free_chunk_lines(LINE* lines)
{
    if(NULL == FREE_ARRAY(int, lines->line_counts))
    {
        init_chunk(lines);
    }
    else
    {
        exit(1); // Error
    }
}

void init_chunk(CHUNK* chunk)
{
    chunk->count = 0;
    chunk->capactiy = 0;
    chunk->code = NULL;
    init_value_array(&chunk->constants);
    init_chunk_lines(&chunk->lines);
}

void free_chunk(CHUNK* chunk)
{
    free_value_array(&chunk->constants);
    free_line_array(&chunk->lines);
    if(NULL == FREE_ARRAY(uint8_t, chunk->code))
    {
        init_chunk(chunk);
    }
    else
    {
        exit(1); // Error
    }
}

void write_chunk(CHUNK* chunk, uint8_t byte, int line)
{
    if(chunk->count >= chunk->capactiy)
    {
        size_t new_capacity = GROW_ARRAY_SIZE(chunk->capactiy);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, chunk->capactiy, new_capacity);
        chunk->capactiy = new_capacity;
    }

    chunk->code[++chunk->count] = byte;
    write_line(&chunk->lines, line);
}


int add_constant(CHUNK* chunk, Value value)
{
    write_value(&chunk->constants, value);
    return chunk->count - 1;
}
