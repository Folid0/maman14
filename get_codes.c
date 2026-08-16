#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"
#include "data_table.h"
#include "reserved_word.h"
#include "writing_to_binary.h"

int get_register_num(char *name) {
    if (name[0] != '$') {
        return -1; /* Not a register */
    }
    int reg_num = atoi(name + 1); /* Convert the number part to an integer */
    if (reg_num < 0 || reg_num > 31) {
        return -1; /* Invalid register number */
    }
    return reg_num;
}


/*returns the opcode for the given r type instruction*/
/*returns -1 if the instruction is invalid*/
int get_opcode(char *name) {
    /*r type*/
    if (strcmp(name, "add") == 0) {
        return 0;
    } else if (strcmp(name, "sub") == 0) {
        return 0;
    } else if (strcmp(name, "and") == 0) {
        return 0;
    } else if (strcmp(name, "or") == 0) {
        return 0;
    } else if (strcmp(name, "nor") == 0) {
        return 0;
    } else if (strcmp(name, "move") == 0) {
        return 1;
    } else if (strcmp(name, "mvhi") == 0) {
        return 1;
    } else if (strcmp(name, "mvlo") == 0) {
        return 1;
    }
    
    /*i type*/
    else if (strcmp(name, "addi") == 0) {
        return 10;
    } else if (strcmp(name, "subi") == 0) {
        return 11;
    } else if (strcmp(name, "andi") == 0) {
        return 12;
    } else if (strcmp(name, "ori") == 0) {
        return 13;
    } else if (strcmp(name, "nori") == 0) {
        return 14;
    } else if (strcmp(name, "bne") == 0) {
        return 15;
    } else if (strcmp(name, "beq") == 0) {
        return 16;
    } else if (strcmp(name, "blt") == 0) {
        return 17;
    } else if (strcmp(name, "bgt") == 0) {
        return 18;
    } else if (strcmp(name, "lb") == 0) {
        return 19;
    } else if (strcmp(name, "sb") == 0) {
        return 20;
    } else if (strcmp(name, "lw") == 0) {
        return 21;
    } else if (strcmp(name, "sw") == 0) {
        return 22;
    } else if (strcmp(name, "lh") == 0) {
        return 23;
    } else if (strcmp(name, "sh") == 0) {
        return 24;
    }

    /*j type*/
    else if (strcmp(name, "jmp") == 0) {
        return 30;
    }
    else if (strcmp(name, "la") == 0) {
        return 31;
    }
    else if (strcmp(name, "call") == 0) {
        return 32;
    }
    else if (strcmp(name, "hlt") == 0) {
        return 63;
    }

    return -1; /*invalid instruction*/
}

/*gets a command name and returns its funct value*/
/*if no funct value then returns 0*/
int get_funct(char *name) {
    if (strcmp(name, "add") == 0) {
        return 1;
    } else if (strcmp(name, "sub") == 0) {
        return 2;
    } else if (strcmp(name, "and") == 0) {
        return 3;
    } else if (strcmp(name, "or") == 0) {
        return 4;
    } else if (strcmp(name, "nor") == 0) {
        return 5;
    } else if (strcmp(name, "move") == 0) {
        return 1;
    } else if (strcmp(name, "mvhi") == 0) {
        return 2;
    } else if (strcmp(name, "mvlo") == 0) {
        return 3;
    }
    return 0; /*no value*/
}
