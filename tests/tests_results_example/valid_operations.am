; Valid input demonstrating instructions, directives, entries and externals
.entry START
.entry BYTEDATA
.extern EXTFUNC
.extern EXTDATA

START: add $1,$2,$3
       sub $4,$5,$6
       and $7,$8,$9
       or $10,$11,$12
       nor $13,$14,$15
       move $16,$17
       mvhi $18,$19
       mvlo $20,$21
       addi $1,25,$2
       subi $3,-14,$4
       andi $5,255,$6
       ori $7,-1,$8
       nori $9,42,$10
       beq $1,$2,EQUAL
       bne $3,$4,DIFFERENT
       blt $5,$6,LOWER
       bgt $7,$8,HIGHER
EQUAL: lb $1,4,$2
       sb $3,-8,$4
DIFFERENT: lw $5,12,$6
       sw $7,16,$8
LOWER: lh $9,20,$10
       sh $11,-24,$12
HIGHER: jmp FINISH
       la BYTEDATA
       la EXTDATA
       call EXTFUNC
       jmp $31
FINISH: hlt

BYTEDATA: .db -128,-1,0,1,127
HALFDATA: .dh -32768,-200,0,300,32767
WORDDATA: .dw -2147483648,-50000,0,70000,2147483647
TEXTDATA: .asciz "Assembler test"
