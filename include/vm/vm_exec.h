#ifndef VM_EXEC_H
#define VM_EXEC_H

#include "vm.h"

void vm_exec_add(vm_t* vm, uint16_t instr);
void vm_exec_and(vm_t* vm, uint16_t instr);
void vm_exec_not(vm_t* vm, uint16_t instr);
void vm_exec_br(vm_t* vm, uint16_t instr);
void vm_exec_jmp(vm_t* vm, uint16_t instr);
void vm_exec_jsr(vm_t* vm, uint16_t instr);
void vm_exec_ld(vm_t* vm, uint16_t instr);
void vm_exec_ldi(vm_t* vm, uint16_t instr);
void vm_exec_ldr(vm_t* vm, uint16_t instr);
void vm_exec_lea(vm_t* vm, uint16_t instr);
void vm_exec_st(vm_t* vm, uint16_t instr);
void vm_exec_sti(vm_t* vm, uint16_t instr);
void vm_exec_str(vm_t* vm, uint16_t instr);
void vm_exec_trap(vm_t* vm, uint16_t instr);

#endif  // VM_EXEC_H
