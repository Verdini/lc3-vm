#include "../../include/asm/program.h"

#include <stdio.h>
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
void program_add_symbol(program_t* program, const char* name,
                         const uint16_t address) {
  if (program->label_count < MAX_SYMBOLS) {
    strcpy(program->symbols[program->label_count].name, name);
    program->symbols[program->label_count].address = address;
    program->label_count++;
  }
}

// Find label address
int program_find_symbol(program_t* program, const char* label_name) {
  for (int i = 0; i < program->label_count; i++) {
    if (strcmp(program->symbols[i].name, label_name) == 0) {
      return program->symbols[i].address;
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

// Write object file
void program_write_file(program_t* program, const char* filename) {
  FILE* file = fopen(filename, "wb");
  if (!file) {
    fprintf(stderr, "Error: Could not create output file %s\n", filename);
    return;
  }

  // Write origin (big-endian)
  uint16_t origin_be = (program->origin << 8) | (program->origin >> 8);
  fwrite(&origin_be, sizeof(uint16_t), 1, file);

  // Write instructions (big-endian)
  for (int i = 0; i < program->instruction_count; i++) {
    uint16_t instr_be = (program->instructions[i].instruction << 8) |
                        (program->instructions[i].instruction >> 8);
    fwrite(&instr_be, sizeof(uint16_t), 1, file);
  }

  fclose(file);
  printf("Generated %s with %d instructions\n", filename,
         program->instruction_count);
}
