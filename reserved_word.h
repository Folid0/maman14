#ifndef RESERVED_WORD_H
#define RESERVED_WORD_H


/*checks if the word is a reserverd world like "add" returns 1 if true else 0*/
int is_reserved_word(const char *word);

/*returns 1 if the word is an instruction, else returns 0*/
int is_instruction(const char *word);

/*returns 1 if the word is a directive, else returns 0*/
int is_directive(const char *word);

/*returns 1 if the word is a data directive, else returns 0*/
/*".db", ".dh", ".dw", ".asciz"*/
int is_data_directive(const char *word);

/*returns 1 if the word is a register, else returns 0*/
int is_register(const char *word);

/*returns 1 if the word is a macro keyword, else returns 0*/
int is_macro_keyword(const char *word);

/*returns 1 if the word is an R-type instruction, 0 otherwise*/
int is_r_type_instruction(const char *word);

/*returns 1 if the word is an I-type instruction, 0 otherwise*/
int is_i_type_instruction(const char *word);

/*returns 1 if the word is a J-type instruction, 0 otherwise*/
int is_j_type_instruction(const char *word);

/*returns 1 if the word is a branch instruction, 0 otherwise*/
int is_branch_instruction(const char *word);
#endif /* RESERVED_WORD_H */

