#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"

typedef enum{
    CODE,
    DATA,
    EXTERN,
    ENTRY /*if entry and another type are both on the same sybol than we choose the other type here
     and we will know its also entry by the is_entry flag */
} LabelType;

typedef struct LabelNode {
    char name[MAX_SYMBOL_NAME_LEN];
    long address;
    LabelType type;
    int is_entry;
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

int should_skip_line(char *line);
int add_label(LabelNode **head, char *name, int address, LabelType type);
int is_label(const char *word);
LabelType get_label_type(const char *word);
LabelNode* find_label(LabelNode *head, const char *name);

#endif /* SYMBOL_TABLE_H */