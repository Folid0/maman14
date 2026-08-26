; Invalid input demonstrating pre-assembler errors
mcro 1BAD
add $1,$2,$3
mcroend

mcro add
sub $4,$5,$6
mcroend

mcro TOO_MANY extra
or $7,$8,$9
mcroend

mcro BAD_END
nor $10,$11,$12
mcroend extra
