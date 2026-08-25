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



/*returns 1 if label was added successfully, -1 otherwise, -2 for memory allocation error*/
int add_label(LabelNode **head, char *name, int address, LabelType type);
/*returns 1 if the word is a label, 0 otherwise*/
int is_label(const char *word);
/*gets a label, returns the label type*/
LabelType get_label_type(const char *word);
/* returns a pointer to the label node if found, NULL otherwise */
LabelNode* find_label(LabelNode *head, const char *name);
/*checking if its a valid symbol name without the ":"*/
/*returns 1 if the symbol name is valid, 0 otherwise*/
int is_valid_label_name(const char *name);
/*adds a new extern usage node to the list*/
/*returns 1 if successful, -1 otherwise, returns -2 if memory allocation fails*/
int add_ExternUsage_node(ExternUsageNode **head, char *name, long address);
/*initialize AssemblerData*/
/*returns 1 if successful, -1 if failed*/
int initialize_assembler_data(AssemblerData *data);

/*frees the nodes in the ExternUsageNode list*/
void free_extern_usage_table(ExternUsageNode *head);
/*frees the nodes in the LabelNode list*/
void free_label_table(LabelNode *head);

#endif /*DATA_TABLE_H */
