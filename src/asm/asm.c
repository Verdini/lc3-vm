#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/asm/parser.h"
#include "../../include/asm/program.h"

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

// First pass: collect symbols and calculate addresses
void parse_symbols(program_t* program, FILE* file) {
  char line[MAX_LINE_LENGTH];
  uint16_t current_address = 0;

  while (fgets(line, sizeof(line), file)) {
    clean_line(line);
    if (strlen(line) == 0) continue;

    // Check for .ORIG directive
    if (strstr(line, ".ORIG")) {
      char* token = strtok(line, " \t");
      token = strtok(NULL, " \t");
      if (token) current_address = parse_number(token);
      continue;
    }

    // Check for .END directive
    if (strstr(line, ".END")) {
      break;
    }

    // Check for label (ends with ':')
    char* colon = strchr(line, ':');
    if (colon) {
      *colon = '\0';  // Remove colon
      program_add_symbol(program, line, current_address);

      // Check if there's an instruction on the same line
      char* rest = colon + 1;
      clean_line(rest);
      if (strlen(rest) > 0) current_address++;
    } else {
      // Regular instruction
      current_address++;
    }
  }
}

// Second pass: generate machine code
void parse_instructions(program_t* program, FILE* file) {
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
}

char* asm_get_output_filename(const char* input_filename) {
  // Create output filename by replacing .asm with .obj
  static char output_filename[256];
  strcpy(output_filename, input_filename);
  char* dot = strrchr(output_filename, '.');
  if (dot) {
    strcpy(dot, ".obj");
  } else {
    strcat(output_filename, ".obj");
  }
  return output_filename;
}

int asm_run(const char* input_filename) {
  FILE* input_file = fopen(input_filename, "r");
  if (!input_file) {
    fprintf(stderr, "Error: Could not open input file %s\n", input_filename);
    return 1;
  }

  program_t* program = program_create();

  printf("Parsing symbols...\n");
  parse_symbols(program, input_file);

  printf("Parsing instructions...\n");
  parse_instructions(program, input_file);

  fclose(input_file);

  printf("Writing to file...\n");
  const char* output_filename = asm_get_output_filename(input_filename);
  program_write_file(program, output_filename);

  program_destroy(program);
  return 0;
}
