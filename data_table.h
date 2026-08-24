#ifndef DATA_TABLE_H
#define DATA_TABLE_H

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

typedef struct ExternLabelNode{
    char name[MAX_SYMBOL_NAME_LEN];
    long address;
    struct ExternLabelNode *next;
} ExternUsageNode;



int add_label(LabelNode **head, char *name, int address, LabelType type);
int is_label(const char *word);
LabelType get_label_type(const char *word);
LabelNode* find_label(LabelNode *head, const char *name);
int is_valid_label_name(const char *name);
int add_ExternUsage_node(ExternUsageNode **head, char *name, long address);
int initialize_assembler_data(AssemblerData *data);

void free_extern_usage_table(ExternUsageNode *head);
void free_label_table(LabelNode *head);

#endif /*DATA_TABLE_H */