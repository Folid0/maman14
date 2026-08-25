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
#include "macro_table.h"

/*writes the line in binary to the data*/
/*word idx is on the name of the command (ussaly the first word or seccond if there is a label)*/
/*returns 1 if successful, -1 otherwise*/
int handle_CODE(char *line, int *word_idx, AssemblerData *data) {  
    char name[MAX_LINE_LEN];
    get_next_word(line, word_idx, name); /*word idx is on the name of the command */
    if (is_r_type_instruction(name)) {
        return encode_r_type_instruction(line, word_idx, name,data);
    }
    else if (is_i_type_instruction(name)) {
        return encode_i_type_instruction(line, word_idx, name, data);
    }
     else if (is_j_type_instruction(name)) {
        return encode_j_type_instruction(line, word_idx, name, data);
    }
    
     else {
        /*not a valid instruction*/
        data->error_flag = 1;
        return -1;
    }
    return data->error_flag ? -1 : 1;
}


/*encodes the r type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int encode_r_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data) {
    int op_code = get_opcode(name);
    int funct = get_funct(name);
    int rs, rt = 0, rd;
    char word[MAX_LINE_LEN];
    unsigned long machine_code = 0; /*32 bits of the command*/
    int img_idx;

    if (op_code == -1) {
        /*invalid instruction*/
        fprintf(stdout, "Error: invalid r type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    if (get_next_command(line, word_idx, word) == 0){
        /*invalid or empty register*/
        fprintf(stdout, "Error: Missing or wrong first register for r type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
    rs = get_register_num(word);

    if (get_next_command(line, word_idx, word) == 0){
        /*invalid or empty register*/
        fprintf(stdout, "Error: Missing or wrong second register for r type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    if (op_code == 0) { /* R-type instructions with 3 registers */  
        rt = get_register_num(word);
        if (get_next_command(line, word_idx, word) == 0){
        /*invalid or empty register*/
        fprintf(stdout, "Error: Missing or wrong third register for r type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
    }
    rd = get_register_num(word);

    if (rs == -1 || rt == -1 || rd == -1) {
        /*invalid register*/
        fprintf(stdout, "Error: invalid register in r type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    /* Verify there is no extra garbage text after the operands */  
    if (get_next_word(line, word_idx, word) != 0) {
        fprintf(stdout, "Error: text after instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
    
    machine_code |= ((unsigned long)op_code << 26);
    machine_code |= (rs << 21);
    machine_code |= (rt << 16);
    machine_code |= (rd << 11);
    machine_code |= (funct << 6);

    img_idx = data->IC - 100; 

    if (!does_image_have_space(img_idx, 4)) {
        fprintf(stdout, "Error: Not enough space in code image to encode instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    data->code_image[img_idx]     = machine_code & 0xFF;         
    data->code_image[img_idx + 1] = (machine_code >> 8) & 0xFF; 
    data->code_image[img_idx + 2] = (machine_code >> 16) & 0xFF; 
    data->code_image[img_idx + 3] = (machine_code >> 24) & 0xFF; 

    data->IC += 4;

    return 1;

}

/*encodes the i type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int encode_i_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data) {
    int op_code = get_opcode(name);
    unsigned long machine_code = 0; /*32 bits of the command*/
    int img_idx;
    int rt, rs;
    char operand1[MAX_LINE_LEN], operand2[MAX_LINE_LEN], operand3[MAX_LINE_LEN], extra[MAX_LINE_LEN];
    int is_branch = 0, immed;
    char *endptr;
    long immed_tmp;


    if (op_code == -1) {
        /*invalid instruction*/
        data->error_flag = 1;
        return -1;
    }
    
    /* Set the opcode in bits 26-31 */
    machine_code |= ((unsigned long)op_code << 26);

    /* Check if its a branch instruction (opcodes 15 to 18) */
    if (op_code >= 15 && op_code <= 18) {
        is_branch = 1;
    }    
    
    if (get_next_command(line, word_idx, operand1) == 0 ||
        get_next_command(line, word_idx, operand2) == 0 ||
        get_next_command(line, word_idx, operand3) == 0) {
        
        fprintf(stdout, "Error: Missing or wrong operands for I-type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    rs = get_register_num(operand1);

    if (is_branch == 1) { 
        rt = get_register_num(operand2);

        immed = 0;

        if (rs == -1 || rt == -1) {
            fprintf(stdout, "Error: Invalid registers for branch instruction '%s'\n", name);
            data->error_flag = 1;
            return -1;
        }

        if (is_valid_label_name(operand3) == 0) {
            fprintf(stdout,
                    "Error: Invalid branch target '%s' for instruction '%s'\n", operand3, name);
            data->error_flag = 1;
            return -1;
        }
    }
    else{

        immed_tmp = strtol(operand2, &endptr, 10);

        /*allow trailing spaces and tabs*/
        while (*endptr == ' ' || *endptr == '\t') {
            endptr++;
        }

        if (endptr == operand2 || *endptr != '\0' || immed_tmp < -32768L || immed_tmp > 32767L) {
            fprintf(stdout, "Error: Invalid immediate value '%s' for instruction '%s'\n", operand2, name);
            data->error_flag = 1;
            return -1;
        }

        immed = (int)immed_tmp;
        rt = get_register_num(operand3);

        if (rs == -1 || rt == -1) {
            fprintf(stdout, "Error: Invalid registers for instruction '%s'\n", name);
            data->error_flag = 1;
            return -1;
        }
    }

    /* Verify there is no extra garbage text after the operands */
    if (get_next_word(line, word_idx, extra) != 0) {
        fprintf(stdout, "Error: text after instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    

    machine_code |= (rs << 21);
    machine_code |= (rt << 16);
    machine_code |= (immed & 0xFFFF);

    /*starting index in the code_image array*/
    img_idx = data->IC - 100;

    if (!does_image_have_space(img_idx, 4)) {
        fprintf(stdout, "Error: Not enough space in code image to encode instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    data->code_image[img_idx]     = machine_code & 0xFF;         
    data->code_image[img_idx + 1] = (machine_code >> 8) & 0xFF;  
    data->code_image[img_idx + 2] = (machine_code >> 16) & 0xFF; 
    data->code_image[img_idx + 3] = (machine_code >> 24) & 0xFF;

    /*increment instruction counter*/
    data->IC += 4;


    return 1;
    
}

/*encodes the j type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int encode_j_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data) {
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
    machine_code |= ((unsigned long)op_code << 26);

    /* op_code 63 is "hlt", which takes absolutely no operands */
    if (op_code == 63) {
        if (get_next_word(line, word_idx, extra) != 0) {
            fprintf(stdout, "Error: Extraneous text after instruction 'hlt'\n");
            data->error_flag = 1;
            return -1;
        }
    } 
    else {
        /* read the next word (the operand for jmp, la, or call) */
        if (get_next_command(line, word_idx, operand) == 0) {
            fprintf(stdout, "Error: Missing operand for J-type instruction '%s'\n", name);
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
                if (is_valid_label_name(operand) == 0) {
                    fprintf(stdout, "Error: Invalid operand '%s' for 'jmp'. Expected a valid label or register.\n", operand);
                    data->error_flag = 1;
                    return -1;
                }
                /* Bits 0-24 remain 0 for now. The second pass will encode the label address */
            }
        } 
        else { 
            /* la (31) and call (32) */
            /* These instructions take a label, not a register */
            if (is_register(operand)) {
                fprintf(stdout, "Error: Instruction '%s' strictly requires a label, not a register.\n", name);
                data->error_flag = 1;
                return -1;
            }
            
            /* Verify it starts with a letter */
            if (is_valid_label_name(operand) == 0) {
                fprintf(stdout, "Error: Invalid operand '%s' for instruction '%s'. Expected a valid label.\n", operand, name);
                data->error_flag = 1;
                return -1;
            }
            /* Bits 0-24 remain 0 for now. The second pass will encode the label address */
        }

        /* Verify there is no extra garbage text after the operand */
        if (get_next_word(line, word_idx, extra) != 0) {
            fprintf(stdout, "Error: text after instruction '%s'\n", name);
            data->error_flag = 1;
            return -1;
        }
    }

    /* Calculate the starting index in the code_image array */
    img_idx = data->IC - 100;

    if (!does_image_have_space(img_idx, 4)) {
        fprintf(stdout, "Error: Not enough space in code image to encode instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
    /* Write the 32-bit machine code into memory (Little-Endian format) */
    data->code_image[img_idx]     = machine_code & 0xFF;         /* Bits 0-7 */
    data->code_image[img_idx + 1] = (machine_code >> 8) & 0xFF;  /* Bits 8-15 */
    data->code_image[img_idx + 2] = (machine_code >> 16) & 0xFF; /* Bits 16-23 */
    data->code_image[img_idx + 3] = (machine_code >> 24) & 0xFF; /* Bits 24-31 */

    /* increment Instruction Counter */
    data->IC += 4;

    return 1;
}


/*encodes line with data directive to binary into data*/
/**/
int handle_data_directive(char *line, int *word_idx, AssemblerData *data) {
    char name[MAX_LINE_LEN];
    get_next_word(line, word_idx, name); /*word idx is on the name of the command */

    if (strcmp(name, ".asciz") == 0) {
        return encode_asciz_directive(line, word_idx, name, data);
    } 
    else if (strcmp(name, ".db") == 0 || strcmp(name, ".dh") == 0 || strcmp(name, ".dw") == 0) {
        return encode_db_dw_db_directive(line, word_idx, name, data);
    } 
    else {
        fprintf(stdout, "Error: Unknown data directive '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
}


/*encodes .asciz directive to binary*/
/*returns 1 if successful, -1 if error*/
int encode_asciz_directive(char *line, int *word_idx,char *name, AssemblerData *data) {
    int end_quote = 0;
    char trash[MAX_LINE_LEN];
    *word_idx = skip_whitespace(line, *word_idx);
    
    if (line[*word_idx] != '"') {
        fprintf(stdout, "Error: .asciz string must start with a quote\n");
        data->error_flag = 1; 
        return -1;
    }
    (*word_idx)++; /* Skip the starting quote */
    
    /* Read characters until closing quote or end of line */
    while(line[*word_idx] != '\0' && line[*word_idx] != '\n' && line[*word_idx] != '"') {
        if ((unsigned char)line[*word_idx] < 32 || (unsigned char)line[*word_idx] > 126) { /*check for non ASCII characters*/
            fprintf(stdout, "Error: .asciz contains a non-printable ASCII character\n");
            data->error_flag = 1;
            return -1;
        }
        if (!does_image_have_space(data->DC, 1)) {
            fprintf(stdout, "Error: Not enough space in data image to encode .asciz string\n");
            data->error_flag = 1;
            return -1;
        }

        data->data_image[data->DC++] = line[*word_idx];
        (*word_idx)++;
    }
    
    if (line[*word_idx] == '"') {
        end_quote = 1;
        (*word_idx)++;
    }

    if (end_quote == 0) {
        fprintf(stdout, "Error: .asciz string missing closing quote\n");
        data->error_flag = 1; 
        return -1;
    }
    

    if (!does_image_have_space(data->DC, 1)) {
        fprintf(stdout, "Error: Not enough space in data image\n");
        data->error_flag = 1;
        return -1;
    }
    /* add null terminator */
    data->data_image[data->DC++] = '\0';
    
    trash[0] = '\0'; /* Clear the trash buffer */
    /* Check for trailing garbage text */
    if (get_next_word(line, word_idx, trash) != 0) {
        fprintf(stdout, "Error: text after instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    return 1;
}


/*encodes .db, .dh, and .dw directives to binary intot the data image */
/*returns 1 if successful, -1 if error*/
int encode_db_dw_db_directive(char *line, int *word_idx, char *name, AssemblerData *data) {
    int size = 0; /* Size in bytes for .db, .dh, .dw */
    long val;
    unsigned long encoded_val; /*for bit shifting*/
    char *endptr;
    int state = 0; /* State: 0 expects a number, 1 expects a comma */
    int i;
    long min_value;
    long max_value;

    size = (strcmp(name, ".db") == 0) ? 1 : ((strcmp(name, ".dh") == 0) ? 2 : (strcmp(name, ".dw") == 0) ? 4 : 0);
    if (size == 0) {
        fprintf(stdout, "Error: Unknown directive '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
    else if (size == 1) {
        min_value = -128L;
        max_value = 127L;
    } else if (size == 2) {
        min_value = -32768L;
        max_value = 32767L;
    } else { /* size == 4 */
        min_value = -2147483647L - 1L; /* To avoid overflow */
        max_value = 2147483647L;
    }

    while (line[*word_idx] != '\0' && line[*word_idx] != '\n') {
        *word_idx = skip_whitespace(line, *word_idx);
        if (line[*word_idx] != '\0' && line[*word_idx] != '\n'){
        
            if (state == 0) {
                if (line[*word_idx] == ',') {
                    fprintf(stdout, "Error: unexpected comma before number\n");
                    data->error_flag = 1; 
                    return -1;
                }
                
                /* Parse the number */
                val = strtol(&line[*word_idx], &endptr, 10);

                if (endptr == &line[*word_idx]) {
                    fprintf(stdout, "Error: expected an integer\n");
                    data->error_flag = 1; 
                    return -1;
                }
                
                if (val < min_value || val > max_value) {
                    fprintf(stdout, "Error: value %ld out of range for directive '%s'\n", val, name);
                    data->error_flag = 1; 
                    return -1;
                }
                
                if (!does_image_have_space(data->DC, size)) {
                    fprintf(stdout, "Error: Not enough space in data image\n");
                    data->error_flag = 1;
                    return -1;
                }

                encoded_val = (unsigned long)val;

                /* Write to data_image*/
                for (i = 0; i < size; i++) {
                    data->data_image[data->DC++] = (unsigned char)((encoded_val >> (i * 8)) & 0xFFUL);
                }
                
                *word_idx = endptr - line;
                state = 1; /* Next, we expect a comma */
            }
            else {
                if (line[*word_idx] == ',') {
                    state = 0; /* Comma found, next we expect a number */
                    (*word_idx)++;
                } else {
                    fprintf(stdout, "Error: missing comma between numbers\n");
                    data->error_flag = 1; 
                    return -1;
                }
            }
    }
    }

    if (state == 0) {
        fprintf(stdout, "Error: trailing comma without a number\n");
        data->error_flag = 1; 
        return -1;
    }
    return 1;
}

/*encodes the extern data to the symbol table*/
/*returns 1 if successful, -1 on failure*/
int encode_extern_directive(char *line, int *word_idx, char *name, AssemblerData *data) {
    char label_name[MAX_LINE_LEN];
    char trash[MAX_LINE_LEN];
    LabelNode *tmp;
    int skip = 0;
    int error_value; /*used to store the return value of functions that may have MEMORY ALLOCATION ERROR*/
    
    if(get_next_word(line, word_idx, name) == 0 && strcmp(name, ".extern") != 0) { /*getting the idx of the first word, which is the command name*/
        fprintf(stdout, "Error: Missing command for .extern directive\n");
        data->error_flag = 1;
        return -1;
    }
    /*get label name*/
    if (get_next_word(line, word_idx, label_name) == 0) {
        fprintf(stdout, "Error: Missing label for .extern directive\n");
        data->error_flag = 1;
        return -1;
    }

    if (is_valid_label_name(label_name) == 0) {
        fprintf(stdout, "Error: Invalid label '%s' for .extern\n", label_name);
        data->error_flag = 1;
        return -1;
    }

    /* Check for trailing garbage text */
   if (get_next_word(line, word_idx, trash) != 0) {
        fprintf(stdout, "Error: text after .extern directive\n");
        data->error_flag = 1;
        return -1;
    }

    tmp = data->label_head;
    while (tmp != NULL) { /*checking if the label was defined twice and not matching extern type */
        if (strcmp(tmp->name, label_name) == 0){
            skip = 1;
            if (tmp->type != EXTERN) {
                data->error_flag = 1;
                fprintf(stdout, "Error: double definition of label directive and not both extern type\n");
                return -1;
            }
        }
        tmp = tmp->next;
    }
    
    if (skip == 0) {
        /* Add the label to the symbol table with EXTERN type */
        error_value = add_label(&data->label_head, label_name, 0, EXTERN);
        if (error_value == MEMORY_ALLOCATION_ERROR) {
            fprintf(stdout, "Memory allocation error while adding label '%s' to symbol table\n",label_name);
            data->error_flag = 1;
            return MEMORY_ALLOCATION_ERROR;
        }
        if (error_value == -1) {
            fprintf(stdout, "Error: Failed to add label '%s' to symbol table\n", label_name);
            data->error_flag = 1;
            return -1;
        }
    }

    return 1;
}




/* Handles the .entry directive during the first pass */
int handle_entry_directive_first_pass(char *line, int *word_idx, char *name, AssemblerData *data) {
    char label_name[MAX_LINE_LEN];
    char extra[MAX_LINE_LEN];
    LabelNode *tmp_label;

    if(get_next_word(line, word_idx, name) == 0 && strcmp(name, ".entry") != 0) { /*getting the idx of the first word, which is the command name*/
        fprintf(stdout, "Error: Missing command for .entry directive\n");
        data->error_flag = 1;
        return -1;
    }

    /*get label name*/
    if (get_next_word(line, word_idx, label_name) == 0) {
        fprintf(stdout, "Error: Missing label for .entry directive\n");
        data->error_flag = 1;
        return -1;
    }

    /* Verify label syntax */
    if (is_valid_label_name(label_name) == 0) {
        fprintf(stdout, "Error: Invalid label '%s' for .entry\n", label_name);
        data->error_flag = 1;
        return -1;
    }

    tmp_label = find_label(data->label_head, label_name); /*make sure the label dosnt already exist as extern*/
    if (tmp_label != NULL) {
        if (tmp_label->type == EXTERN) {
            fprintf(stdout, "Error: Label '%s' defined as .extern cannot be marked as .entry\n", label_name);
            data->error_flag = 1;
            return -1;
        }
    } 


    /* Check for trailing garbage text */
    if (get_next_word(line, word_idx, extra) != 0) {
        fprintf(stdout, "Error: text after .entry directive\n");
        data->error_flag = 1;
        return -1;
    }

    return 1;
}


/*return 1 if the label was handled successfully, -1 otherwise*/
int handle_label(char *line, AssemblerData *data, int line_idx, MacroNode *macro_head) {
    char label_name[MAX_SYMBOL_NAME_LEN];
    /* char *word[MAX_LINE_LEN]; */
    int word_idx = 0;
    int command_idx = 0;
    LabelType type;
    char command_name[MAX_LINE_LEN];
    int error_value; /*used to store the return value of functions that may have MEMORY ALLOCATION ERROR*/
    label_name[0] = '\0'; /* Initialize label_name to an empty string */

    /* setting label name */
    if (get_label_name(line, &word_idx, label_name) == -1) {
        fprintf(stdout, "Error: Invalid label name at line %d.\n", line_idx);
        return -1;
    }
    

    if (find_macro(macro_head, label_name) != NULL) {
        fprintf(stdout, "Error: Label '%s' is already defined as a macro at line %d.\n", label_name, line_idx);
        return -1;
    }
    command_idx = word_idx; /*the index after the label name*/
    get_next_word(line, &word_idx, command_name); /* Move to the next word after the label */
    type = get_label_type(command_name); /*setting type*/

    if (find_label(data->label_head, label_name) != NULL) { /* Check if the label already exists in the symbol table */
        fprintf(stdout, "Error: Label '%s' is already defined at line %d.\n", label_name, line_idx);
        return -1;
    }

    switch (type) {
        case CODE:
            error_value = add_label(&data->label_head, label_name, data->IC, CODE);
            if (error_value != 1) {
                fprintf(stdout, "Error: Failed to handle code directive for label '%s' at line %d.\n", label_name, line_idx);
                return error_value;
            }

            error_value = handle_CODE(line, &command_idx, data);
            if (error_value != 1) {
                fprintf(stdout, "Error: Failed to handle code directive for label '%s' at line %d.\n", label_name, line_idx);
                return error_value;
            }
            break;

        case DATA:
            error_value = add_label(&data->label_head, label_name, data->DC, DATA);
            if (error_value != 1) {
                fprintf(stdout, "Error: Failed to handle data directive for label '%s' at line %d.\n", label_name, line_idx);
                return error_value;
            }

            error_value = handle_data_directive(line, &command_idx, data);
            if (error_value != 1) {
                fprintf(stdout, "Error: Failed to handle data directive for label '%s' at line %d.\n", label_name, line_idx);
                return error_value;
            }
            break;

        case EXTERN:
            /*igonore the label as there is no meaning*/
            error_value = encode_extern_directive(line, &command_idx, command_name, data);
            if (error_value != 1) {
                fprintf(stdout, "Error: Failed to handle extern directive for label '%s' at line %d.\n", label_name, line_idx);
                return error_value;
            }
            break;

        case ENTRY:
            /*ignore the label as there is no meaning*/
            error_value = handle_entry_directive_first_pass(line, &command_idx, command_name, data);
            if (error_value != 1) {
                fprintf(stdout, "Error: Failed to handle entry directive for label '%s' at line %d.\n", label_name, line_idx);
                return error_value;
            }
            break;

        default:
            fprintf(stdout, "Error: Unknown label type for '%s' at line %d.\n", label_name, line_idx);
            return -1; /* Unknown label type */
    }
    return 1; /* Success */
}


/*encodes a branch instruction in the second pass to the code image*/
/*returns 1 if successful, -1 if error*/
int encode_branch_instruction_second_pass(int offset, AssemblerData *data, int cur_IC) {
    int code_idx = cur_IC - 100;
    unsigned long machine_code;

    if (code_idx < 0 || code_idx + 3 >= MAX_MEM_SIZE) { /*check if the code index is valid*/
        fprintf(stdout, "Error: Invalid code index %d for branch instruction at IC %d\n", code_idx, cur_IC);
        data->error_flag = 1;
        return -1;
    }

    machine_code =
      ((unsigned long)data->code_image[code_idx])
    | ((unsigned long)data->code_image[code_idx + 1] << 8)
    | ((unsigned long)data->code_image[code_idx + 2] << 16)
    | ((unsigned long)data->code_image[code_idx + 3] << 24);

    machine_code &= 0xFFFF0000UL;
    machine_code |= ((unsigned long)offset & 0xFFFFUL);
    data->code_image[code_idx] = machine_code & 0xFF;
    data->code_image[code_idx + 1] = (machine_code >> 8) & 0xFF;
    data->code_image[code_idx + 2] = (machine_code >> 16) & 0xFF;
    data->code_image[code_idx + 3] = (machine_code >> 24) & 0xFF;

    return 1;
}

/*encodes a J-type instruction in the second pass to the code image*/
/*returns 1 if successful, -1 if error*/
int encode_j_type_instruction_second_pass(char *line, LabelNode *label_node, AssemblerData *data, int *cur_IC) {
    unsigned long machine_code = 0;
    int code_idx = *cur_IC - 100;
    
    if (code_idx < 0 || code_idx + 3 >= MAX_MEM_SIZE) { /*check if the code index is valid*/
        fprintf(stdout, "Error: Invalid code index %d for J-type instruction at IC %d\n", code_idx, *cur_IC);
        data->error_flag = 1;
        return -1;
    }

    machine_code =
        ((unsigned long)data->code_image[code_idx])
    | ((unsigned long)data->code_image[code_idx + 1] << 8)
    | ((unsigned long)data->code_image[code_idx + 2] << 16)
    | ((unsigned long)data->code_image[code_idx + 3] << 24);

    /* Clear bits 0-24 and bit 25 */
    machine_code &= 0xFC000000UL;

    machine_code |= ((unsigned long)label_node->address & 0x01FFFFFFUL);

    data->code_image[code_idx] = machine_code & 0xFF;
    data->code_image[code_idx + 1] = (machine_code >> 8) & 0xFF;
    data->code_image[code_idx + 2] = (machine_code >> 16) & 0xFF;
    data->code_image[code_idx + 3] = (machine_code >> 24) & 0xFF;
    
    return 1;
}

