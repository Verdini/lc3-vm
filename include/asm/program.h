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
  uint16_t current_address;
} program_t;

program_t* program_create();
void program_destroy(program_t* program);
int program_find_label(program_t* program, const char* label_name);
void program_add_label(program_t* program, const char* name);

#endif  // PROGRAM_H
