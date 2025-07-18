#include "../../include/asm/program.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define swap16(val) ((val << 8) | (val >> 8))

void program_add_instruction(program_t* program, uint16_t instruction,
                             uint16_t address) {
  if (program->instruction_count < MAX_INSTRUCTIONS) {
    program->instructions[program->instruction_count].address = address;
    program->instructions[program->instruction_count].instruction = instruction;
    program->instruction_count++;
  }
}

// Parse a number from a string (supports #decimal, x/Xhex, and decimal)
int parse_number(const char* str) {
  if (!str) return 0;

  // Skip leading whitespace
  while (*str == ' ' || *str == '\t') str++;

  // Handle immediate values starting with #
  if (*str == '#') {
    str++;
    // Check for hex after #
    if (*str == 'x' || *str == 'X') {
      return (int)strtol(str + 1, NULL, 16);
    } else {
      return (int)strtol(str, NULL, 10);
    }
  }
  // Handle hex values starting with x or X
  else if (*str == 'x' || *str == 'X') {
    return (int)strtol(str + 1, NULL, 16);
  }
  // Handle decimal values
  else {
    return (int)strtol(str, NULL, 10);
  }
}

// Register name to number mapping
int get_register_number(const char* reg_str) {
  if (!reg_str || (reg_str[0] != 'R' && reg_str[0] != 'r')) return -1;
  char digit = reg_str[1];
  if (digit >= '0' && digit <= '7') {
    return digit - '0';
  }
  return -1;
}

// Parse ADD instruction
uint16_t parse_add(char* tokens[], int token_count) {
  if (token_count < 4) return 0;

  int dr = get_register_number(tokens[1]);
  int sr1 = get_register_number(tokens[2]);

  if (dr == -1 || sr1 == -1) return 0;

  uint16_t instruction = 0x1000 | (dr << 9) | (sr1 << 6);

  // Check if third operand is register or immediate
  if (tokens[3][0] == '#') {
    // Immediate mode
    int imm = parse_number(tokens[3]);
    if (imm < -16 || imm > 15) return 0;  // 5-bit signed immediate
    instruction |= 0x20 | (imm & 0x1F);
  } else {
    // Register mode
    int sr2 = get_register_number(tokens[3]);
    if (sr2 == -1) return 0;
    instruction |= sr2;
  }

  return instruction;
}

// Parse AND instruction
uint16_t parse_and(char* tokens[], int token_count) {
  if (token_count < 4) return 0;

  int dr = get_register_number(tokens[1]);
  int sr1 = get_register_number(tokens[2]);

  if (dr == -1 || sr1 == -1) return 0;

  uint16_t instruction = 0x5000 | (dr << 9) | (sr1 << 6);

  // Check if third operand is register or immediate
  if (tokens[3][0] == '#') {
    // Immediate mode
    int imm = parse_number(tokens[3]);
    if (imm < -16 || imm > 15) return 0;  // 5-bit signed immediate
    instruction |= 0x20 | (imm & 0x1F);
  } else {
    // Register mode
    int sr2 = get_register_number(tokens[3]);
    if (sr2 == -1) return 0;
    instruction |= sr2;
  }

  return instruction;
}

// Parse NOT instruction
uint16_t parse_not(char* tokens[], int token_count) {
  if (token_count < 3) return 0;

  int dr = get_register_number(tokens[1]);
  int sr = get_register_number(tokens[2]);

  if (dr == -1 || sr == -1) return 0;

  return 0x9000 | (dr << 9) | (sr << 6) | 0x3F;
}

// Parse BR instruction with symbol resolution
uint16_t parse_br(char* tokens[], int token_count, symbol_table_t* symbols,
                  uint16_t current_address) {
  if (token_count < 2) return 0;

  // Default to unconditional branch (BRnzp)
  uint16_t condition = 0x0E00;  // n=1, z=1, p=1

  // Check for specific branch conditions
  char* opcode = tokens[0];
  if (strcmp(opcode, "BRn") == 0 || strcmp(opcode, "brn") == 0) {
    condition = 0x0800;  // n=1
  } else if (strcmp(opcode, "BRz") == 0 || strcmp(opcode, "brz") == 0) {
    condition = 0x0400;  // z=1
  } else if (strcmp(opcode, "BRp") == 0 || strcmp(opcode, "brp") == 0) {
    condition = 0x0200;  // p=1
  } else if (strcmp(opcode, "BRzp") == 0 || strcmp(opcode, "brzp") == 0) {
    condition = 0x0600;  // z=1, p=1
  } else if (strcmp(opcode, "BRnp") == 0 || strcmp(opcode, "brnp") == 0) {
    condition = 0x0A00;  // n=1, p=1
  } else if (strcmp(opcode, "BRnz") == 0 || strcmp(opcode, "brnz") == 0) {
    condition = 0x0C00;  // n=1, z=1
  }

  int offset = 0;

  // Try to resolve as symbol first
  if (symbols && tokens[1][0] != '#' && tokens[1][0] != 'x' &&
      tokens[1][0] != 'X') {
    uint16_t symbol_address = symbol_table_find_address(symbols, tokens[1]);
    if (symbol_address != (uint16_t)-1) {
      // Calculate PC-relative offset (target - (current_address + 1))
      offset = (int)symbol_address - (int)(current_address + 1);
    } else {
      printf("Warning: Symbol '%s' not found\n", tokens[1]);
      return 0;
    }
  } else {
    // Handle immediate offsets
    offset = parse_number(tokens[1]);
  }

  if (offset < -256 || offset > 255) return 0;  // 9-bit signed offset

  return 0x0000 | condition | (offset & 0x1FF);
}

// Parse LEA instruction with symbol resolution
uint16_t parse_lea(char* tokens[], int token_count, symbol_table_t* symbols,
                   uint16_t current_address) {
  if (token_count < 3) return 0;

  int dr = get_register_number(tokens[1]);
  if (dr == -1) return 0;

  int offset = 0;

  // Try to resolve as symbol first
  if (symbols && tokens[2][0] != '#' && tokens[2][0] != 'x' &&
      tokens[2][0] != 'X') {
    uint16_t symbol_address = symbol_table_find_address(symbols, tokens[2]);
    if (symbol_address != (uint16_t)-1) {
      // Calculate PC-relative offset (target - (current_address + 1))
      offset = (int)symbol_address - (int)(current_address + 1);
    } else {
      printf("Warning: Symbol '%s' not found\n", tokens[2]);
      return 0;
    }
  } else {
    // Handle immediate offsets
    offset = parse_number(tokens[2]);
  }

  if (offset < -256 || offset > 255) return 0;  // 9-bit signed offset

  return 0xE000 | (dr << 9) | (offset & 0x1FF);
}

// Parse LD instruction with symbol resolution
uint16_t parse_ld(char* tokens[], int token_count, symbol_table_t* symbols,
                  uint16_t current_address) {
  if (token_count < 3) return 0;

  int dr = get_register_number(tokens[1]);
  if (dr == -1) return 0;

  int offset = 0;

  // Try to resolve as symbol first
  if (symbols && tokens[2][0] != '#' && tokens[2][0] != 'x' &&
      tokens[2][0] != 'X') {
    uint16_t symbol_address = symbol_table_find_address(symbols, tokens[2]);
    if (symbol_address != (uint16_t)-1) {
      // Calculate PC-relative offset (target - (current_address + 1))
      offset = (int)symbol_address - (int)(current_address + 1);
    } else {
      printf("Warning: Symbol '%s' not found\n", tokens[2]);
      return 0;
    }
  } else {
    // Handle immediate offsets
    offset = parse_number(tokens[2]);
  }

  if (offset < -256 || offset > 255) return 0;  // 9-bit signed offset

  return 0x2000 | (dr << 9) | (offset & 0x1FF);
}

// Parse ST instruction with symbol resolution
uint16_t parse_st(char* tokens[], int token_count, symbol_table_t* symbols,
                  uint16_t current_address) {
  if (token_count < 3) return 0;

  int sr = get_register_number(tokens[1]);
  if (sr == -1) return 0;

  int offset = 0;

  // Try to resolve as symbol first
  if (symbols && tokens[2][0] != '#' && tokens[2][0] != 'x' &&
      tokens[2][0] != 'X') {
    uint16_t symbol_address = symbol_table_find_address(symbols, tokens[2]);
    if (symbol_address != (uint16_t)-1) {
      // Calculate PC-relative offset (target - (current_address + 1))
      offset = (int)symbol_address - (int)(current_address + 1);
    } else {
      printf("Warning: Symbol '%s' not found\n", tokens[2]);
      return 0;
    }
  } else {
    // Handle immediate offsets
    offset = parse_number(tokens[2]);
  }

  if (offset < -256 || offset > 255) return 0;  // 9-bit signed offset

  return 0x3000 | (sr << 9) | (offset & 0x1FF);
}

// Parse TRAP instruction
uint16_t parse_trap(char* tokens[], int token_count) {
  if (token_count < 2) return 0;

  int trap_vector = parse_number(tokens[1]);
  if (trap_vector < 0 || trap_vector > 255) return 0;

  return 0xF000 | (trap_vector & 0xFF);
}

// Parse a single instruction line with symbol resolution
uint16_t parse_instruction(char* line, symbol_table_t* symbols,
                           uint16_t current_address) {
  char* tokens[10];
  int token_count = 0;

  // Make a copy of the line to work with (since strtok modifies the string)
  char line_copy[MAX_LINE_LENGTH];
  strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
  line_copy[MAX_LINE_LENGTH - 1] = '\0';

  // Tokenize the line
  char* token = strtok(line_copy, " \t,");
  while (token && token_count < 10) {
    tokens[token_count++] = token;
    token = strtok(NULL, " \t,");  // Pass NULL for subsequent calls
  }

  if (token_count == 0) return 0;

  // Convert opcode to uppercase for comparison
  for (char* p = tokens[0]; *p; p++) *p = toupper(*p);

  // Parse different instruction types
  if (strcmp(tokens[0], "ADD") == 0) {
    return parse_add(tokens, token_count);
  } else if (strcmp(tokens[0], "AND") == 0) {
    return parse_and(tokens, token_count);
  } else if (strcmp(tokens[0], "NOT") == 0) {
    return parse_not(tokens, token_count);
  } else if (strcmp(tokens[0], "LEA") == 0) {
    return parse_lea(tokens, token_count, symbols, current_address);
  } else if (strcmp(tokens[0], "LD") == 0) {
    return parse_ld(tokens, token_count, symbols, current_address);
  } else if (strcmp(tokens[0], "ST") == 0) {
    return parse_st(tokens, token_count, symbols, current_address);
  } else if (strcmp(tokens[0], "TRAP") == 0) {
    return parse_trap(tokens, token_count);
  } else if (strncmp(tokens[0], "BR", 2) == 0) {
    return parse_br(tokens, token_count, symbols, current_address);
  } else if (strcmp(tokens[0], "HALT") == 0) {
    return 0xF025;  // TRAP x25
  } else if (strcmp(tokens[0], "PUTS") == 0) {
    return 0xF022;  // TRAP x22
  } else if (strcmp(tokens[0], "GETC") == 0) {
    return 0xF020;  // TRAP x20
  } else if (strcmp(tokens[0], "OUT") == 0) {
    return 0xF021;  // TRAP x21
  }

  return 0;  // Unknown instruction
}

// Parse instruction lines (handles multi-line constructs)
int parse_instruction_lines(char* first_line, char* buffer,
                            size_t bufsize) {
  // For now, we'll handle most instructions as single-line
  // Multi-line handling would be needed for complex .STRINGZ cases
  strncpy(buffer, first_line, bufsize - 1);
  buffer[bufsize - 1] = '\0';
  return 1;  // One line consumed
}

// Parse and add instruction to program
int parse_and_add_instruction(char* line, program_t* program,
                              symbol_table_t* symbols,
                              uint16_t current_address) {
  if (!line || strlen(line) == 0) return 0;

  // Make a copy of the line to work with
  char line_copy[MAX_LINE_LENGTH];
  strncpy(line_copy, line, MAX_LINE_LENGTH - 1);
  line_copy[MAX_LINE_LENGTH - 1] = '\0';

  // Trim leading whitespace
  char* trimmed = line_copy;
  while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

  // Skip empty lines
  if (*trimmed == '\0') return 0;

  // Handle labels - check if line starts with a label
  char* space_pos = trimmed;
  while (*space_pos && *space_pos != ' ' && *space_pos != '\t') space_pos++;

  // If there's something after the first token, it might be a label +
  // instruction
  if (*space_pos != '\0') {
    // Check if first token is a label (not a known instruction)
    char saved_char = *space_pos;
    *space_pos = '\0';

    // Convert first token to uppercase for comparison
    char temp_token[64];
    strncpy(temp_token, trimmed, 63);
    temp_token[63] = '\0';
    for (char* p = temp_token; *p; p++) *p = toupper(*p);

    // Check if it's a known instruction
    bool is_instruction =
        (strcmp(temp_token, "ADD") == 0 || strcmp(temp_token, "AND") == 0 ||
         strcmp(temp_token, "NOT") == 0 || strcmp(temp_token, "LEA") == 0 ||
         strcmp(temp_token, "LD") == 0 || strcmp(temp_token, "ST") == 0 ||
         strcmp(temp_token, "TRAP") == 0 || strcmp(temp_token, "HALT") == 0 ||
         strcmp(temp_token, "PUTS") == 0 || strcmp(temp_token, "GETC") == 0 ||
         strcmp(temp_token, "OUT") == 0 || strcmp(temp_token, "JMP") == 0 ||
         strcmp(temp_token, "JSR") == 0 || strcmp(temp_token, "RET") == 0 ||
         strncmp(temp_token, "BR", 2) == 0 ||
         strncmp(temp_token, ".FILL", 5) == 0 ||
         strncmp(temp_token, ".STRINGZ", 8) == 0 ||
         strncmp(temp_token, ".BLKW", 5) == 0);

    *space_pos = saved_char;

    if (!is_instruction) {
      // This is a label, skip to the instruction part
      while (*space_pos == ' ' || *space_pos == '\t') space_pos++;
      trimmed = space_pos;
    }
  }

  // Handle pseudo-ops
  if (strncmp(trimmed, ".FILL", 5) == 0) {
    // .FILL directive - parse the value
    char* value_str = trimmed + 5;
    while (*value_str == ' ' || *value_str == '\t') value_str++;
    uint16_t value = (uint16_t)parse_number(value_str);
    program_add_instruction(program, value, current_address);
    return 1;
  }

  if (strncmp(trimmed, ".STRINGZ", 8) == 0) {
    // .STRINGZ directive - parse the string
    char* string_start = trimmed + 8;
    while (*string_start == ' ' || *string_start == '\t') string_start++;

    if (*string_start == '"') {
      string_start++;  // Skip opening quote
      char* string_end = strchr(string_start, '"');
      if (string_end) {
        *string_end = '\0';  // Terminate string

        // Add each character as a separate instruction
        int count = 0;
        for (char* p = string_start; *p; p++) {
          program_add_instruction(program, (uint16_t)*p,
                                  current_address + count);
          count++;
        }
        // Add null terminator
        program_add_instruction(program, 0, current_address + count);
        count++;

        return count;
      }
    }
    return 0;
  }

  if (strncmp(trimmed, ".BLKW", 5) == 0) {
    // .BLKW directive - reserve space
    char* count_str = trimmed + 5;
    while (*count_str == ' ' || *count_str == '\t') count_str++;
    int count = parse_number(count_str);

    // Add zeros for the reserved space
    for (int i = 0; i < count; i++) {
      program_add_instruction(program, 0, current_address + i);
    }

    return count;
  }

  // Parse regular instruction
  uint16_t instruction = parse_instruction(trimmed, symbols, current_address);
  if (instruction != 0) {
    program_add_instruction(program, instruction, current_address);
    return 1;
  }

  return 0;  // No instruction added
}

program_t* program_create(const char* input_filename, symbol_table_t* symbols) {
  FILE* file = fopen(input_filename, "r");
  if (!file) {
    fprintf(stderr, "Error: Could not open input file %s\n", input_filename);
    return NULL;
  }

  program_t* program = malloc(sizeof(program_t));
  program->instruction_count = 0;
  program->origin = 0x3000;  // Default origin

  char line[MAX_LINE_LENGTH];
  uint16_t current_address = 0;
  bool origin_set = false;

  while (fgets(line, sizeof(line), file)) {
    // Remove comments (everything after ';')
    char* comment = strchr(line, ';');
    if (comment) *comment = '\0';

    // Trim trailing whitespace
    int len = strlen(line);
    while (len > 0 && isspace(line[len - 1])) {
      line[--len] = '\0';
    }
    if (strlen(line) == 0) continue;

    // Handle .ORIG directive
    char* trimmed = line;
    while (*trimmed == ' ' || *trimmed == '\t') trimmed++;

    if (strncmp(trimmed, ".ORIG", 5) == 0) {  // .ORIG x3000 ; example
      char* addr_str = trimmed + 5;
      while (*addr_str == ' ' || *addr_str == '\t') addr_str++;
      program->origin = (uint16_t)parse_number(addr_str);
      current_address = program->origin;
      origin_set = true;
      continue;
    }

    if (strncmp(trimmed, ".END", 4) == 0) break;

    if (!origin_set) continue;  // Skip until .ORIG is found

    // Parse instructions and add to program
    // Buffer to accumulate multi-line instructions
    char instr_buffer[MAX_LINE_LENGTH * 4];

    // Parse the instruction line
    int lines_consumed = parse_instruction_lines(trimmed, instr_buffer,
                                                 sizeof(instr_buffer));
    if (lines_consumed > 0) {
      // Parse the instruction(s) and add to program
      int added = parse_and_add_instruction(instr_buffer, program, symbols,
                                            current_address);
      if (added > 0) {
        current_address +=
            added - 1;  // -1 because current_address++ at the end of loop
      }
    }

    current_address++;
  }

  return program;
}

void program_destroy(program_t* program) {
  if (program) {
    free(program);
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
  uint16_t origin_be = swap16(program->origin);
  fwrite(&origin_be, sizeof(uint16_t), 1, file);

  // Write instructions (big-endian)
  for (int i = 0; i < program->instruction_count; i++) {
    uint16_t instr_be = swap16(program->instructions[i].instruction);
    fwrite(&instr_be, sizeof(uint16_t), 1, file);
  }

  fclose(file);
  printf("Generated %s with %d instructions\n", filename,
         program->instruction_count);
}
