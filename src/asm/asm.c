#include <stdio.h>

#include "../../include/asm/program.h"
#include "../../include/asm/symbol.h"

int asm_symbol_run(const char* input_filename, const char* output_filename) {
  printf("Parsing symbols...\n");
  symbol_table_t* symbols = symbol_parse_file(input_filename);
  symbol_table_write_file(symbols, output_filename);
  symbol_table_destroy(symbols);
  return 0;
}

int asm_run(const char* input_filename, const char* output_filename) {
  printf("Parsing symbols...\n");
  symbol_table_t* symbols = symbol_parse_file(input_filename);

  printf("Parsing instructions...\n");
  program_t* program = program_create(input_filename, symbols);

  printf("Writing to file...\n");
  program_write_file(program, output_filename);

  symbol_table_destroy(symbols);
  program_destroy(program);
  return 0;
}
