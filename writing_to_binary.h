#ifndef WRITING_TO_BINARY_H
#define WRITING_TO_BINARY_H
#include "data_table.h"
#include "macro_table.h"
#include "error_information.h"

/*writes the line in binary to the data*/
/*word idx is on the name of the command (ussaly the first word or seccond if there is a label)*/
/*returns 1 if successful, -1 otherwise*/
int handle_CODE(char *line, int *word_idx, AssemblerData *data, ErrorInfo *error_info);

/*encodes the r type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int encode_r_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data, ErrorInfo *error_info);

/*encodes the i type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int encode_i_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data, ErrorInfo *error_info);

/*encodes the j type line to binary to the data*/
/*returns -1 if error, 1 if successful*/
int encode_j_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data, ErrorInfo *error_info);

/*encodes line with data directive to binary into data*/
/**/
int handle_data_directive(char *line, int *word_idx, AssemblerData *data, ErrorInfo *error_info);

/* Handles the .entry directive during the first pass */
int handle_entry_directive_first_pass(char *line, int *word_idx, char *name, AssemblerData *data, ErrorInfo *error_info);

/*encodes the extern data to the symbol table*/
/*returns 1 if successful, -1 on failure*/
int encode_extern_directive(char *line, int *word_idx, char *name, AssemblerData *data, ErrorInfo *error_info);

/*encodes .asciz directive to binary*/
/*returns 1 if successful, -1 if error*/
int encode_asciz_directive(char *line, int *word_idx, char *name, AssemblerData *data, ErrorInfo *error_info);

/*encodes .db, .dh, and .dw directives to binary intot the data image */
/*returns 1 if successful, -1 if error*/
int encode_db_dw_db_directive(char *line, int *word_idx, char *name, AssemblerData *data, ErrorInfo *error_info);


/*return 1 if the label was handled successfully, -1 otherwise*/
int handle_label(char *line, AssemblerData *data, int line_idx, MacroNode *macro_head, ErrorInfo *error_info);

/*encodes a J-type instruction in the second pass to the code image*/
/*returns 1 if successful, -1 if error*/
int encode_j_type_instruction_second_pass(char *line, LabelNode *label_node, AssemblerData *data, int *cur_IC, ErrorInfo *error_info);

/*encodes a branch instruction in the second pass to the code image*/
/*returns 1 if successful, -1 if error*/
int encode_branch_instruction_second_pass(int offset, AssemblerData *data, int cur_IC, ErrorInfo *error_info);

#endif /* WRITING_TO_BINARY_H */
