#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*returns 1 if the word is an instruction, else returns 0*/
int is_instruction(const char *word) {
    int i;
    const char * const instructions[] = {
        "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo",
        "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt",
        "lb", "sb", "lw", "sw", "lh", "sh",
        "jmp", "la", "call", "hlt"
    };

    int num_instructions = sizeof(instructions) / sizeof(instructions[0]);

    /*check if the word is one of the instructions */
    for (i = 0; i < num_instructions; i++){
        if (strcmp(instructions[i], word) == 0){
            return 1;
        }
    }
    return 0;
}

/*returns 1 if the word is a directive, else returns 0*/
int is_directive(const char *word) {
    int i;
    const char * const directives[] = {
        ".db", ".dh", ".dw", ".asciz", ".entry", ".extern"
    };

    int num_directives = sizeof(directives) / sizeof(directives[0]);
    
    for (i = 0; i <num_directives; i++){
        if (strcmp(directives[i], word) == 0){
            return 1;
        }
    }
    return 0;
}

/*returns 1 if the word is a directive without dots, else returns 0*/
int is_directive_without_dots(const char *word) {
    int i;
    const char * const directives[] = {
        "db", "dh", "dw", "asciz", "entry", "extern"
    };

    int num_directives = sizeof(directives) / sizeof(directives[0]);
    
    for (i = 0; i <num_directives; i++){
        if (strcmp(directives[i], word) == 0){
            return 1;
        }
    }
    return 0;
}

/*returns 1 if the word is a data directive, else returns 0*/
/*".db", ".dh", ".dw", ".asciz"*/
int is_data_directive(const char *word) {
    int i;
    int num_data_directives;
    const char * const data_directives[] = {
        ".db", ".dh", ".dw", ".asciz"
    };
    num_data_directives = sizeof(data_directives) / sizeof(data_directives[0]);

    for (i = 0; i < num_data_directives; i++) {
        if (strcmp(data_directives[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

/*returns 1 if the word is a register, else returns 0*/
int is_register(const char *word) {
    int i;
    const char * const registers[] = {
        "$0",  "$1",  "$2",  "$3",  "$4",  "$5",  "$6",  "$7",
        "$8",  "$9",  "$10", "$11", "$12", "$13", "$14", "$15",
        "$16", "$17", "$18", "$19", "$20", "$21", "$22", "$23",
        "$24", "$25", "$26", "$27", "$28", "$29", "$30", "$31"
    };
    int num_registers = sizeof(registers) / sizeof(registers[0]);

    /*check if the word is one of the registers */
    for (i = 0; i <num_registers; i++){
        if (strcmp(registers[i], word) == 0){
            return 1;
        }
    }
    return 0;
}

/*returns 1 if the word is a macro keyword, else returns 0*/
int is_macro_keyword(const char *word) {
    int i;
    const char * const keywords[] = {
        "mcro", "mcroend"
    };
    int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

    /*check if the word is one of the macro keywords */
    for (i = 0; i < num_keywords; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


/*return 1 if is a reserved word, else returns 0*/
int is_reserved_word(const char *word){
    if (is_instruction(word) || is_directive(word) || is_register(word) || is_macro_keyword(word) || is_directive_without_dots(word)){
        return 1;
    }
    return 0;
}

/*returns 1 if the word is an R-type instruction, 0 otherwise*/
int is_r_type_instruction(const char *word) {
    const char * const r_type_instructions[] = {

    "add", "sub", "and", "or", "nor", "move", "mvhi", "mvlo"
    };
    int num_r_type_instructions = sizeof(r_type_instructions) / sizeof(r_type_instructions[0]);
    int i = 0;

    /*checks if the word is one of the R-type instructions*/
    for (i = 0; i < num_r_type_instructions; i++) {
        if (strcmp(r_type_instructions[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

/*returns 1 if the word is an I-type instruction, 0 otherwise*/
int is_i_type_instruction(const char *word) {
    const char * const i_type_instructions[] = {
    "addi", "subi", "andi", "ori", "nori", "bne", "beq", "blt", "bgt", "lb", "sb", "lw", "sw", "lh", "sh"    
    };
    /*checks if the word is one of the I-type instructions*/
    int num_i_type_instructions = sizeof(i_type_instructions) / sizeof(i_type_instructions[0]);

    int i = 0;
    for (i = 0; i < num_i_type_instructions; i++) {
        if (strcmp(i_type_instructions[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

/*returns 1 if the word is a J-type instruction, 0 otherwise*/
int is_j_type_instruction(const char *word) {
    const char * const j_type_instructions[] = {
    "jmp", "la", "call", "hlt"
    };
    int num_j_type_instructions = sizeof(j_type_instructions) / sizeof(j_type_instructions[0]);

    int i = 0;
    /*checks if the word is one of the J-type instructions*/
    for (i = 0; i < num_j_type_instructions; i++) {
        if (strcmp(j_type_instructions[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}


int is_branch_instruction(const char *word) {
    const char * const branch_instructions[] = {
        "bne", "beq", "blt", "bgt"
    };
    int num_branch_instructions = sizeof(branch_instructions) / sizeof(branch_instructions[0]);

    int i;
    for (i = 0; i < num_branch_instructions; i++) {
        if (strcmp(branch_instructions[i], word) == 0) {
            return 1;
        }
    }
    return 0;
}

