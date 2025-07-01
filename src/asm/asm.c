#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/asm/instruction.h"

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

  printf("Assembling %s...\n", input_filename);

  printf("Pass 1: lexical analysis...\n");
  instruction_list_t* list = instruction_list_read(input_file);

  instruction_list_print(list);

  fclose(input_file);
  instruction_list_free(list);

  // Write output file
  // const char* output_filename = asm_get_output_filename(input_filename);
  // write_object_file(program, output_filename);

  // program_destroy(program);

  printf("Assembly complete!\n");
  return 0;
}
