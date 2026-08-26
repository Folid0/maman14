; Invalid input demonstrating several assembler error types
BADREG: add $1,$2,$32
BADCOUNT: move $1,$2,$3
BADIMM: addi $4,40000,$5
BADCOMMAS: beq $1,,$2,TARGET
BADJUMP: jmp $40
BADBYTE: .db -129,0,128
BADHALF: .dh -32769,32768
BADWORD: .dw 12,
BADSTRING: .asciz text
.extern 7EXTERNAL
.entry 9ENTRY
hlt $1
