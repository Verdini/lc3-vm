#include "../../include/asm/symbol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

symbol_table_t* symbol_table_create(void) {
  symbol_table_t* symbol_table = malloc(sizeof(symbol_table_t));
  symbol_table->symbol_count = 0;
  return symbol_table;
}

void symbol_table_destroy(symbol_table_t* symbol_table) {
  free(symbol_table);
}

void symbol_table_add_symbol(symbol_table_t* symbol_table, const char* name,
  const uint16_t address) {
  strncpy(symbol_table->symbols[symbol_table->symbol_count].name, name, 63);
  symbol_table->symbols[symbol_table->symbol_count].name[63] = '\0';
  symbol_table->symbols[symbol_table->symbol_count].address = address;
  symbol_table->symbol_count++;
}

uint16_t symbol_table_find_address(symbol_table_t* symbol_table,
                              const char* label_name) {
  for (int i = 0; i < symbol_table->symbol_count; i++) {
    if (strcmp(symbol_table->symbols[i].name, label_name) == 0) {
      return symbol_table->symbols[i].address;
    }
  }
  return -1;
}

symbol_table_t* symbol_parse_file(const char* filename) {
  symbol_table_t* symbol_table = symbol_table_create();
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open file %s to read symbols\n", filename);
    return NULL;
  }

  char line[256];
  uint16_t current_address = 0;
  bool origin_set = false;

  while (fgets(line, sizeof(line), file)) {
    char* newline = strchr(line, '\n');
    if (newline) *newline = '\0';

    char* trimmed = line;
    while (*trimmed == ' ' || *trimmed == '\t') trimmed++;
    if (*trimmed == '\0' || *trimmed == ';') continue;

    // Check for .ORIG directive
    if (strncmp(trimmed, ".ORIG", 5) == 0) {
      char* addr_str = trimmed + 5;
      while (*addr_str == ' ' || *addr_str == '\t') addr_str++;
      if (*addr_str == 'x' || *addr_str == 'X') {
        current_address = (uint16_t)strtol(addr_str + 1, NULL, 16);
      } else {
        current_address = (uint16_t)strtol(addr_str, NULL, 10);
      }
      origin_set = true;
      continue;
    }

    if (strncmp(trimmed, ".END", 4) == 0) break;

    if (!origin_set) continue;

    // Look for labels (identifier followed by space/tab or at start of line)
    char* label_end = trimmed;
    while (*label_end && *label_end != ' ' && *label_end != '\t' && *label_end != ';') {
      label_end++;
    }

    // Check if this looks like a label (not an instruction)
    bool is_label = false;
    char saved_char = *label_end;
    *label_end = '\0';

    // Check if it's not a common instruction
    if (strcmp(trimmed, "ADD") != 0 && strcmp(trimmed, "AND") != 0 &&
        strcmp(trimmed, "NOT") != 0 && strcmp(trimmed, "BR") != 0 &&
        strcmp(trimmed, "BRn") != 0 && strcmp(trimmed, "BRz") != 0 &&
        strcmp(trimmed, "BRp") != 0 && strcmp(trimmed, "BRnz") != 0 &&
        strcmp(trimmed, "BRnp") != 0 && strcmp(trimmed, "BRzp") != 0 &&
        strcmp(trimmed, "BRnzp") != 0 && strcmp(trimmed, "JMP") != 0 &&
        strcmp(trimmed, "JSR") != 0 && strcmp(trimmed, "JSRR") != 0 &&
        strcmp(trimmed, "RET") != 0 && strcmp(trimmed, "RTI") != 0 &&
        strcmp(trimmed, "LD") != 0 && strcmp(trimmed, "LDI") != 0 &&
        strcmp(trimmed, "LDR") != 0 && strcmp(trimmed, "LEA") != 0 &&
        strcmp(trimmed, "ST") != 0 && strcmp(trimmed, "STI") != 0 &&
        strcmp(trimmed, "STR") != 0 && strcmp(trimmed, "TRAP") != 0 &&
        strcmp(trimmed, "GETC") != 0 && strcmp(trimmed, "OUT") != 0 &&
        strcmp(trimmed, "PUTS") != 0 && strcmp(trimmed, "IN") != 0 &&
        strcmp(trimmed, "PUTSP") != 0 && strcmp(trimmed, "HALT") != 0 &&
        strncmp(trimmed, ".ORIG", 5) != 0 && strncmp(trimmed, ".END", 4) != 0) {
      is_label = true;
    }

    *label_end = saved_char;

    if (is_label && symbol_table->symbol_count < MAX_SYMBOLS) {
      // Extract label name
      char label_name[64];
      int label_len = label_end - trimmed;
      if (label_len >= 64) label_len = 63;
      strncpy(label_name, trimmed, label_len);
      label_name[label_len] = '\0';

      // Add to symbol table
      strncpy(symbol_table->symbols[symbol_table->symbol_count].name, label_name, 63);
      symbol_table->symbols[symbol_table->symbol_count].name[63] = '\0';
      symbol_table->symbols[symbol_table->symbol_count].address = current_address;
      symbol_table->symbol_count++;
    }

    // Calculate instruction/data size and increment address
    char* rest_of_line = label_end;
    while (*rest_of_line == ' ' || *rest_of_line == '\t') rest_of_line++;

    if (strncmp(rest_of_line, ".FILL", 5) == 0) {
      current_address += 1;
    } else if (strncmp(rest_of_line, ".BLKW", 5) == 0) {
      char* num_str = rest_of_line + 5;
      while (*num_str == ' ' || *num_str == '\t') num_str++;
      int num_words = 1;
      if (*num_str == '#') {
        num_words = atoi(num_str + 1);
      } else {
        num_words = atoi(num_str);
      }
      current_address += num_words;
    } else if (strncmp(rest_of_line, ".STRINGZ", 8) == 0) {
      char* str_start = strchr(rest_of_line, '"');
      if (str_start) {
        char* str_end = strchr(str_start + 1, '"');
        if (str_end) {
          int str_len = str_end - str_start - 1;
          current_address += str_len + 1; // +1 for null terminator
        } else {
          current_address += 1; // Default if malformed
        }
      } else {
        current_address += 1; // Default if malformed
      }
    } else if (*rest_of_line != '\0' && *rest_of_line != ';') {
      // Regular instruction
      current_address += 1;
    }
  }

  fclose(file);
  return symbol_table;
}

void symbol_table_write_file(symbol_table_t* symbol_table, const char* filename) {
  FILE* file = fopen(filename, "w");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open file %s to write symbols\n", filename);
    return;
  }
  for (int i = 0; i < symbol_table->symbol_count; i++) {
    fprintf(file, "%s\t%d\n", symbol_table->symbols[i].name, symbol_table->symbols[i].address);
  }
  fclose(file);
}
