#include <stdio.h>

#include "test/asm_tests.h"
#include "test/vm_tests.h"
#include "test_framework.h"

int main(void) {
  printf("=== RUNNING LC-3 VM AND ASSEMBLER TESTS ===\n\n");

  run_vm_tests();
  run_asm_tests();

  REPORT_TESTS();
}
