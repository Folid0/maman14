#ifndef UTILS_H
#define UTILS_H

/*returns the index of the first non-whitespace character from the given index*/
int skip_whitespace(char *line, int index);

/*puts the next word in the array, and returns 1 if there was a word and 0 if end of line*/
int get_next_word(char *line, int *index, char *word);

/*checks if the word is a reserverd world like "add" returns 1 if true else 0*/
int is_reserved_word(const char *word);

#endif /* UTILS_H */