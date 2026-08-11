#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H
#include "consts.h"

/* Line node for storing each line of the macro */
typedef struct LineNode {
    char line[MAX_LINE_LEN];
    struct LineNode *next;
} LineNode;

/* Macro node for storing each macro */
typedef struct MacroNode {
    char name[Max_MACRO_NAME_LEN];
    LineNode *lines_head;
    LineNode *lines_tail;
    struct MacroNode *next;
} MacroNode;

#endif
/* MACRO_TABLE_H */