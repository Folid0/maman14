#ifndef WRITING_TO_BINARY_H
#define WRITING_TO_BINARY_H
#include "data_table.h"

int handle_CODE(char *line, int *word_idx, AssemblerData *data);

int encode_r_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data);

int encode_i_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data);

int encode_j_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data);

int handle_data_directive(char *line, int *word_idx, AssemblerData *data);
int handle_entry_directive_first_pass(char *line, int *word_idx, char *name, AssemblerData *data);
int encode_extern_directive(char *line, int *word_idx, char *name, AssemblerData *data);
int encode_asciz_directive(char *line, int *word_idx, char *name, AssemblerData *data);
int encode_db_dw_db_directive(char *line, int *word_idx, char *name, AssemblerData *data);
#endif /* WRITING_TO_BINARY_H */