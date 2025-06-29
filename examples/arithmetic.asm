; Simple arithmetic test program for LC-3
; Tests ADD, AND, NOT operations

.ORIG x3000

; Load two numbers into registers
LD R1, NUM1     ; Load 5 into R1
LD R2, NUM2     ; Load 3 into R2

; Test ADD operation: R3 = R1 + R2 (5 + 3 = 8)
ADD R3, R1, R2

; Test ADD with immediate: R4 = R1 + 2 (5 + 2 = 7)
ADD R4, R1, #2

; Test AND operation: R5 = R1 & R2 (5 & 3 = 1)
AND R5, R1, R2

; Test NOT operation: R6 = ~R1 (~5 = -6 in two's complement)
NOT R6, R1

; Store results in memory for inspection
ST R3, RESULT1  ; Store R1 + R2
ST R4, RESULT2  ; Store R1 + 2
ST R5, RESULT3  ; Store R1 & R2
ST R6, RESULT4  ; Store ~R1

; Print success message
LEA R0, SUCCESS_MSG
TRAP x22

; Halt the program
TRAP x25

; Data section
NUM1        .FILL #5
NUM2        .FILL #3
RESULT1     .BLKW 1
RESULT2     .BLKW 1
RESULT3     .BLKW 1
RESULT4     .BLKW 1
SUCCESS_MSG .STRINGZ "Arithmetic test completed!"

.END
