#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"

typedef enum{
    CODE,
    DATA,
    ENTRY,
    EXTERN
} LabelType;

typedef struct LabelNode {
    char name[Max_SYMBOL_NAME_LEN];
    long address;
    LabelType type;
    struct LabelNode *next;
} LabelNode;


typedef struct {
    unsigned char code_image[MAX_MEM_SIZE];
    unsigned char data_image[MAX_MEM_SIZE];
    int IC;
    int DC;
    LabelNode *label_head;
    int error_flag;
} AssemblerData;

int process_line_first_pass(char *line, AssemblerData *data, int line_idx);
int should_skip_line(char *line);

#endif /* SYMBOL_TABLE_H */