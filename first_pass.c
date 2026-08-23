#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "consts.h"
#include "utils.h"
#include "data_table.h"
#include "reserved_word.h"
#include "first_pass.h"
#include "writing_to_binary.h"
#include "macro_table.h"
    


/*returns 1 if there was a line to process, 0 if skipped, -1 if error*/
int process_line_first_pass(char *line, AssemblerData *data, int line_idx, MacroNode *macro_head) {
    int word_idx = 0;
    char word [MAX_LINE_LEN];
    

    /* Check if the line should be skipped */
    if (should_skip_line(line) == 1) {
        return 0; /* Skip this line */
    }

    get_next_word(line, &word_idx, word);/*gettign the first word in the line*/
    if (is_label(word) == 1) {
        return handle_label(line, data, line_idx, macro_head); /*returns 1 if successful, -1 if error*/
    }
    else {
        word_idx = 0; /* Reset word_idx to start reading the line again */
        if (is_instruction(word) == 1) {
            return handle_CODE(line, &word_idx, data);
        }
        else if (is_data_directive(word) == 1) {
            return handle_data_directive(line, &word_idx, data);
        }
        else if(strcmp(word, ".extern") == 0) { /*check if the word is .extern*/
            return encode_extern_directive(line, &word_idx, word, data);
        }
        else if(strcmp(word, ".entry") == 0) { /*check if the word is .entry*/
            return handle_entry_directive_first_pass(line, &word_idx, word, data);
        }
        else {
            fprintf(stderr, "Error: Unknown instruction or directive '%s' at line %d.\n", word, line_idx);
            data->error_flag = 1;
            return -1; /* Unknown instruction or directive */
        }
    }



    return 1; /* Successfully processed the line */
}


/*returns 1 if successful, -1 if there was an error*/
int run_first_pass(FILE *am_file, AssemblerData *data, MacroNode *macro_head) {
    char cur_line[MAX_LINE_LEN];
    int line_idx = 0;
    LabelNode *tmp;

    /*initilsised data*/
    data->IC = 100; /*the IC starts at address 100*/
    data->DC = 0;
    data->label_head = NULL;
    data->error_flag = 0;

    while (fgets(cur_line, sizeof(cur_line), am_file) != NULL) {
        line_idx++;
        
        if (strchr(cur_line, '\n') == NULL && !feof(am_file)) {
            fprintf(stderr, "Error: Line %d exceeds maximum allowed length (%d characters).\n", 
                    line_idx + 1, MAX_LINE_LEN);
            data->error_flag = 1;
            /* Skip the rest of the line to avoid processing it */
            flush_line(am_file);
        }
        else{  
            /* Process the current line */
            if (process_line_first_pass(cur_line, data, line_idx, macro_head) == -1) {
                fprintf(stderr, "Error processing line %d.\n", line_idx);
                data->error_flag = 1; /* Set error flag */
            }
        }
    }


    /*adding the ICF to the DC*/
    if (data->error_flag == 0){
        tmp = data->label_head;
        while (tmp != NULL) {
            if (tmp->type == DATA) {
                tmp->address += data->IC; /* Adjust data label addresses by adding IC */
            }
            tmp = tmp->next;
        }
    }

    return (data->error_flag == 1)? -1 : 1; /* Return -1 if there was an error, 1 otherwise */
    
}