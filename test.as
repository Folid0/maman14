.extern ext_lbl
.entry ent_lbl

    add $3, $5, $9
    sub $1, $2, $3
    move $20, $4

    addi $9, -45, $8
    ori $9, -5, $2
    lw $7, 12, $5
    sb $1, 0, $2

    jmp $4
    hlt

.db 6, -9, 15
.dh 27056, -123
.dw 31, -12, 0
.asciz "test"