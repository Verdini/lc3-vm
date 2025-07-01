#define _POSIX_C_SOURCE 200809L
#include "../../include/asm/instruction.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

instruction_t* instruction_read_line(const char* line) {
  instruction_t* instruction = malloc(sizeof(instruction_t));

  char line_copy[MAX_LINE_LENGTH];
  strncpy(line_copy, line, MAX_LINE_LENGTH - 1);

  char* delim = " \t\n";
  char* token = strtok(line_copy, delim);

  while (token) {
    if (token[0] == ';') break;

    if (instruction->count >= MAX_INSTRUCTION_LENGTH) {
      fprintf(stderr, "Error: Maximum instruction length exceeded.\n");
      free(instruction);
      return NULL;
    }

    // Copy token to instruction
    strncpy(instruction->tokens[instruction->count], token,
            MAX_TOKEN_LENGTH - 1);
    instruction->tokens[instruction->count][MAX_TOKEN_LENGTH - 1] =
        '\0';  // Ensure null termination
    instruction->count++;

    token = strtok(NULL, delim);
  }

  return instruction;
}

void instruction_free(instruction_t* instruction) {
  if (!instruction) return;
  free(instruction);
}

instruction_list_t* instruction_list_read(FILE* input_file) {
  instruction_list_t* list = malloc(sizeof(instruction_list_t));

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), input_file)) {
    // Remove trailing newline character
    line[strcspn(line, "\n")] = 0;

    // Skip empty lines and comments
    if (line[0] == '\0' || line[0] == ';') continue;

    // Get instruction from line
    instruction_t* instruction = instruction_read_line(line);
    if (!instruction) {
      fprintf(stderr, "Error: Invalid instruction in line: %s\n", line);
      continue;
    }

    // Add instruction to the list
    if (list->count < MAX_INSTRUCTIONS) {
      list->instructions[list->count++] = *instruction;
    } else {
      fprintf(stderr, "Error: Maximum instruction limit reached.\n");
      instruction_free(instruction);
      free(instruction);
      break;
    }
  }

  return list;
}

void instruction_list_free(instruction_list_t* list) {
  if (!list) return;
  free(list);
}

void instruction_list_print(const instruction_list_t* list) {
  if (!list) {
    printf("Instruction list is empty.\n");
    return;
  }

  printf("Instruction List:\n");
  for (int i = 0; i < list->count; i++) {
    instruction_t instr = list->instructions[i];
    printf("Instruction %d: ", i + 1);
    for (int j = 0; j < instr.count; j++) {
      printf("%s ", instr.tokens[j]);
    }
    printf("\n");
  }
}
