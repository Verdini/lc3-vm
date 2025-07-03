#include <stdio.h>

#include "../../include/vm/vm.h"

void vm_exec_add(vm_t* vm, uint16_t instr) {
  // TODO: Implement ADD instruction
  printf("ADD instruction not implemented yet\n");
  vm->running = false;
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
