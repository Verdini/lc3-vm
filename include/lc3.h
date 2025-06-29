#ifndef LC3_H
#define LC3_H

#include <stdbool.h>
#include <stdint.h>

// LC-3 Architecture Constants
#define LC3_MEMORY_MAX (1 << 16) // 65536 memory locations
#define LC3_PC_START 0x3000      // Default PC start location

// LC-3 Registers
enum
{
    LC3_R_R0 = 0,
    LC3_R_R1,
    LC3_R_R2,
    LC3_R_R3,
    LC3_R_R4,
    LC3_R_R5,
    LC3_R_R6,
    LC3_R_R7,
    LC3_R_PC,   // Program Counter
    LC3_R_COND, // Condition Register
    LC3_R_COUNT
};

// LC-3 Opcodes
enum
{
    LC3_OP_BR = 0, // Branch
    LC3_OP_ADD,    // Add
    LC3_OP_LD,     // Load
    LC3_OP_ST,     // Store
    LC3_OP_JSR,    // Jump register
    LC3_OP_AND,    // Bitwise and
    LC3_OP_LDR,    // Load register
    LC3_OP_STR,    // Store register
    LC3_OP_RTI,    // Unused
    LC3_OP_NOT,    // Bitwise not
    LC3_OP_LDI,    // Load indirect
    LC3_OP_STI,    // Store indirect
    LC3_OP_JMP,    // Jump
    LC3_OP_RES,    // Reserved (unused)
    LC3_OP_LEA,    // Load effective address
    LC3_OP_TRAP    // Execute trap
};

// Condition Flags
enum
{
    LC3_FL_POS = 1 << 0, // P
    LC3_FL_ZRO = 1 << 1, // Z
    LC3_FL_NEG = 1 << 2, // N
};

// Trap Codes
enum
{
    LC3_TRAP_GETC = 0x20,  // Get character from keyboard, not echoed onto the terminal
    LC3_TRAP_OUT = 0x21,   // Output a character
    LC3_TRAP_PUTS = 0x22,  // Output a word string
    LC3_TRAP_IN = 0x23,    // Get character from keyboard, echoed onto the terminal
    LC3_TRAP_PUTSP = 0x24, // Output a byte string
    LC3_TRAP_HALT = 0x25   // Halt the program
};

// Memory Mapped Registers
enum
{
    LC3_MR_KBSR = 0xFE00, // Keyboard status
    LC3_MR_KBDR = 0xFE02  // Keyboard data
};

// LC-3 VM Structure
typedef struct
{
    uint16_t memory[LC3_MEMORY_MAX];
    uint16_t reg[LC3_R_COUNT];
    bool running;
} lc3_vm_t;

// Function declarations
void lc3_vm_init(lc3_vm_t *vm);
bool lc3_vm_load_program(lc3_vm_t *vm, const char *filename);
void lc3_vm_run(lc3_vm_t *vm);
void lc3_vm_shutdown(lc3_vm_t *vm);

// Utility functions
void lc3_update_flags(lc3_vm_t *vm, uint16_t r);
uint16_t lc3_sign_extend(uint16_t x, int bit_count);
uint16_t lc3_swap16(uint16_t val);
void lc3_mem_write(lc3_vm_t *vm, uint16_t address, uint16_t val);
uint16_t lc3_mem_read(lc3_vm_t *vm, uint16_t address);

// Instruction execution functions
void lc3_exec_add(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_and(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_not(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_br(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_jmp(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_jsr(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_ld(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_ldi(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_ldr(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_lea(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_st(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_sti(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_str(lc3_vm_t *vm, uint16_t instr);
void lc3_exec_trap(lc3_vm_t *vm, uint16_t instr);

#endif // LC3_H
