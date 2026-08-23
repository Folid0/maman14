; Valid: forward, backward, and zero-distance branch resolution
START: add $1,$2,$3
       beq $1,$2,FORWARD
       bne $3,$4,START
SELF:  blt $5,$6,SELF
       jmp FORWARD
FORWARD: move $7,$8
       la DATA1
       call FUNC
       hlt
FUNC:  addi $0,1,$1
       jmp $0
DATA1: .dw 123
.entry START
.entry DATA1
