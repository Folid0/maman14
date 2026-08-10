#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "macro_table.h"

/* Add a new macro to the list */
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