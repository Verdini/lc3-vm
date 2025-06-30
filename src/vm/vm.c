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

void vm_init(lc3_vm_t *vm)
{
    // Clear memory
    memset(vm->memory, 0, sizeof(vm->memory));

    // Clear registers
    memset(vm->reg, 0, sizeof(vm->reg));

    // Set PC to start location
    vm->reg[LC3_R_PC] = LC3_PC_START;

    // Set condition flag to zero
    vm->reg[LC3_R_COND] = LC3_FL_ZRO;

    // VM is ready to run
    vm->running = true;

    printf("LC-3 VM initialized\n");
}

bool vm_load_program(lc3_vm_t *vm, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        return false;
    }

    // Read origin address
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = lc3_swap16(origin);

    // Read program into memory
    uint16_t *p = vm->memory + origin;
    uint16_t *max = vm->memory + LC3_MEMORY_MAX;

    while (p < max && fread(p, sizeof(uint16_t), 1, file))
    {
        *p = lc3_swap16(*p);
        ++p;
    }

    fclose(file);

    printf("Program loaded at origin 0x%04X\n", origin);
    return true;
}

void vm_run(lc3_vm_t *vm)
{
    while (vm->running)
    {
        // Fetch instruction
        uint16_t instr = lc3_mem_read(vm, vm->reg[LC3_R_PC]++);

        // Decode opcode
        uint16_t op = instr >> 12;

        // Execute instruction
        switch (op)
        {
        case LC3_OP_ADD:
            lc3_exec_add(vm, instr);
            break;
        case LC3_OP_AND:
            lc3_exec_and(vm, instr);
            break;
        case LC3_OP_NOT:
            lc3_exec_not(vm, instr);
            break;
        case LC3_OP_BR:
            lc3_exec_br(vm, instr);
            break;
        case LC3_OP_JMP:
            lc3_exec_jmp(vm, instr);
            break;
        case LC3_OP_JSR:
            lc3_exec_jsr(vm, instr);
            break;
        case LC3_OP_LD:
            lc3_exec_ld(vm, instr);
            break;
        case LC3_OP_LDI:
            lc3_exec_ldi(vm, instr);
            break;
        case LC3_OP_LDR:
            lc3_exec_ldr(vm, instr);
            break;
        case LC3_OP_LEA:
            lc3_exec_lea(vm, instr);
            break;
        case LC3_OP_ST:
            lc3_exec_st(vm, instr);
            break;
        case LC3_OP_STI:
            lc3_exec_sti(vm, instr);
            break;
        case LC3_OP_STR:
            lc3_exec_str(vm, instr);
            break;
        case LC3_OP_TRAP:
            lc3_exec_trap(vm, instr);
            break;
        case LC3_OP_RES:
        case LC3_OP_RTI:
        default:
            printf("Unknown opcode: 0x%04X\n", op);
            vm->running = false;
            break;
        }
    }
}

void vm_shutdown(lc3_vm_t *vm)
{
    vm->running = false;
    printf("LC-3 VM shutdown\n");
}

// Utility Functions
void lc3_update_flags(lc3_vm_t *vm, uint16_t r)
{
    if (vm->reg[r] == 0)
    {
        vm->reg[LC3_R_COND] = LC3_FL_ZRO;
    }
    else if (vm->reg[r] >> 15) // Check sign bit
    {
        vm->reg[LC3_R_COND] = LC3_FL_NEG;
    }
    else
    {
        vm->reg[LC3_R_COND] = LC3_FL_POS;
    }
}

uint16_t lc3_sign_extend(uint16_t x, int bit_count)
{
    if ((x >> (bit_count - 1)) & 1)
    {
        x |= (0xFFFF << bit_count);
    }
    return x;
}

uint16_t lc3_swap16(uint16_t val) { return (val << 8) | (val >> 8); }

void lc3_mem_write(lc3_vm_t *vm, uint16_t address, uint16_t val) { vm->memory[address] = val; }

uint16_t lc3_mem_read(lc3_vm_t *vm, uint16_t address)
{
    if (address == LC3_MR_KBSR)
    {
        // Check if keyboard input is available
        // This is a simplified implementation
        return 0x0000;
    }
    else if (address == LC3_MR_KBDR)
    {
        // Read keyboard data
        // This is a simplified implementation
        return 0x0000;
    }

    return vm->memory[address];
}

// Instruction Implementation Stubs
// TODO: Implement these functions

void lc3_exec_add(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement ADD instruction
    printf("ADD instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_and(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement AND instruction
    printf("AND instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_not(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement NOT instruction
    printf("NOT instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_br(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement BR instruction
    printf("BR instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_jmp(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement JMP instruction
    printf("JMP instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_jsr(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement JSR instruction
    printf("JSR instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_ld(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement LD instruction
    printf("LD instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_ldi(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement LDI instruction
    printf("LDI instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_ldr(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement LDR instruction
    printf("LDR instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_lea(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement LEA instruction
    printf("LEA instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_st(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement ST instruction
    printf("ST instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_sti(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement STI instruction
    printf("STI instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_str(lc3_vm_t *vm, uint16_t instr)
{
    // TODO: Implement STR instruction
    printf("STR instruction not implemented yet\n");
    vm->running = false;
}

void lc3_exec_trap(lc3_vm_t *vm, uint16_t instr)
{
    uint16_t trap_vect = instr & 0xFF;

    switch (trap_vect)
    {
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
