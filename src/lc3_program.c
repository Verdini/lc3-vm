#include "../include/lc3_program.h"
#include <stdlib.h>
#include <string.h>

program_t *lc3_program_create()
{
    program_t *prog = malloc(sizeof(program_t));
    if (prog)
    {
        prog->label_count = 0;
        prog->instruction_count = 0;
        prog->current_address = 0x3000; // Default starting address
    }
    return prog;
}

// Find label address
int lc3_program_find_label(program_t *program, const char *label_name)
{
    for (int i = 0; i < program->label_count; i++)
    {
        if (strcmp(program->labels[i].name, label_name) == 0)
        {
            return program->labels[i].address;
        }
    }
    return -1; // Label not found
}

// Add label to symbol table
void lc3_program_add_label(program_t *program, const char *name)
{
    if (program->label_count < MAX_LABELS)
    {
        strcpy(program->labels[program->label_count].name, name);
        program->labels[program->label_count].address = program->current_address;
        program->label_count++;
    }
}
