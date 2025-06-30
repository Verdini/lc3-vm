#include "../../include/asm/program.h"

#include <stdlib.h>
#include <string.h>

program_t* program_create() {
  program_t* prog = malloc(sizeof(program_t));
  if (prog) {
    prog->origin = 0x3000;
    prog->label_count = 0;
    prog->instruction_count = 0;
    prog->current_address = 0x3000;
  }
  return prog;
}

void program_destroy(program_t* program) {
  if (program) {
    free(program);
  }
}

// Find label address
int program_find_label(program_t* program, const char* label_name) {
  for (int i = 0; i < program->label_count; i++) {
    if (strcmp(program->labels[i].name, label_name) == 0) {
      return program->labels[i].address;
    }
  }
  return -1;  // Label not found
}

// Add label to symbol table
void program_add_label(program_t* program, const char* name) {
  if (program->label_count < MAX_LABELS) {
    strcpy(program->labels[program->label_count].name, name);
    program->labels[program->label_count].address = program->current_address;
    program->label_count++;
  }
}
