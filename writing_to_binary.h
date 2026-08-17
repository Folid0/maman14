#ifndef WRITING_TO_BINARY_H
#define WRITING_TO_BINARY_H
#include "data_table.h"

int handle_CODE(char *line, int *word_idx, AssemblerData *data);

int handle_r_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data);

int handle_i_type_instruction(char *line, int *word_idx, char *name, AssemblerData *data);

int handle_DATA();

int handle_ENTRY();

int handle_EXTERN();

#endif /* WRITING_TO_BINARY_H */