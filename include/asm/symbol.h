#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdint.h>

#define MAX_SYMBOLS 100

// Label structure for symbol table
typedef struct {
  char name[64];
  uint16_t address;
} symbol_t;

typedef struct {
  symbol_t symbols[MAX_SYMBOLS];
  int symbol_count;
} symbol_table_t;

symbol_table_t* symbol_table_create(void);

void symbol_table_destroy(symbol_table_t* symbol_table);

symbol_table_t* symbol_parse_file(const char* filename);

uint16_t symbol_table_find_address(symbol_table_t* symbol_table,
                              const char* label_name);

void symbol_table_write_file(symbol_table_t* symbol_table, const char* filename);


#endif  // SYMBOL_H
