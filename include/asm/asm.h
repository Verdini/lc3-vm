#ifndef ASM_H
#define ASM_H

#define LC3_REGISTERS \
  {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "COND", "PC"}

#define LC3_INSTRUCTION_SET                                       \
  {"BR",   "ADD",   "LD",   "ST",    "JSR",   "AND",     "LDR",   \
   "STR",  "RTI",   "NOT",  "LDI",   "STI",   "JMP",     "RES",   \
   "LEA",  "TRAP",  "GETC", "OUT",   "PUTS",  "IN",      "PUTSP", \
   "HALT", ".ORIG", ".END", ".FILL", ".BLKW", ".STRINGZ"}

int asm_run(const char* input_filename, const char* output_filename);

#endif  // ASM_H
