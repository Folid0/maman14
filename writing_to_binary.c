#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"
#include "data_table.h"
#include "reserved_word.h"
#include "writing_to_binary.h"
#include "get_codes.h"


/*writes the line in binary to the data*/
/*word idx is on the name of the command (ussaly the first word or seccond if there is a label)*/
/*returns 1 if successful, -1 otherwise*/
int handle_CODE(char *line, int *word_idx, AssemblerData *data) {  
    
    char* name = get_word(line, word_idx); /*word idx is on the name of the command */
    if (is_r_type_instruction(name)) {
        return handle_r_type_instruction(line, word_idx, name,data);
    }
     else if (is_i_type_instruction(name)) {
        return handle_i_type_instruction(line, word_idx, data);
    }
     else if (is_j_type_instruction(name)) {
        return handle_j_type_instruction(line, word_idx, data);
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
    if (op_code == -1) {
        /*invalid instruction*/
        data->error_flag = 1;
        return -1;
    }

    get_next_word(line, word_idx, word); 
    rs = get_register_num(word);
    get_next_word(line, word_idx, word); 

    if (op_code == 1) { /* R-type instructions with 3 registers */  
        rt = get_register_num(word);
        get_next_word(line, word_idx, word); 
    }
    rd = get_register_num(word);

    if (rs == -1 || rt == -1 || rd == -1) {
        /*invalid register*/
        data->error_flag = 1;
        return -1;
    }
    


    return 1;

}

