; Valid input demonstrating ordinary, repeated and empty macros
.entry MAIN
.entry MESSAGE
.extern PRINTTEXT

mcro CALCULATE
add $1,$2,$3
subi $3,1,$3
mcroend

mcro LOAD_TEXT
la MESSAGE
call PRINTTEXT
mcroend

mcro EMPTY_MACRO
mcroend

MAIN: ori $0,10,$1
CALCULATE
CALCULATE
LOAD_TEXT
EMPTY_MACRO
beq $3,$0,DONE
jmp MAIN
DONE: hlt

MESSAGE: .asciz "Macro expansion works"
VALUES: .dw 10,20,30
