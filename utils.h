#ifndef UTILS_H
#define UTILS_H

/*returns the index of the first non-whitespace character from the given index*/
int skip_whitespace(char *line, int index);

/*puts the next word in the array, and returns 1 if there was a word and 0 if end of line*/
int get_next_word(char *line, int *index, char *word);

/*checks if its a mcro and then puts the name of the mcro into name*/
/*returns 0 if not mcro and 1 if mcro*/
/*returns -1 if there is an error*/
int check_set_mcro(char *line, char* name);

/*change comma to whitespace, dosent remove 2 commas in a row (will only replace one)*/
/*returns 1 if successful, -1 if error*/
int change_single_comma_to_whitespace(char *line);
#endif /* UTILS_H */