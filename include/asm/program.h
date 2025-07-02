#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdint.h>

#define MAX_LINE_LENGTH 256
#define MAX_LABELS 100
#define MAX_INSTRUCTIONS 1000

// Label structure for symbol table
typedef struct {
  char name[64];
  uint16_t address;
} label_t;

// Instruction structure
typedef struct {
  uint16_t address;
  uint16_t instruction;
} instruction_t;

typedef struct {
  uint16_t origin;
  label_t labels[MAX_LABELS];
  int label_count;
  instruction_t instructions[MAX_INSTRUCTIONS];
  int instruction_count;
} program_t;

program_t* program_create();
void program_destroy(program_t* program);

void program_set_origin(program_t* program, uint16_t origin);

void program_add_address(program_t* program, const char* name,
                         const uint16_t address);
int program_find_address(program_t* program, const char* label_name);

void program_add_instruction(program_t* program, uint16_t instruction,
                             uint16_t address);

#endif  // PROGRAM_H
