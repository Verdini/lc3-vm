# LC-3 Quick Reference Guide

## Memory map

The LC-3 has 65,536 memory locations (x0000 - xFFFF)

- x0000 - x00FF (256 bytes) - Trap Vector Table
- x0100 - x01FF: (256 bytes) - Interrupt Vector Table
- x0200 - x2FFF: (11,776 bytes) - OS and Supervisor Stack
- x3000 - xFDFF: (52,224 bytes) - User Program Area
- xFE00 - xFFFF (512 bytes) - Device Register Addresses

Big-indian (most significative byte first)

Example:

```
.ORIG x3000
ADD R0, R1, x0

x3000 32 ; OP 0001, R0 000, R1 001, IMF 1, IMM5 00000
x3001 96 ; 00010000 01100000
```

## Registers

The LC-3 has 8 general-purpose registers (R0-R7), each 16 bits wide:

- R0: General purpose register
- R1: General purpose register
- R2: General purpose register
- R3: General purpose register
- R4: General purpose register
- R5: General purpose register
- R6: Often used as stack pointer (SP)
- R7: Automatically used for return address by JSR/JSRR instructions
- PC (Program Counter): 16-bit register that holds the memory address of the next instruction to be executed. It is automatically incremented after each instruction fetch.
- COND (Condition Codes): 3-bit register that stores the result of the most recent ALU operation:
  - N: Set when result is negative (bit 2)
  - Z: Set when result is zero (bit 1)
  - P: Set when result is positive (bit 0)

## Instruction Set

### Symbol Legend

- SR1, SR2: Source registers used by instruction
- DR: Destination register that will hold the instruction's result
- LABEL: Label used by instruction
- imm5: Immediate value (5 bits)
- offset6: Offset value (6 bits)
- trapvector8: 8-bit value specifying trap service routine

### Arithmetic and Logical Operations

- ADD: Add values

  - Format: `ADD DR, SR1, SR2` or `ADD DR, SR1, imm5`
  - Description: Adds values in SR1 and SR2/imm5, stores result in DR
  - Example: `ADD R1, R2, #5` (adds 5 to R2, stores in R1)

- AND: Bitwise AND

  - Format: `AND DR, SR1, SR2` or `AND DR, SR1, imm5`
  - Description: Performs bitwise AND on SR1 and SR2/imm5, stores in DR
  - Example: `AND R0, R1, R2` (performs AND on R1 and R2, stores in R0)

- NOT: Bitwise NOT
  - Format: `NOT DR, SR1`
  - Description: Performs bitwise NOT on SR1, stores result in DR
  - Example: `NOT R0, R1` (inverts all bits in R1, stores in R0)

### Control Flow

- BR: Conditional branch

  - Format: `BR(n/z/p) LABEL`
  - Description: Branch to LABEL if condition codes match specified flags (n=negative, z=zero, p=positive)
  - Example: `BRz LPBODY` (branch if zero flag set), `BRnp ALT1` (branch if negative or positive)

- JMP: Unconditional jump

  - Format: `JMP SR1`
  - Description: Jump to address in SR1
  - Example: `JMP R1` (jump to address in R1)

- JSR: Jump to subroutine

  - Format: `JSR LABEL`
  - Description: Store next instruction address in R7, jump to subroutine at LABEL
  - Example: `JSR POP` (save return address, jump to POP subroutine)

- JSRR: Jump to subroutine register

  - Format: `JSRR SR1`
  - Description: Store next instruction address in R7, jump to address in SR1
  - Example: `JSRR R3` (save return address, jump to address in R3)

- RET: Return from subroutine

  - Format: `RET`
  - Description: Return to address in R7 (equivalent to JMP R7)
  - Example: `RET`

- RTI: Return from interrupt
  - Format: `RTI`
  - Description: Return from interrupt using address popped from supervisor stack
  - Note: Only usable in supervisor mode

### Memory Operations

- LD: Load

  - Format: `LD DR, LABEL`
  - Description: Load value at LABEL's memory location into DR
  - Example: `LD R2, VAR1` (loads value at VAR1 into R2)

- LDI: Load indirect

  - Format: `LDI DR, LABEL`
  - Description: Load value at address stored at LABEL's location into DR
  - Example: `LDI R3, ADDR1` (if ADDR1 contains x3100 and x3100 contains 8, load 8 into R3)

- LDR: Load register offset

  - Format: `LDR DR, SR1, offset6`
  - Description: Load value from memory at (SR1 + offset6) into DR
  - Example: `LDR R3, R4, #-2` (load value at address R4-2 into R3)

- LEA: Load effective address

  - Format: `LEA DR, LABEL`
  - Description: Load address of LABEL into DR
  - Example: `LEA R1, DATA1` (load address of DATA1 into R1)

- ST: Store

  - Format: `ST SR1, LABEL`
  - Description: Store value in SR1 into memory at LABEL
  - Example: `ST R1, VAR3` (store R1's value at VAR3)

- STI: Store indirect

  - Format: `STI SR1, LABEL`
  - Description: Store value in SR1 into memory at address stored at LABEL
  - Example: `STI R2, ADDR2` (if ADDR2 contains x3101, store R2 at x3101)

- STR: Store register offset
  - Format: `STR SR1, SR2, offset6`
  - Description: Store value in SR1 at memory location (SR2 + offset6)
  - Example: `STR R2, R1, #4` (store R2 at address R1+4)

### System Operations

- TRAP: System call
  - Format: `TRAP trapvector8`
  - Description: Execute system service specified by trapvector8
  - Example: `TRAP x25` (halt program execution)

#### TRAP Routines

- x20 (GETC): Read one character from keyboard into R0 (no echo)
- x21 (OUT): Output character in R0 to console
- x22 (PUTS): Output null-terminated string starting at address in R0
- x23 (IN): Read character from keyboard into R0 with echo
- x24 (PUTSP): Output null-terminated string with two characters per memory location
- x25 (HALT): End program execution

## Pseudo-ops

- .ORIG: `.ORIG #` - Set starting address for code segment
- .FILL: `.FILL #` - Place value # at current location
- .BLKW: `.BLKW #` - Reserve # memory locations
- .STRINGZ: `.STRINGZ "<String>"` - Place null-terminated string
- .END: `.END` - End of assembly code

Example:

```
.ORIG x3100
.STRINGZ "Sunday"

x3100 0053 ; S
x3101 0075 ; u
x3102 006e ; n
x3103 0064 ; d
x3104 0061 ; a
x3105 0079 ; y
x3106 0000 ; NUL
```
