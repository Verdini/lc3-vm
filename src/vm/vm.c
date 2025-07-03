#include "../../include/vm/vm.h"

#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../include/vm/vm_exec.h"

uint16_t swap16(uint16_t val) { return (val << 8) | (val >> 8); }

uint16_t vm_mem_read(vm_t* vm, uint16_t address) {
  if (address == LC3_MR_KBSR) {
    // Check if keyboard input is available
    // This is a simplified implementation
    return 0x0000;
  } else if (address == LC3_MR_KBDR) {
    // Read keyboard data
    // This is a simplified implementation
    return 0x0000;
  }

  return vm->memory[address];
}

vm_t* vm_init(const char* filename) {
  FILE* file = fopen(filename, "rb");
  if (!file) return NULL;

  vm_t* vm = malloc(sizeof(vm_t));

  // Clear memory
  memset(vm->memory, 0, sizeof(vm->memory));
  // Clear registers
  memset(vm->reg, 0, sizeof(vm->reg));
  // Set PC to start location
  vm->reg[LC3_R_PC] = LC3_PC_START;
  // Set condition flag to zero
  vm->reg[LC3_R_COND] = LC3_FL_ZRO;

  // Read origin address
  uint16_t origin;
  fread(&origin, sizeof(origin), 1, file);
  origin = swap16(origin);

  // Read program into memory
  uint16_t* p = vm->memory + origin;
  uint16_t* max = vm->memory + LC3_MEMORY_MAX;

  while (p < max && fread(p, sizeof(uint16_t), 1, file)) {
    *p = swap16(*p);
    ++p;
  }

  fclose(file);

  printf("Program loaded at origin 0x%04X\n", origin);
  return 0;
}

int vm_run(const char* filename) {
  vm_t* vm = vm_init(filename);
  if (!vm) {
    fprintf(stderr, "Error: Could not initialize VM with file %s\n", filename);
    return 1;
  }
  vm->running = true;
  int result = 0;
  while (vm->running) {
    // Fetch instruction
    uint16_t instr = vm_mem_read(vm, vm->reg[LC3_R_PC]++);

    // Decode opcode
    uint16_t op = instr >> 12;

    // Execute instruction
    switch (op) {
      case LC3_OP_ADD:
        vm_exec_add(vm, instr);
        break;
      case LC3_OP_AND:
        vm_exec_and(vm, instr);
        break;
      case LC3_OP_NOT:
        vm_exec_not(vm, instr);
        break;
      case LC3_OP_BR:
        vm_exec_br(vm, instr);
        break;
      case LC3_OP_JMP:
        vm_exec_jmp(vm, instr);
        break;
      case LC3_OP_JSR:
        vm_exec_jsr(vm, instr);
        break;
      case LC3_OP_LD:
        vm_exec_ld(vm, instr);
        break;
      case LC3_OP_LDI:
        vm_exec_ldi(vm, instr);
        break;
      case LC3_OP_LDR:
        vm_exec_ldr(vm, instr);
        break;
      case LC3_OP_LEA:
        vm_exec_lea(vm, instr);
        break;
      case LC3_OP_ST:
        vm_exec_st(vm, instr);
        break;
      case LC3_OP_STI:
        vm_exec_sti(vm, instr);
        break;
      case LC3_OP_STR:
        vm_exec_str(vm, instr);
        break;
      case LC3_OP_TRAP:
        vm_exec_trap(vm, instr);
        break;
      case LC3_OP_RES:
      case LC3_OP_RTI:
      default:
        printf("Unknown opcode: 0x%04X\n", op);
        vm->running = false;
        result = 1;
        break;
    }
  }
  free(vm);
  return result;
}
