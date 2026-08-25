#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include "macro_table.h"
#include "data_table.h"

/*runs the second pass on the am file*/
/*returns 1 if successful, -1 if error, MEMORY_ALLOCATION_ERROR if memory allocation error*/
int run_second_pass(FILE *am_file, AssemblerData *data, ExternUsageNode **extern_head);

/*returns 1 if successful, 0 if skipped, -1 if failed*/
int process_line_second_pass(char *cur_line, AssemblerData *data, int line_idx,
int *cur_IC, int *cur_DC, ExternUsageNode **extern_head);

/*handles the .entry directive in the second pass*/
/*returns 1 if successful, -1 if error*/
int handle_entry_directive_second_pass(char *line, int word_idx, AssemblerData *data);

/*handles branch instructions in the second pass*/
/*returns 1 if successful, -1 if error*/
int handle_branch_instruction_second_pass(char *line, int *word_idx, AssemblerData *data, int cur_IC);

/*gets the value for a J-type instruction and puts them in the appropriate variables*/
/*returns 1 if successful, -1 if failed*/
int get_j_type_value(char *line,  char *label_name, int *word_idx, int *op_code, char *operand, AssemblerData *data);

/*handles J-type instructions in the second pass*/
/*returns 1 if successful, -1 if error, MEMORY_ALLOCATION_ERROR if memory allocation error*/
int handle_j_type_instruction_second_pass(char *line, int *word_idx, int cur_IC, AssemblerData *data, ExternUsageNode **extern_head);

#endif /* SECOND_PASS_H */
