#ifndef RESERVED_WORD_H
#define RESERVED_WORD_H


/*checks if the word is a reserverd world like "add" returns 1 if true else 0*/
int is_reserved_word(const char *word);

int is_instruction(const char *word);

int is_directive(const char *word);

int is_register(const char *word);

int is_macro_keyword(const char *word);

#endif /* RESERVED_WORD_H */

