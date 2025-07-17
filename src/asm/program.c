#include "../../include/asm/program.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/asm/parser.h"

program_t* program_create() {
  program_t* prog = malloc(sizeof(program_t));
  if (prog) {
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

void program_add_instruction(program_t* program, uint16_t instruction,
                             uint16_t address) {
  if (program->instruction_count < MAX_INSTRUCTIONS) {
    program->instructions[program->instruction_count].address = address;
    program->instructions[program->instruction_count].instruction = instruction;
    program->instruction_count++;
  }
}

// Remove comments and trim whitespace
void clean_line(char* line) {
  // Remove comments (everything after ';')
  char* comment = strchr(line, ';');
  if (comment) *comment = '\0';

  // Trim trailing whitespace
  int len = strlen(line);
  while (len > 0 && isspace(line[len - 1])) {
    line[--len] = '\0';
  }
}

program_t* program_parse_file(const char* input_filename) {
  FILE* file = fopen(input_filename, "r");
  if (!file) {
    fprintf(stderr, "Error: Could not open input file %s\n", input_filename);
    return NULL;
  }

  program_t* program = program_create();
  char line[MAX_LINE_LENGTH];
  uint16_t current_address;

  rewind(file);

  while (fgets(line, sizeof(line), file)) {
    clean_line(line);
    if (strlen(line) == 0) continue;

    // Handle .ORIG directive
    if (strstr(line, ".ORIG")) {
      char* token = strtok(line, " \t");
      token = strtok(NULL, " \t");
      if (token) {
        current_address = parse_number(token);
        program_set_origin(program, current_address);
      }
      continue;
    }

    // Handle .END directive
    if (strstr(line, ".END")) {
      break;
    }

    // Handle symbols
    char* colon = strchr(line, ':');
    if (colon) {
      char* rest = colon + 1;
      clean_line(rest);
      if (strlen(rest) > 0) {
        // Instruction on same line as label
        strcpy(line, rest);
      } else {
        continue;  // Label only, no instruction
      }
    }

    // Parse instruction
    char line_copy[MAX_LINE_LENGTH];
    strcpy(line_copy, line);
    uint16_t instruction = parse_instruction(line_copy);

    if (instruction != 0) {
      program_add_instruction(program, instruction, current_address);
    }

    current_address++;
  }

  return program;
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
