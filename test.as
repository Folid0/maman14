; test_err3.as - Range overflows and reserved names
mcro add
    move    $1, $2
mcroend

add:        addi    $0, 40000, $1
            subi    $1, -35000, $2
            hlt

OVERFLOWS:  .db     256
            .dh     70000