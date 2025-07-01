#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdio.h>

#define MAX_LINE_LENGTH 256
#define MAX_TOKEN_LENGTH 50
#define MAX_INSTRUCTION_LENGTH 10
#define MAX_INSTRUCTIONS 1000

typedef char token_t[MAX_TOKEN_LENGTH];

typedef struct {
  token_t tokens[MAX_INSTRUCTION_LENGTH];
  int count;
} instruction_t;

typedef struct {
  instruction_t instructions[MAX_INSTRUCTIONS];
  int count;
} instruction_list_t;

instruction_list_t* instruction_list_read(FILE* input_file);
void instruction_list_free(instruction_list_t* list);
void instruction_list_print(const instruction_list_t* list);

#endif  // INSTRUCTION_H
