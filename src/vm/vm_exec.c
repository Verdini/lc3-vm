#include <stdio.h>

#include "../../include/vm/vm.h"

/*
  INSTRUCTION FORMAT - 16 bits

  |----------------------------------------------|
  | 15 - 12 | 11 - 9 | 8 - 6 | 5 | 4 - 0         |
  |----------------------------------------------|
  | Opcode  | DR     | SR1   | 1 | IMM5          |
  |----------------------------------------------|
  | 15 - 12 | 11 - 9 | 8 - 6 | 5 | 4 - 3 | 2 - 0 |
  |----------------------------------------------|
  | Opcode  | DR     | SR1   | 0 |   00  |  SR2  |
  |----------------------------------------------|

  15 - 12 - Opcode (4 bits)
  11 - 9  - DR (Destination Register) (3 bits)
  8 - 6   - SR1 (Source Register 1) (3 bits)
  5       - Bit indicating immediate mode (1 for immediate, 0 for register)
  4 - 0   - IMM5 (5 bits) or SR2 (3 bits if not immediate)
*/

#define DR(instr) ((instr >> 9) & 0x7)
#define SR1(instr) ((instr >> 6) & 0x7)
#define IMFLAG(instr) ((instr >> 5) & 0x1)
#define SR2(instr) ((instr) & 0x7)
#define IMM5(instr) ((instr) & 0x1F)
#define SIGN_EXTEND(value, bits) \
  ((value) & (1 << ((bits) - 1)) ? (value) | ~((1 << (bits)) - 1) : (value))

void vm_update_flags(vm_t* vm, uint16_t reg) {
  if (vm->reg[reg] == 0) {
    vm->reg[LC3_R_COND] = LC3_FL_ZRO;  // Zero flag
  } else if (vm->reg[reg] >> 15) {
    vm->reg[LC3_R_COND] = LC3_FL_NEG;  // Negative flag
  } else {
    vm->reg[LC3_R_COND] = LC3_FL_POS;  // Positive flag
  }
}

void vm_exec_add(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t r1 = SR1(instr);
  uint16_t imm_flag = IMFLAG(instr);

  if (imm_flag) {
    uint16_t imm5 = SIGN_EXTEND(instr & 0x1F, 5);
    vm->reg[r0] = vm->reg[r1] + imm5;
  } else {
    uint16_t r2 = instr & 0x7;
    vm->reg[r0] = vm->reg[r1] + vm->reg[r2];
  }

  vm_update_flags(vm, r0);
}

void vm_exec_and(vm_t* vm, uint16_t instr) {
  // TODO: Implement AND instruction
  printf("AND instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_not(vm_t* vm, uint16_t instr) {
  // TODO: Implement NOT instruction
  printf("NOT instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_br(vm_t* vm, uint16_t instr) {
  // TODO: Implement BR instruction
  printf("BR instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_jmp(vm_t* vm, uint16_t instr) {
  // TODO: Implement JMP instruction
  printf("JMP instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_jsr(vm_t* vm, uint16_t instr) {
  // TODO: Implement JSR instruction
  printf("JSR instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_ld(vm_t* vm, uint16_t instr) {
  // TODO: Implement LD instruction
  printf("LD instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_ldi(vm_t* vm, uint16_t instr) {
  // TODO: Implement LDI instruction
  printf("LDI instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_ldr(vm_t* vm, uint16_t instr) {
  // TODO: Implement LDR instruction
  printf("LDR instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_lea(vm_t* vm, uint16_t instr) {
  // TODO: Implement LEA instruction
  printf("LEA instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_st(vm_t* vm, uint16_t instr) {
  // TODO: Implement ST instruction
  printf("ST instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_sti(vm_t* vm, uint16_t instr) {
  // TODO: Implement STI instruction
  printf("STI instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_str(vm_t* vm, uint16_t instr) {
  // TODO: Implement STR instruction
  printf("STR instruction not implemented yet\n");
  vm->running = false;
}

void vm_exec_trap(vm_t* vm, uint16_t instr) {
  uint16_t trap_vect = instr & 0xFF;

  switch (trap_vect) {
    case LC3_TRAP_GETC:
      // TODO: Implement GETC trap
      printf("GETC trap not implemented yet\n");
      vm->running = false;
      break;
    case LC3_TRAP_OUT:
      // TODO: Implement OUT trap
      printf("OUT trap not implemented yet\n");
      vm->running = false;
      break;
    case LC3_TRAP_PUTS:
      // TODO: Implement PUTS trap
      printf("PUTS trap not implemented yet\n");
      vm->running = false;
      break;
    case LC3_TRAP_IN:
      // TODO: Implement IN trap
      printf("IN trap not implemented yet\n");
      vm->running = false;
      break;
    case LC3_TRAP_PUTSP:
      // TODO: Implement PUTSP trap
      printf("PUTSP trap not implemented yet\n");
      vm->running = false;
      break;
    case LC3_TRAP_HALT:
      printf("HALT\n");
      vm->running = false;
      break;
    default:
      printf("Unknown trap: 0x%02X\n", trap_vect);
      vm->running = false;
      break;
  }
}
