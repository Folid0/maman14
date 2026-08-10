#ifndef MACRO_TABLE_H
#define MACRO_TABLE_H
#include "consts.h"
typedef struct LineNode {
    char line[MAX_LINE_LEN];
    struct LineNode *next;
} LineNode;

#endif
/* MACRO_TABLE_H */