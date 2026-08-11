#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "macro_table.h"
#include "utils.h"

/* Add a new macro to the list */
/*returns -1 if an error occurs, 0 if successful*/
int add_macro(MacroNode **head, char *name) {
    MacroNode *new_node = (MacroNode *)malloc(sizeof(MacroNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for new macro node.\n");
        return -1; /*memory allocation failed*/
    }
    /* Initialize the new macro node */
    strncpy(new_node->name, name, Max_MACRO_NAME_LEN);
    new_node->name[Max_MACRO_NAME_LEN - 1] = '\0'; /* Ensure correct termination */
    new_node->lines_head = NULL;
    new_node->lines_tail = NULL;
    new_node->next = NULL;

    /* Add to the head of the list */
    if (*head == NULL) {
        *head = new_node;
    } else {
        new_node->next = *head;
        *head = new_node;
    }


    return 0; 
}

/* Add a line to a macro */
int add_line_to_macro(MacroNode *macro, char *line) {
    LineNode *new_node;
    if (macro == NULL) {
        fprintf(stderr, "Macro node is NULL.\n");
        return -1; /*macro node is NULL*/
    }

    new_node = (LineNode *)malloc(sizeof(LineNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for new line node.\n");
        return -1; /*memory allocation failed*/
    }
    strncpy(new_node->line, line, MAX_LINE_LEN);
    new_node->line[MAX_LINE_LEN - 1] = '\0'; /* Ensure correct termination */
    new_node->next = NULL;

    /* Add the new line node to the end of the lines list */
    if (macro->lines_head == NULL) {
        macro->lines_head = new_node;
        macro->lines_tail = new_node;
    } else {
        macro->lines_tail->next = new_node;
        macro->lines_tail = new_node;
    }

    return 0; 
}

/* Find a macro by its name */
/*returns a pointer to the macro if found, NULL otherwise*/
MacroNode *find_macro(MacroNode *head, char *name) {
    MacroNode *cur = head;
    while (cur != NULL) {
        if (strcmp(cur->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL; /* Macro not found */
}

/* Free the entire macro table includig the lines */
void free_macro_table(MacroNode *head){
    MacroNode *cur = head;
    LineNode *line_cur;
    MacroNode *tmp;
    LineNode *line_tmp;
    while (cur != NULL){
        tmp = cur;
        line_cur = tmp->lines_head;
        cur = cur->next;
        /* Free the lines of the macro */
        while (line_cur != NULL){
            line_tmp = line_cur;
            line_cur = line_cur->next;
            free(line_tmp);
        }
        free(tmp);
    }
}

/*add all the macro lines to the macro*/
/*returns -1 if an error occurs, probably memory allocation error*/
int add_lineblock_to_macro(MacroNode *node, char *cur_line, FILE *input_file_as,int *line_idx){
    char word[MAX_LINE_LEN];
    int word_idx = 0;

    int flag = 0; /*0 for loop to run and 1 to break*/
    while (fgets(cur_line, MAX_LINE_LEN, input_file_as)!= NULL && flag == 0){
        word_idx = 0; /*checking the first word of the line*/
        get_next_word(cur_line, &word_idx, word);
        word_idx = skip_whitespace(cur_line, word_idx);

        if (strcmp(word, "mcroend") == 0){ /*macro ended*/
            if (get_next_word(cur_line, &word_idx, word) == 1){ /*there is a word after mcroend*/
                fprintf(stderr, "Error: Unexpected text after 'mcroend' at line %d.\n", *line_idx);
                return -1;
            }
            flag = 1;
        }
        
        else{
            if (add_line_to_macro(node, cur_line) == -1){ /*checks memory allocation error*/
                fprintf(stderr, "memmory allocation ERROR");
                return -1;
            }
        }
        (*line_idx)++;
    }

    return 0;
}

/* Replace a macro with its lines in the output file */
/*returns 0 if successful, -1 if an error occurs*/
int replace_macro(MacroNode *macro, FILE *output_file_am) {
    if (macro == NULL) {
        fprintf(stderr, "Macro node is NULL.\n");
        return -1; /*macro node is NULL*/
    }

    LineNode *cur_line_node = macro->lines_head;
    while (cur_line_node != NULL) {
        fputs(cur_line_node->line, output_file_am);
        cur_line_node = cur_line_node->next;
    }

    return 0; 
}

/*puts the line to the output file am and if there is a macro it replaces it*/
/*returns 0 if successful, -1 if an error occurs*/
int put_line(FILE *output_file_am, char *cur_line, MacroNode *mcro_node_head) {
    char cur_word[MAX_LINE_LEN];
    int word_idx = 0;

    get_next_word(cur_line, &word_idx, cur_word);
    if (cur_word[0] != '\0') { /*if the line is not empty*/
        MacroNode *found_macro = find_macro(mcro_node_head, cur_word);
        if (found_macro != NULL) { /*if the word is a macro name*/
            if (get_next_word(cur_line, &word_idx, cur_word) == 1) { /*there is a word after the macro name*/
                fprintf(stderr, "Error: Unexpected text after macro name '%s'.\n", found_macro->name);
                return -1;
            }
            if (replace_macro(found_macro, output_file_am) == -1) {
                fprintf(stderr, "Error replacing macro.\n");
                return -1;
            }
        } else { /*if the word is not a macro name*/
            fputs(cur_line, output_file_am);
        }
    }

    return 0; 
}
