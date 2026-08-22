#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "second_pass.h"
#include "utils.h"
#include "data_table.h"
#include "reserved_word.h"
#include "writing_to_binary.h"
#include "get_codes.h"


/*returns 1 if successful, 0 if skipped, -1 if failed*/
int process_line_second_pass(char *cur_line, AssemblerData *data, int line_idx, int *cur_IC,
     int *cur_DC, ExternUsageNode **extern_head) {

    char word[MAX_LINE_LEN];
    int word_idx = 0;

    if (should_skip_line(cur_line) == 1) {
        return 0; /* Skip this line */
    }

    if (get_next_word(cur_line, &word_idx, word) == 0) { /* Get the first word in the line */
        return 0; /* Skip lines with no words */
    }

    if (is_label(word)) {
        get_next_word(cur_line, &word_idx, word); /* skip the label */
    }
    
    if (is_entry_directive(cur_line)) {
        /* Handle .entry directive */
        if (handle_entry_directive_second_pass(cur_line, data) == -1) {
            fprintf(stderr, "Error: Failed to handle .entry directive at line %d.\n", line_idx);
            return -1;
        }
    }

    else if (is_branch_instruction(word)) {            
        /* Handle branch instruction */
        if (handle_branch_instruction_second_pass(cur_line, &word_idx, data, *cur_IC) == -1) {
            return -1;
        }
    }

    /* Update IC and DC based on the type of instruction or directive */
    else if (is_instruction(word)) {
        *cur_IC += 4;
    }
    else if (is_data_directive(word)) {
        if (increment_DC(cur_line, &word_idx, word, cur_DC, data) == -1) {
            return -1;
        }
    }


    return 1;
}


int run_second_pass(FILE *am_file, AssemblerData *data, ExternUsageNode **extern_head) {
    char cur_line[MAX_LINE_LEN];
    int line_idx = 0;
    int cur_IC = 100;
    int cur_DC = 0;

    if (extern_head == NULL) {
        fprintf(stderr, "Error: no pointer was given for extern usage list.\n");
        return -1;
    }
    *extern_head = NULL;

    while (fgets(cur_line, sizeof(cur_line), am_file) != NULL) {
        line_idx++;
        if (process_line_second_pass(cur_line, data, line_idx, &cur_IC, &cur_DC, extern_head) == -1) {
            fprintf(stderr, "Error: Failed to process line %d in second pass.\n", line_idx);
            return -1;
        }
    }

    return 1; /* Successfully completed the second pass */
}

int handle_entry_directive_second_pass(char *line, AssemblerData *data) {
    int word_idx = 0;
    char label_name[MAX_SYMBOL_NAME_LEN];
    LabelNode *label_node;

    if (is_entry_directive(line) == 0) {
        fprintf(stderr, "Error: Not an .entry directive\n");
        data->error_flag = 1;
        return -1;
    }

    /* Get the .entry directive */
    get_next_word(line, &word_idx, label_name); /* Move to the next word after .entry */

    /* Get the label name */
    if (get_next_word(line, &word_idx, label_name) == 0) {
        fprintf(stderr, "Error: Missing label for .entry directive\n");
        data->error_flag = 1;
        return -1;
    }

    if (is_valid_label_name(label_name) == 0) {
        fprintf(stderr, "Error: Invalid label '%s' for .entry\n", label_name);
        data->error_flag = 1;
        return -1;
    }

    /* check if the label exists in the symbol table */
    label_node = find_label(data->label_head, label_name);
    if (label_node == NULL) {
        fprintf(stderr, "Error: Label '%s' not found for .entry directive\n", label_name);
        data->error_flag = 1;
        return -1;
    }
    
    /* set the label as an entry */
    label_node->is_entry = 1;

    return 1; /* Successfully handled .entry directive */
}

/*gets the value for a branch instruction, and puts them in the appropriate variables */
/*returns 1 if successful, -1 if error*/
int get_branch_value(char *line, AssemblerData *data, int cur_IC, char *label_name, int *word_idx, int *offset, char *operand1, char *operand2) {
    char name[MAX_LINE_LEN];
    LabelNode *label_node;

    if (get_next_word(line, word_idx, name) == 0) {
        fprintf(stderr, "Error: Missing instruction name\n");
        data->error_flag = 1;
        return -1;
    }

    if (is_branch_instruction(name) == 0) {
        fprintf(stderr, "Error: Not a branch instruction\n");
        data->error_flag = 1;
        return -1;
    }

    /*get the operands*/
    if (get_next_command(line, word_idx, operand1)== 0 || get_next_command(line, word_idx, operand2) == 0
    || get_next_command(line, word_idx, label_name) == 0) {
        fprintf(stderr, "Error: Missing operands for branch instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }

    if (is_valid_label_name(label_name) == 0) {
        fprintf(stderr, "Error: Invalid label '%s' for branch instruction\n", label_name);
        data->error_flag = 1;
        return -1;
    }

    label_node = find_label(data->label_head, label_name);
    if (label_node == NULL) {
        fprintf(stderr, "Error: Label '%s' not found for branch instruction\n", label_name);
        data->error_flag = 1;
        return -1;
    }
    if (label_node->type == EXTERN) {
        fprintf(stderr, "Error: Label '%s' is defined as .extern and cannot be used in branch instruction\n", label_name);
        data->error_flag = 1;
        return -1;
    }

    *offset = label_node->address - (cur_IC + 4); /* calculate the offset */
    
    if (*offset < -32768 || *offset > 32767) {
        fprintf(stderr, "Error: Offset %d for label '%s' is out of range for branch instruction\n", *offset, label_name);
        data->error_flag = 1;
        return -1;
    }
    return 1;
}

int handle_branch_instruction_second_pass(char *line, int *word_idx, AssemblerData *data, int cur_IC) {
    char operand1[MAX_LINE_LEN], operand2[MAX_LINE_LEN], label_name[MAX_LINE_LEN];
    int offset;

    if (get_branch_value(line, data, cur_IC, label_name, word_idx, &offset, operand1, operand2) == -1) { 
        /*get and check if the branch values are valid*/
        return -1;
    }

    if (encode_branch_instruction_second_pass(offset, data, cur_IC) == -1) {
        /*encode the branch instruction, if failed return -1*/
        return -1;
    }

    return 1;
}

/*gets the value for a J-type instruction and puts them in the appropriate variables*/
/*returns 1 if successful, -1 if failed*/
int get_j_type_value(char *line,  char *label_name, int *word_idx, int *op_code, char *operand, AssemblerData *data) {
    char name[MAX_LINE_LEN];

    if (get_next_word(line, word_idx, name) == 0) {
        fprintf(stderr, "Error: Missing instruction name\n");
        data->error_flag = 1;
        return -1;
    }

    if (!is_j_type_instruction(name)) {
        fprintf(stderr, "Error: '%s' is not a J-type instruction\n", name);
        data->error_flag = 1;
        return -1;
    }
    *op_code = get_opcode(name);
    if (*op_code == -1) {
        fprintf(stderr, "Error: Invalid J-type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }


    if (get_next_command(line, word_idx, operand) == 0) {
        fprintf(stderr, "Error: Missing operand for J-type instruction '%s'\n", name);
        data->error_flag = 1;
        return -1;
    }
    
    return 1; /* Successfully retrieved the label for J-type instruction */
}
int handle_j_type_instruction_second_pass(char *line, int *word_idx, int cur_IC, AssemblerData *data, ExternUsageNode **extern_head) {
    char operand[MAX_LINE_LEN];
    char name[MAX_LINE_LEN];
    int op_code;
    LabelNode *label_node;

    if (get_j_type_value(line, name, word_idx, &op_code, operand, data) == -1) {
        return -1; /* Failed to get J-type instruction values */
    }

    if (op_code == 63) { /* hlt */
        return 1;
    }

    else if (op_code == 30 && is_register(operand)) { /* jmp with register operand */
        return 1; /* No further processing needed for register operand */
    }      

    label_node = find_label(data->label_head, operand);
    if (label_node == NULL) {
        fprintf(stderr, "Error: Label '%s' not found for J-type instruction\n", operand);
        data->error_flag = 1;
        return -1;
    }   

    if (label_node->type == EXTERN) {
        if (add_ExternUsage_node(extern_head, label_node->name, label_node->address) == -1) {
            fprintf(stderr, "Error: Failed to add extern usage for label '%s'\n", label_node->name);
            data->error_flag = 1;
            return -1;
        }
        return 1; /* no more processing needed for extern label */
    }
    
    if (encode_j_type_instruction_second_pass(line, label_node, data, &cur_IC) == -1) {
        return -1;
    }

    return 1;
}