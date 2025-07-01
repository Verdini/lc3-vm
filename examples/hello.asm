; Simple Hello World program for LC-3
; This program demonstrates basic LC-3 operations

.ORIG x3000

; Load address of hello string into R0
LEA R0, HELLO_STR

; Call PUTS trap to print the string
TRAP x22

; Load a newline character and print it
LD R0, NEWLINE
TRAP x21

; Load address of number and print it as character
LD R0, NUMBER
TRAP x21

; Print another newline
LD R0, NEWLINE
TRAP x21

; Halt the program
TRAP x25

HELLO_STR: .STRINGZ "Hello, LC-3!"
NEWLINE:   .FILL x000A   ; ASCII newline character
NUMBER:    .FILL x0041   ; ASCII 'A' character

.END
