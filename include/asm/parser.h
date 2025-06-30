#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

int parse_number(const char* str);
uint16_t parse_instruction(char* line);

#endif
