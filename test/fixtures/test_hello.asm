.ORIG x3000
; Simple test program for LC-3 VM
    LEA R0, HELLO      ; Load address of hello string
    PUTS               ; Output the string
    HALT               ; Halt the program

HELLO .STRINGZ "Hello, LC-3 Tests!"
.END
