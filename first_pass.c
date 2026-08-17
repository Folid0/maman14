#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "consts.h"
#include "utils.h"
#include "data_table.h"
#include "reserved_word.h"
#include "first_pass.h"

int handle_label(char *line, AssemblerData *data, int line_idx) {
    char label_name[Max_SYMBOL_NAME_LEN];
    /* char *word[MAX_LINE_LEN]; */
    int word_idx = 0;
    LabelType type;

    label_name[0] = '\0'; /* Initialize label_name to an empty string */

    /* setting label name */
    if (get_next_word(line, &word_idx, label_name) == 0) {
        fprintf(stderr, "Error: Failed to extract label name at line %d.\n", line_idx);
        return -1;
    }

    get_next_word(line, &word_idx, label_name); /* Move to the next word after the label */
    type = get_label_type(label_name); /*setting type*/


    switch (type) {
        case CODE:
            /* Handle code label */
            break;
        case DATA:
            /* Handle data label */
            break;
        case ENTRY:
            /* Handle entry label */
            break;
        case EXTERN:
            /* Handle extern label */
            break;
        default:
            fprintf(stderr, "Error: Unknown label type for '%s' at line %d.\n", label_name, line_idx);
            return -1; /* Unknown label type */
    }

    return 0; /* Successfully handled the label */
}

/*gets the second word from the line declaring a label and returns its type*/
LabelType get_label_type(const char *word) {


    if (is_instruction(word)) {
        return CODE;
    } else if (is_directive(word)) {
        return DATA;
    } else if (is_reserved_word(word)) {
        return ENTRY;
    } else if (is_register(word)) {
        return EXTERN;
    } else {
        return -1; /* Invalid label type */
    }
}

/*returns 1 if the word is a label, 0 otherwise*/
int is_label(const char *word) {
    int len = strlen(word);
    int i;
    if (len == 0 || len > Max_SYMBOL_NAME_LEN) {
        return 0; /* Not a label */
    }
    if (!isalpha(word[0])) {
        return 0; /* Must start with a letter */
    }
    for (i = 1; i < len-1; i++) {
        if (!isalnum(word[i])) {
            return 0; /* Must be alphanumeric*/
        }
    }
    if (word[len - 1] != ':') {
        return 0; /* Must end with : */
    }

    return 1; /* Valid label */
}

/*returns 1 if the line should be skipped, 0 otherwise*/
int should_skip_line(char *line) {
    int word_idx = 0;
    char word[MAX_LINE_LEN];

    if (get_next_word(line, &word_idx, word) == 0) {
        /* Empty line, should skip */
        return 1;
    } else if (strcmp(word, ";") == 0) {
        /* Comment line, should skip */
        return 1;
    }

    return 0; /* Line should not be skipped */
}

int process_line_first_pass(char *line, AssemblerData *data, int line_idx) {
    int word_idx = 0;
    char word [MAX_LINE_LEN];
    

    /* Check if the line should be skipped */
    if (should_skip_line(line)) {
        return 0; /* Skip this line */
    }

    get_next_word(line, &word_idx, word);
    if (is_label(word) == 1) {
        /* Handle label */
        /* Add label to symbol table with current IC or DC */
        /* Update word_idx to point to the next word after the label */


    }



    return 2;
}


/*returns 1 if successful, -1 if there was an error*/
int run_first_pass(FILE *am_file, AssemblerData *data) {
    char cur_line[MAX_LINE_LEN];
    int line_idx = 0;

    /*initilsised data*/
    data->IC = 100; /*the IC starts at address 100*/
    data->DC = 0;
    data->label_head = NULL;
    data->error_flag = 0;

    while (fgets(cur_line, sizeof(cur_line), am_file) != NULL) {
        /* Process the current line */
        if (process_line_first_pass(cur_line, data, line_idx) == -1) {
            fprintf(stderr, "Error processing line %d.\n", line_idx);
            data->error_flag = 1; /* Set error flag */
        }


        line_idx++;
    }

    if (data->error_flag == 0){


    }

    return (data->error_flag == 1)? -1 : 1; /* Return -1 if there was an error, 1 otherwise */
    
}