#ifndef FIRST_PASS_H
#define FIRST_PASS_H
#include "data_table.h"
#include "macro_table.h"
#include "error_information.h"

/*gets the second word from the line declaring a label and returns its type*/
LabelType get_label_type(const char *word);

/*returns 1 if the word is a label, 0 otherwise*/
int is_label(const char *word);

/*returns 1 if there was a line to process, 0 if skipped, -1 if error*/
int process_line_first_pass(char *line, AssemblerData *data, int line_idx, MacroNode *macro_head, ErrorInfo *error_info);

/*returns 1 if successful, -1 if there was an error*/
int run_first_pass(FILE *am_file, AssemblerData *data, MacroNode *macro_head, ErrorInfo *error_info);



#endif /* FIRST_PASS_H */
