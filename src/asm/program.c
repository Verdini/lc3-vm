#include "../../include/asm/program.h"

#include <stdlib.h>
#include <string.h>

program_t* program_create() {
  program_t* prog = malloc(sizeof(program_t));
  if (prog) {
    prog->label_count = 0;
    prog->instruction_count = 0;
  }
  return prog;
}

void program_destroy(program_t* program) {
  if (program) {
    free(program);
  }
}

void program_set_origin(program_t* program, uint16_t origin) {
  program->origin = origin;
}

// Add label to symbol table
void program_add_address(program_t* program, const char* name,
                         const uint16_t address) {
  if (program->label_count < MAX_LABELS) {
    strcpy(program->labels[program->label_count].name, name);
    program->labels[program->label_count].address = address;
    program->label_count++;
  }
}

// Find label address
int program_find_address(program_t* program, const char* label_name) {
  for (int i = 0; i < program->label_count; i++) {
    if (strcmp(program->labels[i].name, label_name) == 0) {
      return program->labels[i].address;
    }
  }
  return -1;  // Label not found
}

void program_add_instruction(program_t* program, uint16_t instruction,
                             uint16_t address) {
  if (program->instruction_count < MAX_INSTRUCTIONS) {
    program->instructions[program->instruction_count].address = address;
    program->instructions[program->instruction_count].instruction = instruction;
    program->instruction_count++;
  }
}
