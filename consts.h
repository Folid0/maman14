#ifndef CONSTS_H
#define CONSTS_H

#define MAX_LINE_LEN 82
#define Max_MACRO_NAME_LEN 32
#define Max_FILE_NAME_LEN 256 /*change this. u gotta make it dynamic*/


/*arrays with the saved words*/
extern const char *INSTRUCTIONS[];
extern const char *DIRECTIVES[];
extern const char *REGISTERS[];
extern const char *MACRO_KEYWORDS[];

/*size of the arrays*/
extern const int NUM_INSTRUCTIONS;
extern const int NUM_DIRECTIVES;
extern const int NUM_REGISTERS;
extern const int NUM_MACRO_KEYWORDS;

#endif /* CONSTS_H */