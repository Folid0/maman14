#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H
#include <string.h>


#include "consts.h"
#include "error_information.h"

/* Line node for storing each line of the macro */
typedef struct LineNode {
    char line[MAX_LINE_LEN];
    struct LineNode *next;
} LineNode;

/* Macro node for storing each macro */
typedef struct MacroNode {
    char name[MAX_MACRO_NAME_LEN];
    LineNode *lines_head;
    LineNode *lines_tail;
    struct MacroNode *next;
} MacroNode;


/* Add a new macro to the list */
/*returns -1 if an error occurs, 0 if successful*/
int add_macro(MacroNode **head, char *name, ErrorInfo *error_info);

/* Add a line to a macro */
int add_line_to_macro(MacroNode *macro, char *line, ErrorInfo *error_info);

/* Find a macro by its name */
/*returns a pointer to the macro if found, NULL otherwise*/
MacroNode *find_macro(MacroNode *head, char *name);

/* Free the entire macro table includig the lines */
void free_macro_table(MacroNode *head);


/*add all the macro lines to the macro*/
/*returns -1 if an error occurs, probably memory allocation error*/
int add_lineblock_to_macro(MacroNode *node, char *cur_line, FILE *input_file_as,int *line_idx, ErrorInfo *error_info);

/*puts the line to the output file am and if there is a macro it replaces it*/
/*returns 0 if successful, -1 if an error occurs*/
int put_line(FILE *output_file_am, char *cur_line, MacroNode *mcro_node_head, ErrorInfo *error_info);

/* Replace a macro with its lines in the output file */
/*returns 0 if successful, -1 if an error occurs*/
int replace_macro(MacroNode *macro, FILE *output_file_am, ErrorInfo *error_info);




#endif
/* MACRO_TABLE_H */
