#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "consts.h"
#include "utils.h"
#include "data_table.h"
#include "reserved_word.h"
#include "writing_to_binary.h"
#include "get_codes.h"
#include "writing_to_binary.h"

/*writes the line in binary to the data*/
/*word idx is on the name of the command (ussaly the first word or seccond if there is a label)*/
/*returns 1 if successful, -1 otherwise*/
int handle_CODE(char *line, int *word_idx, AssemblerData *data) {  
    char name[MAX_LINE_LEN];
    get_next_word(line, word_idx, name); /*word idx is on the name of the command */
    if (is_r_type_instruction(name)) {
        return handle_r_type_instruction(line, word_idx, name,data);
    }
    else if (is_i_type_instruction(name)) {
        return handle_i_type_instruction(line, word_idx, name, data);
    }
     else if (is_j_type_instruction(name)) {
        return handle_j_type_instruction(line, word_idx, name, data);
    }
    
     else {
        /*not a valid instruction*/
        data->error_flag = 1;
        return -1;
    }
    return data->error_flag ? -1 : 1;
}



/*writes the r type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int handle_r_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data) {
    int op_code = get_opcode(name);
    int funct = get_funct(name);
    int rs, rt = 0, rd;
    char word[MAX_LINE_LEN];
    unsigned long machine_code = 0; /*32 bits of the command*/
    int img_idx;

    if (op_code == -1) {
        /*invalid instruction*/
        data->error_flag = 1;
        return -1;
    }

    if (get_next_command(line, word_idx, word) == 0){
        /*invalid or empty register*/
        data->error_flag = 1;
        return -1;
    }
    rs = get_register_num(word);

    if (get_next_command(line, word_idx, word) == 0){
        /*invalid or empty register*/
        data->error_flag = 1;
        return -1;
    }

    if (op_code == 0) { /* R-type instructions with 3 registers */  
        rt = get_register_num(word);
        if (get_next_command(line, word_idx, word) == 0){
        /*invalid or empty register*/
        data->error_flag = 1;
        return -1;
    }
    }
    rd = get_register_num(word);

    if (rs == -1 || rt == -1 || rd == -1) {
        /*invalid register*/
        data->error_flag = 1;
        return -1;
    }

    /* Verify there is no extra garbage text after the operands */  
    if (get_next_word(line, word_idx, word) != 0) {
        fprintf(stderr, "Error: text after instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
    
    machine_code |= (op_code << 26);
    machine_code |= (rs << 21);
    machine_code |= (rt << 16);
    machine_code |= (rd << 11);
    machine_code |= (funct << 6);

    img_idx = data->IC - 100; 

    data->code_image[img_idx]     = machine_code & 0xFF;         
    data->code_image[img_idx + 1] = (machine_code >> 8) & 0xFF; 
    data->code_image[img_idx + 2] = (machine_code >> 16) & 0xFF; 
    data->code_image[img_idx + 3] = (machine_code >> 24) & 0xFF; 

    data->IC += 4;

    return 1;

}

/*writes the i type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int handle_i_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data) {
    int op_code = get_opcode(name);
    char word[MAX_LINE_LEN];
    unsigned long machine_code = 0; /*32 bits of the command*/
    int img_idx;
    int rt, rs, rd;
    char operand1[MAX_LINE_LEN], operand2[MAX_LINE_LEN], operand3[MAX_LINE_LEN], extra[MAX_LINE_LEN];
    int is_branch, immed;
    
    if (op_code == -1) {
        /*invalid instruction*/
        data->error_flag = 1;
        return -1;
    }

    if (get_next_command(line, word_idx, word) == 0){
        /*invalid or empty label*/
        data->error_flag = 1;
        return -1;
    }
    
    /* Set the opcode in bits 26-31 */
    machine_code |= (op_code << 26);

    /* Check if its a branch instruction (opcodes 15 to 18) */
    if (op_code >= 15 && op_code <= 18) {
        is_branch = 1;
    }    
    
    if (get_next_command(line, word_idx, operand1) == 0 ||
        get_next_command(line, word_idx, operand2) == 0 ||
        get_next_command(line, word_idx, operand3) == 0) {
        
        fprintf(stderr, "Error: Missing or wrong operands for I-type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    rs = get_register_num(operand1);

    if (is_branch == 1) { 
        rt = get_register_num(operand2);

        immed = 0;

        if (rs == -1 || rt == -1) {
            fprintf(stderr, "Error: Invalid registers for branch instruction '%s'\n", name);
            data->error_flag = 1;
            return -1;
        }
    }
    else{
        immed = atol(operand2);  /*string to integer*/
        rt = get_register_num(operand3);

        if (rs == -1 || rt == -1) {
            fprintf(stderr, "Error: Invalid registers for instruction '%s'\n", name);
            data->error_flag = 1;
            return -1;
        }
    }

    /* Verify there is no extra garbage text after the operands */
    if (get_next_word(line, word_idx, extra) != 0) {
        fprintf(stderr, "Error: text after instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    machine_code |= (rs << 21);
    machine_code |= (rt << 16);
    machine_code |= (immed & 0xFFFF);

    /*starting index in the code_image array*/
    img_idx = data->IC - 100;

    data->code_image[img_idx]     = machine_code & 0xFF;         
    data->code_image[img_idx + 1] = (machine_code >> 8) & 0xFF;  
    data->code_image[img_idx + 2] = (machine_code >> 16) & 0xFF; 
    data->code_image[img_idx + 3] = (machine_code >> 24) & 0xFF;

    /*increment instruction counter*/
    data->IC += 4;


    return 1;
    
}

/*writes the j type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int handle_j_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data) {
    int op_code = get_opcode(name);
    unsigned long machine_code = 0;
    char operand[MAX_LINE_LEN], extra[MAX_LINE_LEN];
    int img_idx;

    if (op_code == -1) {
        /* Invalid instruction */
        data->error_flag = 1;
        return -1;
    }

    /* Set the opcode in bits 26-31 */
    machine_code |= (op_code << 26);

    /* op_code 63 is "hlt", which takes absolutely no operands */
    if (op_code == 63) {
        /* check that there is no extra text*/
        if (get_next_word(line, word_idx, extra) != 0) {
            fprintf(stderr, "Error: Extraneous text after instruction 'hlt'\n");
            data->error_flag = 1;
            return -1;
        }
    } 
    else {
        /* read the next word (the operand for jmp, la, or call) */
        if (get_next_command(line, word_idx, operand) == 0) {
            fprintf(stderr, "Error: Missing operand for J-type instruction '%s'\n", name);
            data->error_flag = 1;
            return -1;
        }

        if (op_code == 30) { /* jmp */
            if (is_register(operand)) {
                /* Operand is a register: Set the 'reg' indicator bit (bit 25) to 1 */
                machine_code |= (1 << 25);
                machine_code |= get_register_num(operand);
            } 
            else {
                /* Operand is a label: Verify it does not start with a number */
                if (!isalpha(operand[0])) {
                    fprintf(stderr, "Error: Invalid operand '%s' for 'jmp'. Expected a valid label or register.\n", operand);
                    data->error_flag = 1;
                    return -1;
                }
                /* Bits 0-24 remain 0 for now. The second pass will encode the label address. */
            }
        } 
        else { 
            /* la (31) and call (32) */
            /* These instructions take a label, not a register */
            if (is_register(operand)) {
                fprintf(stderr, "Error: Instruction '%s' strictly requires a label, not a register.\n", name);
                data->error_flag = 1;
                return -1;
            }
            
            /* Verify it starts with a letter */
            if (!isalpha(operand[0])) {
                fprintf(stderr, "Error: Invalid operand '%s' for '%s'. Expected a valid label.\n", operand, name);
                data->error_flag = 1;
                return -1;
            }
            /* Bits 0-24 remain 0 for now. The second pass will encode the label address. */
        }

        /* Verify there is no extra garbage text after the operand */
        if (get_next_word(line, word_idx, extra) != 0) {
            fprintf(stderr, "Error: text after instruction '%s'\n", name);
            data->error_flag = 1;
            return -1;
        }
    }

    /* Calculate the starting index in the code_image array */
    img_idx = data->IC - 100;

    /* Write the 32-bit machine code into memory (Little-Endian format) */
    data->code_image[img_idx]     = machine_code & 0xFF;         /* Bits 0-7 */
    data->code_image[img_idx + 1] = (machine_code >> 8) & 0xFF;  /* Bits 8-15 */
    data->code_image[img_idx + 2] = (machine_code >> 16) & 0xFF; /* Bits 16-23 */
    data->code_image[img_idx + 3] = (machine_code >> 24) & 0xFF; /* Bits 24-31 */

    /* Increment Instruction Counter */
    data->IC += 4;

    return 1;
}
