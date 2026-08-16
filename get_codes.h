#ifndef GET_CODES_H
#define GET_CODES_H

/*returns the opcode for the given r type instruction*/
/*returns -1 if the instruction is invalid*/
int get_opcode(char *name);

/*gets a command name and returns its funct value*/
/*if no funct value then returns 0*/
int get_funct(char *name);

/*returns the number of the register, if not a register returns -1*/
int get_register_num(char *name); 

#endif /* GET_CODES_H */