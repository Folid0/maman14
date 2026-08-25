#ifndef UTILS_H
#define UTILS_H

#include "data_table.h"
#include "macro_table.h"

/*returns the index of the first non-whitespace character from the given index*/
int skip_whitespace(char *line, int index);

/*puts the next word in the array, and returns 1 if there was a word and 0 if end of line*/
int get_next_word(char *line, int *index, char *word);

/*checks if its a mcro and then puts the name of the mcro into name*/
/*returns 0 if not mcro and 1 if mcro*/
/*returns -1 if there is an error*/
int get_macro_initialization_name_from_line(char *line, char* name);


/*puts the next word into word, returns 1 if there was a word and 0 if there is no word, stops at the next word*/
/*a word has to be devided by a comma*/
/*index is at the end of the word*/
int get_next_command(char *line, int *index, char *word);

/*returns 1 if its only digits, 0 otherwise */
int is_strictly_digits(const char *str);

/*returns 1 if a label is found and extracted, -1 otherwise*/
int get_label_name(char *line, int *word_idx, char *label_name);

/*returns 1 if the line should be skipped, 0 otherwise*/
int should_skip_line(char *line);

/*returns 1 if entry directive, 0 otherwise*/
int is_entry_directive(char *line, int word_idx);

/*returns 1 if the image has enough space, 0 otherwise*/
int does_image_have_space(int cur_size, int needed_bytes);


/*frees all allocated memory*/
void free_everything(AssemblerData *data, MacroNode *macro_head, ExternUsageNode *extern_head,
     char *am_file_name, char *base_file_name, char *ob_file_name, char *ext_file_name, char *ent_file_name);

/*flushes the rest of the line*/
void flush_line(FILE *fp);

#endif /* UTILS_H */
