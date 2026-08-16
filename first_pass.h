#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "data_table.h"


int handle_label(char *line, AssemblerData *data, int line_idx);

/*gets the second word from the line declaring a label and returns its type*/
LabelType get_label_type(const char *word);

/*returns 1 if the word is a label, 0 otherwise*/
int is_label(const char *word);

/*returns 1 if the line should be skipped, 0 otherwise*/
int should_skip_line(char *line);

int process_line_first_pass(char *line, AssemblerData *data, int line_idx);

int run_first_pass(FILE *am_file, AssemblerData *data);



#endif /* FIRST_PASS_H */