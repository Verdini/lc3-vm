#ifndef ASM_TESTS_H
#define ASM_TESTS_H

#include <stdint.h>
#include <stdio.h>

#include "../../include/asm/asm.h"
#include "../../include/asm/parser.h"
#include "../../include/asm/program.h"
#include "../test_framework.h"

// Test parsing of symbols
static char *test_asm_label_parsing(void) {
  // This test would depend on your specific implementation
  // Placeholder for now
  ASSERT_TRUE("Placeholder test", 1);
  return NULL;
}

// Test assembly of instructions
static char *test_asm_instruction_assembly(void) {
  // This test would depend on your specific implementation
  // Placeholder for now
  ASSERT_TRUE("Placeholder test", 1);
  return NULL;
}

// Run all assembler tests
void run_asm_tests(void) {
  printf("Running Assembler tests...\n\n");
  RUN_TEST(test_asm_label_parsing);
  RUN_TEST(test_asm_instruction_assembly);
  // Add more assembler tests here
}

#endif /* ASM_TESTS_H */
