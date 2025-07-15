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
  uint16_t r0 = DR(instr);
  uint16_t r1 = SR1(instr);
  uint16_t imm_flag = IMFLAG(instr);

  if (imm_flag) {
    uint16_t imm5 = SIGN_EXTEND(IMM5(instr), 5);
    vm->reg[r0] = vm->reg[r1] & imm5;
  } else {
    uint16_t r2 = SR2(instr);
    vm->reg[r0] = vm->reg[r1] & vm->reg[r2];
  }

  vm_update_flags(vm, r0);
}

void vm_exec_not(vm_t* vm, uint16_t instr) {
  uint16_t dr = DR(instr);
  uint16_t sr = SR1(instr);

  // Perform bitwise NOT operation
  vm->reg[dr] = ~vm->reg[sr];

  vm_update_flags(vm, dr);
}

void vm_exec_br(vm_t* vm, uint16_t instr) {
  uint16_t n_flag = (instr >> 11) & 0x1;               // Negative flag
  uint16_t z_flag = (instr >> 10) & 0x1;               // Zero flag
  uint16_t p_flag = (instr >> 9) & 0x1;                // Positive flag
  uint16_t pc_offset = SIGN_EXTEND(instr & 0x1FF, 9);  // PC offset

  if ((n_flag && (vm->reg[LC3_R_COND] == LC3_FL_NEG)) ||
      (z_flag && (vm->reg[LC3_R_COND] == LC3_FL_ZRO)) ||
      (p_flag && (vm->reg[LC3_R_COND] == LC3_FL_POS))) {
    vm->reg[LC3_R_PC] += pc_offset;
  }
}

void vm_exec_jmp(vm_t* vm, uint16_t instr) {
  uint16_t base_r = SR1(instr);

  // Set the PC to the value in the base register
  vm->reg[LC3_R_PC] = vm->reg[base_r];

  vm_update_flags(vm, LC3_R_PC);
}

void vm_exec_jsr(vm_t* vm, uint16_t instr) {
  uint16_t pc_offset = SIGN_EXTEND(instr & 0x7FF, 11);  // PC offset
  uint16_t long_flag = (instr >> 11) & 0x1;             // Long flag

  // Save the current PC to R7
  vm->reg[LC3_R_R7] = vm->reg[LC3_R_PC];

  if (long_flag) {
    // JSR instruction with PC offset
    vm->reg[LC3_R_PC] += pc_offset;
  } else {
    // JSRR instruction, using base register
    uint16_t base_r = SR1(instr);
    vm->reg[LC3_R_PC] = vm->reg[base_r];
  }

  vm_update_flags(vm, LC3_R_PC);
}

void vm_exec_ld(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t pc_offset = SIGN_EXTEND(instr & 0x1FF, 9);  // PC offset

  // Load the value from memory at the calculated address
  vm->reg[r0] = vm->memory[vm->reg[LC3_R_PC] + pc_offset];

  // Update flags based on the loaded value
  vm_update_flags(vm, r0);
}

void vm_exec_ldi(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t pc_offset = SIGN_EXTEND(instr & 0x1FF, 9);  // PC offset

  // Load the address from memory at the calculated address
  uint16_t address = vm->reg[LC3_R_PC] + pc_offset;
  uint16_t value = vm->memory[address];

  // Load the value from the address stored in memory
  vm->reg[r0] = vm->memory[value];

  vm_update_flags(vm, r0);
}

void vm_exec_ldr(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t r1 = SR1(instr);
  uint16_t offset = SIGN_EXTEND(instr & 0x3F, 6);  // Offset

  // Load the value from memory at the address calculated using base register
  // and offset
  vm->reg[r0] = vm->memory[vm->reg[r1] + offset];

  vm_update_flags(vm, r0);
}

void vm_exec_lea(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t pc_offset = SIGN_EXTEND(instr & 0x1FF, 9);  // PC offset

  // Load the effective address into the register
  vm->reg[r0] = vm->reg[LC3_R_PC] + pc_offset;

  vm_update_flags(vm, r0);
}

void vm_exec_st(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t pc_offset = SIGN_EXTEND(instr & 0x1FF, 9);  // PC offset

  // Store the value from the register into memory at the calculated address
  vm->memory[vm->reg[LC3_R_PC] + pc_offset] = vm->reg[r0];

  vm_update_flags(vm, r0);
}

void vm_exec_sti(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t pc_offset = SIGN_EXTEND(instr & 0x1FF, 9);  // PC offset

  // Load the address from memory at the calculated address
  uint16_t address = vm->reg[LC3_R_PC] + pc_offset;

  // Store the value from the register into memory at the address stored in
  // memory
  vm->memory[vm->memory[address]] = vm->reg[r0];

  vm_update_flags(vm, r0);
}

void vm_exec_str(vm_t* vm, uint16_t instr) {
  uint16_t r0 = DR(instr);
  uint16_t r1 = SR1(instr);
  uint16_t offset = SIGN_EXTEND(instr & 0x3F, 6);  // Offset

  // Store the value from the register into memory at the address calculated
  // using base register and offset
  vm->memory[vm->reg[r1] + offset] = vm->reg[r0];

  vm_update_flags(vm, r0);
}

void vm_exec_trap(vm_t* vm, uint16_t instr) {
  uint16_t trap_vect = instr & 0xFF;

  switch (trap_vect) {
    case LC3_TRAP_GETC:  // Get character from keyboard, not echoed
      vm->reg[LC3_R_R0] = (uint16_t)getchar();
      break;

    case LC3_TRAP_OUT:  // x21: Output a character
      putchar((char)vm->reg[LC3_R_R0]);
      fflush(stdout);
      break;

    case LC3_TRAP_PUTS:  // Output a string
    {
      uint16_t addr = vm->reg[LC3_R_R0];
      char c;
      while ((c = (char)vm->memory[addr]) != 0) {
        putchar(c);
        addr++;
      }
      fflush(stdout);
    } break;

    case LC3_TRAP_IN:  // Input a character and echo it
    {
      vm->reg[LC3_R_R0] = (uint16_t)getchar();
      putchar((char)vm->reg[LC3_R_R0]);
      fflush(stdout);
    } break;

    case LC3_TRAP_PUTSP:  // Output a string of bytes (two chars per word)
    {
      uint16_t addr = vm->reg[LC3_R_R0];
      uint16_t word;
      while ((word = vm->memory[addr]) != 0) {
        char c1 = word & 0xFF;
        putchar(c1);
        char c2 = word >> 8;
        if (c2) putchar(c2);  // Only print the second char if it's not null
        addr++;
      }
      fflush(stdout);
    } break;
    case LC3_TRAP_HALT:
      vm->running = false;
      break;
    default:
      printf("Unknown trap: 0x%02X\n", trap_vect);
      vm->running = false;
      break;
  }
}
