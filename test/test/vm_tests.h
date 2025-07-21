#ifndef VM_TESTS_H
#define VM_TESTS_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../include/vm/vm.h"
#include "../../include/vm/vm_exec.h"
#include "../../include/lc3/lc3.h"
#include "../test_framework.h"

// Helper function to create a VM for testing
vm_t* create_test_vm(void) {
  vm_t* vm = malloc(sizeof(vm_t));
  memset(vm->memory, 0, sizeof(vm->memory));
  memset(vm->reg, 0, sizeof(vm->reg));
  vm->reg[LC3_R_PC] = 0x3000;  // Set PC to default start
  vm->reg[LC3_R_COND] = LC3_FL_ZRO;  // Set initial condition flag
  vm->running = true;
  return vm;
}

// Helper function to destroy test VM
void destroy_test_vm(vm_t* vm) {
  free(vm);
}

// Test ADD instruction with register mode
char* test_add_register_mode(void) {
  vm_t* vm = create_test_vm();

  // Set up initial values: R1 = 5, R2 = 3
  vm->reg[1] = 5;
  vm->reg[2] = 3;

  // CREATE ADD R0, R1, R2 instruction (0x1042)
  // Opcode: 0001 (ADD), DR: 000 (R0), SR1: 001 (R1), Mode: 0, SR2: 010 (R2)
  uint16_t instr = 0x1042;

  vm_exec_add(vm, instr);

  // Verify: R0 should contain 5 + 3 = 8
  ASSERT_UINT16_EQUAL("ADD register mode result", 8, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test ADD instruction with immediate mode
char* test_add_immediate_mode(void) {
  vm_t* vm = create_test_vm();

  // Set up initial value: R1 = 10
  vm->reg[1] = 10;

  // CREATE ADD R0, R1, #5 instruction (0x1065)
  // Opcode: 0001, DR: 000, SR1: 001, Mode: 1, IMM5: 00101 (5)
  uint16_t instr = 0x1065;

  vm_exec_add(vm, instr);

  // Verify: R0 should contain 10 + 5 = 15
  ASSERT_UINT16_EQUAL("ADD immediate mode result", 15, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test ADD instruction with negative immediate
char* test_add_negative_immediate(void) {
  vm_t* vm = create_test_vm();

  // Set up initial value: R1 = 10
  vm->reg[1] = 10;

  // CREATE ADD R0, R1, #-3 instruction (0x107D)
  // IMM5 = -3 in 2's complement = 11101 (0x1D)
  uint16_t instr = 0x107D;

  vm_exec_add(vm, instr);

  // Verify: R0 should contain 10 + (-3) = 7
  ASSERT_UINT16_EQUAL("ADD negative immediate result", 7, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test AND instruction with register mode
char* test_and_register_mode(void) {
  vm_t* vm = create_test_vm();

  // Set up initial values: R1 = 0xF0, R2 = 0x0F
  vm->reg[1] = 0xF0;
  vm->reg[2] = 0x0F;

  // CREATE AND R0, R1, R2 instruction (0x5042)
  uint16_t instr = 0x5042;

  vm_exec_and(vm, instr);

  // Verify: R0 should contain 0xF0 & 0x0F = 0x00
  ASSERT_UINT16_EQUAL("AND register mode result", 0x00, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test AND instruction with immediate mode
char* test_and_immediate_mode(void) {
  vm_t* vm = create_test_vm();

  // Set up initial value: R1 = 0xFF
  vm->reg[1] = 0xFF;

  // CREATE AND R0, R1, #7 instruction (0x5067)
  // IMM5 = 7 = 00111
  uint16_t instr = 0x5067;

  vm_exec_and(vm, instr);

  // Verify: R0 should contain 0xFF & 0x07 = 0x07
  ASSERT_UINT16_EQUAL("AND immediate mode result", 0x07, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test NOT instruction
char* test_not_instruction(void) {
  vm_t* vm = create_test_vm();

  // Set up initial value: R1 = 0x00FF
  vm->reg[1] = 0x00FF;

  // CREATE NOT R0, R1 instruction (0x907F)
  // Opcode: 1001, DR: 000, SR: 001, 111111
  uint16_t instr = 0x907F;

  vm_exec_not(vm, instr);

  // Verify: R0 should contain ~0x00FF = 0xFF00
  ASSERT_UINT16_EQUAL("NOT instruction result", 0xFF00, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test BR instruction - branch taken
char* test_br_branch_taken(void) {
  vm_t* vm = create_test_vm();

  // Set condition flag to zero
  vm->reg[LC3_R_COND] = LC3_FL_ZRO;
  vm->reg[LC3_R_PC] = 0x3000;

  // CREATE BRz +5 instruction (0x0405)
  // Opcode: 0000, nzp: 010 (z flag), PCoffset9: 000000101 (5)
  uint16_t instr = 0x0405;

  vm_exec_br(vm, instr);

  // Verify: PC should be 0x3000 + 5 = 0x3005
  ASSERT_UINT16_EQUAL("BR instruction branch taken", 0x3005, vm->reg[LC3_R_PC]);

  destroy_test_vm(vm);
}

// Test BR instruction - branch not taken
char* test_br_branch_not_taken(void) {
  vm_t* vm = create_test_vm();

  // Set condition flag to positive (not zero)
  vm->reg[LC3_R_COND] = LC3_FL_POS;
  vm->reg[LC3_R_PC] = 0x3000;

  // CREATE BRz +5 instruction (0x0405)
  uint16_t instr = 0x0405;

  vm_exec_br(vm, instr);

  // Verify: PC should remain 0x3000 (no branch)
  ASSERT_UINT16_EQUAL("BR instruction branch not taken", 0x3000, vm->reg[LC3_R_PC]);

  destroy_test_vm(vm);
}

// Test JMP instruction
char* test_jmp_instruction(void) {
  vm_t* vm = create_test_vm();

  // Set up R1 with target address
  vm->reg[1] = 0x4000;

  // CREATE JMP R1 instruction (0xC040)
  // Opcode: 1100, BaseR: 001
  uint16_t instr = 0xC040;

  vm_exec_jmp(vm, instr);

  // Verify: PC should be set to 0x4000
  ASSERT_UINT16_EQUAL("JMP instruction result", 0x4000, vm->reg[LC3_R_PC]);

  destroy_test_vm(vm);
}

// Test JSR instruction
char* test_jsr_instruction(void) {
  vm_t* vm = create_test_vm();

  vm->reg[LC3_R_PC] = 0x3000;

  // CREATE JSR +10 instruction (0x480A)
  // Opcode: 0100, L: 1, PCoffset11: 00000001010 (10)
  uint16_t instr = 0x480A;

  vm_exec_jsr(vm, instr);

  // Verify: R7 should contain return address (0x3000)
  ASSERT_UINT16_EQUAL("JSR saves return address", 0x3000, vm->reg[LC3_R_R7]);
  // Verify: PC should be 0x3000 + 10 = 0x300A
  ASSERT_UINT16_EQUAL("JSR sets PC", 0x300A, vm->reg[LC3_R_PC]);

  destroy_test_vm(vm);
}

// Test LD instruction
char* test_ld_instruction(void) {
  vm_t* vm = create_test_vm();

  vm->reg[LC3_R_PC] = 0x3000;
  // Put test value at memory location 0x3005
  vm->memory[0x3005] = 0x1234;

  // CREATE LD R0, +5 instruction (0x2005)
  // Opcode: 0010, DR: 000, PCoffset9: 000000101 (5)
  uint16_t instr = 0x2005;

  vm_exec_ld(vm, instr);

  // Verify: R0 should contain value from memory[0x3005] = 0x1234
  ASSERT_UINT16_EQUAL("LD instruction result", 0x1234, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test LDI instruction
char* test_ldi_instruction(void) {
  vm_t* vm = create_test_vm();

  vm->reg[LC3_R_PC] = 0x3000;
  // Put pointer at memory location 0x3005
  vm->memory[0x3005] = 0x4000;
  // Put test value at memory location 0x4000
  vm->memory[0x4000] = 0x5678;

  // CREATE LDI R0, +5 instruction (0xA005)
  uint16_t instr = 0xA005;

  vm_exec_ldi(vm, instr);

  // Verify: R0 should contain value from memory[memory[0x3005]] = 0x5678
  ASSERT_UINT16_EQUAL("LDI instruction result", 0x5678, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test LDR instruction
char* test_ldr_instruction(void) {
  vm_t* vm = create_test_vm();

  // Set base register R1 to 0x3000
  vm->reg[1] = 0x3000;
  // Put test value at memory location 0x3003
  vm->memory[0x3003] = 0x9ABC;

  // CREATE LDR R0, R1, +3 instruction (0x6043)
  // Opcode: 0110, DR: 000, BaseR: 001, offset6: 000011 (3)
  uint16_t instr = 0x6043;

  vm_exec_ldr(vm, instr);

  // Verify: R0 should contain value from memory[R1 + 3] = 0x9ABC
  ASSERT_UINT16_EQUAL("LDR instruction result", 0x9ABC, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test LEA instruction
char* test_lea_instruction(void) {
  vm_t* vm = create_test_vm();

  vm->reg[LC3_R_PC] = 0x3000;

  // CREATE LEA R0, +10 instruction (0xE00A)
  // Opcode: 1110, DR: 000, PCoffset9: 000001010 (10)
  uint16_t instr = 0xE00A;

  vm_exec_lea(vm, instr);

  // Verify: R0 should contain effective address 0x3000 + 10 = 0x300A
  ASSERT_UINT16_EQUAL("LEA instruction result", 0x300A, vm->reg[0]);

  destroy_test_vm(vm);
}

// Test ST instruction
char* test_st_instruction(void) {
  vm_t* vm = create_test_vm();

  vm->reg[LC3_R_PC] = 0x3000;
  vm->reg[0] = 0xDEAD;  // Value to store

  // CREATE ST R0, +5 instruction (0x3005)
  uint16_t instr = 0x3005;

  vm_exec_st(vm, instr);

  // Verify: memory[0x3005] should contain 0xDEAD
  ASSERT_UINT16_EQUAL("ST instruction result", 0xDEAD, vm->memory[0x3005]);

  destroy_test_vm(vm);
}

// Test STI instruction
char* test_sti_instruction(void) {
  vm_t* vm = create_test_vm();

  vm->reg[LC3_R_PC] = 0x3000;
  vm->reg[0] = 0xBEEF;  // Value to store
  // Put pointer at memory location 0x3005
  vm->memory[0x3005] = 0x4000;

  // CREATE STI R0, +5 instruction (0xB005)
  uint16_t instr = 0xB005;

  vm_exec_sti(vm, instr);

  // Verify: memory[memory[0x3005]] = memory[0x4000] should contain 0xBEEF
  ASSERT_UINT16_EQUAL("STI instruction result", 0xBEEF, vm->memory[0x4000]);

  destroy_test_vm(vm);
}

// Test STR instruction
char* test_str_instruction(void) {
  vm_t* vm = create_test_vm();

  vm->reg[0] = 0xCAFE;  // Value to store
  vm->reg[1] = 0x3000;  // Base register

  // CREATE STR R0, R1, +7 instruction (0x7047)
  // Opcode: 0111, SR: 000, BaseR: 001, offset6: 000111 (7)
  uint16_t instr = 0x7047;

  vm_exec_str(vm, instr);

  // Verify: memory[R1 + 7] = memory[0x3007] should contain 0xCAFE
  ASSERT_UINT16_EQUAL("STR instruction result", 0xCAFE, vm->memory[0x3007]);

  destroy_test_vm(vm);
}

// Test TRAP HALT instruction
char* test_trap_halt(void) {
  vm_t* vm = create_test_vm();

  vm->running = true;

  // CREATE TRAP HALT instruction (0xF025)
  uint16_t instr = 0xF025;

  vm_exec_trap(vm, instr);

  // Verify: VM should be stopped
  ASSERT_TRUE("TRAP HALT stops VM", !vm->running);

  destroy_test_vm(vm);
}

// Test condition flag updates (using ADD as example)
char* test_condition_flags_positive(void) {
  vm_t* vm = create_test_vm();

  vm->reg[1] = 5;
  vm->reg[2] = 3;

  // ADD R0, R1, R2 (result = 8, positive)
  uint16_t instr = 0x1042;
  vm_exec_add(vm, instr);

  // Verify: Condition flag should be positive
  ASSERT_UINT16_EQUAL("Positive condition flag", LC3_FL_POS, vm->reg[LC3_R_COND]);

  destroy_test_vm(vm);
}

// Test condition flag updates - zero
char* test_condition_flags_zero(void) {
  vm_t* vm = create_test_vm();

  vm->reg[1] = 5;

  // ADD R0, R1, #-5 (result = 0)
  uint16_t instr = 0x107B;  // IMM5 = -5 = 11011
  vm_exec_add(vm, instr);

  // Verify: Condition flag should be zero
  ASSERT_UINT16_EQUAL("Zero condition flag", LC3_FL_ZRO, vm->reg[LC3_R_COND]);

  destroy_test_vm(vm);
}

// Test condition flag updates - negative
char* test_condition_flags_negative(void) {
  vm_t* vm = create_test_vm();

  vm->reg[1] = 3;

  // ADD R0, R1, #-5 (result = -2)
  uint16_t instr = 0x107B;  // IMM5 = -5 = 11011
  vm_exec_add(vm, instr);

  // Verify: Condition flag should be negative
  ASSERT_UINT16_EQUAL("Negative condition flag", LC3_FL_NEG, vm->reg[LC3_R_COND]);

  destroy_test_vm(vm);
}

// Run all VM tests
void run_vm_tests(void) {
  printf("Running VM Instruction Tests...\n\n");

  // ADD instruction tests
  RUN_TEST(test_add_register_mode);
  RUN_TEST(test_add_immediate_mode);
  RUN_TEST(test_add_negative_immediate);

  // AND instruction tests
  RUN_TEST(test_and_register_mode);
  RUN_TEST(test_and_immediate_mode);

  // NOT instruction test
  RUN_TEST(test_not_instruction);

  // Branch instruction tests
  RUN_TEST(test_br_branch_taken);
  RUN_TEST(test_br_branch_not_taken);

  // Jump instruction tests
  RUN_TEST(test_jmp_instruction);
  RUN_TEST(test_jsr_instruction);

  // Load instruction tests
  RUN_TEST(test_ld_instruction);
  RUN_TEST(test_ldi_instruction);
  RUN_TEST(test_ldr_instruction);
  RUN_TEST(test_lea_instruction);

  // Store instruction tests
  RUN_TEST(test_st_instruction);
  RUN_TEST(test_sti_instruction);
  RUN_TEST(test_str_instruction);

  // Trap instruction test
  RUN_TEST(test_trap_halt);

  // Condition flag tests
  RUN_TEST(test_condition_flags_positive);
  RUN_TEST(test_condition_flags_zero);
  RUN_TEST(test_condition_flags_negative);
}

#endif /* VM_TESTS_H */
