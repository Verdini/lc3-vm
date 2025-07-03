#ifndef VM_TESTS_H
#define VM_TESTS_H

#include <stdint.h>
#include <stdio.h>

#include "../../include/vm/vm.h"
#include "../test_framework.h"

// Test VM initialization
static char *test_vm_init(void) {
  lc3_vm_t vm;
  vm_init(&vm);

  // Check PC is set to the start location
  ASSERT_UINT16_EQUAL("PC should be initialized to LC3_PC_START", LC3_PC_START,
                      vm.reg[LC3_R_PC]);

  // Check condition flag is initialized to zero
  ASSERT_INT_EQUAL("Condition flag should be initialized to LC3_FL_ZRO",
                   LC3_FL_ZRO, vm.reg[LC3_R_COND]);

  // Check that the VM is set to running
  ASSERT_TRUE("VM should be initialized to running state", vm.running);

  // Verify all registers are initialized to 0 (except PC and COND which we
  // checked above)
  for (int i = 0; i < LC3_R_PC; i++) {
    ASSERT_UINT16_EQUAL("Register should be initialized to 0", 0, vm.reg[i]);
  }

  return NULL;
}

// Test memory read/write operations
static char *test_vm_memory_ops(void) {
  lc3_vm_t vm;
  vm_init(&vm);

  // Test writing and reading from memory
  uint16_t test_addr = 0x3100;
  uint16_t test_value = 0x1234;

  vm.memory[test_addr] = test_value;

  ASSERT_UINT16_EQUAL("Memory value should match what was written", test_value,
                      vm.memory[test_addr]);

  return NULL;
}

// Run all VM tests
void run_vm_tests(void) {
  printf("Running VM tests...\n\n");
  RUN_TEST(test_vm_init);
  RUN_TEST(test_vm_memory_ops);
  // Add more VM tests here
}

#endif /* VM_TESTS_H */
