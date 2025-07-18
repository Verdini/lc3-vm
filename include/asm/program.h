#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdint.h>

#include "symbol.h"

#define MAX_LINE_LENGTH 256
#define MAX_INSTRUCTIONS 1000

// Instruction structure
typedef struct {
  uint16_t address;
  uint16_t instruction;
} instruction_t;

typedef struct {
  uint16_t origin;
  instruction_t instructions[MAX_INSTRUCTIONS];
  int instruction_count;
} program_t;

program_t* program_create(const char* input_filename, symbol_table_t* symbols);
void program_destroy(program_t* program);

void program_write_file(program_t* program, const char* filename);

#endif  // PROGRAM_H
