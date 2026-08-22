#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include "macro_table.h"
#include "data_table.h"

int run_second_pass(FILE *am_file, AssemblerData *data, ExternUsageNode **extern_head);
int process_line_second_pass(char *cur_line, AssemblerData *data, int line_idx,
     int *cur_IC, int *cur_DC, ExternUsageNode **extern_head);
int handle_entry_directive_second_pass(char *line, AssemblerData *data);
int handle_branch_instruction_second_pass(char *line, int *word_idx, AssemblerData *data, int cur_IC);
int get_j_type_value(char *line,  char *label_name, int *word_idx, int *op_code, char *operand, AssemblerData *data);
int handle_j_type_instruction_second_pass(char *line, int *word_idx, int cur_IC, AssemblerData *data, ExternUsageNode **extern_head);
#endif /* SECOND_PASS_H */