#ifndef VM_H
#define VM_H

#include <stdbool.h>
#include <stdint.h>

#include "../lc3/lc3.h"

// LC-3 VM Structure
typedef struct {
  uint16_t memory[LC3_MEMORY_MAX];
  uint16_t reg[LC3_R_COUNT];
  bool running;
} lc3_vm_t;

// Function declarations
void vm_init(lc3_vm_t* vm);
bool vm_load_program(lc3_vm_t* vm, const char* filename);
void vm_run(lc3_vm_t* vm);
void vm_shutdown(lc3_vm_t* vm);

// Utility functions
void lc3_update_flags(lc3_vm_t* vm, uint16_t r);
uint16_t lc3_sign_extend(uint16_t x, int bit_count);
uint16_t lc3_swap16(uint16_t val);
void lc3_mem_write(lc3_vm_t* vm, uint16_t address, uint16_t val);
uint16_t lc3_mem_read(lc3_vm_t* vm, uint16_t address);

// Instruction execution functions
void lc3_exec_add(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_and(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_not(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_br(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_jmp(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_jsr(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_ld(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_ldi(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_ldr(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_lea(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_st(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_sti(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_str(lc3_vm_t* vm, uint16_t instr);
void lc3_exec_trap(lc3_vm_t* vm, uint16_t instr);

#endif  // VM_H
