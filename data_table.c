#include "data_table.h"
#include "reserved_word.h"
#include "writing_to_binary.h"
#include "consts.h"

/*returns 1 if label was added successfully, -1 otherwise*/
int add_label(LabelNode **head, char *name, int address, LabelType type) {
    LabelNode *new_node = (LabelNode *)malloc(sizeof(LabelNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed for new label node.\n");
        return -1; /*memory allocation failed*/
    }
    /* Initialize the new label node */
    strncpy(new_node->name, name, MAX_SYMBOL_NAME_LEN);
    new_node->name[MAX_SYMBOL_NAME_LEN - 1] = '\0'; /* Ensure correct termination */
    new_node->address = address;
    new_node->type = type;
    new_node->next = NULL;

    /* Add to the head of the list */
    if (*head == NULL) {
        *head = new_node;
    } else {
        new_node->next = *head;
        *head = new_node;
    }


    return 1; 
}

LabelType get_label_type(const char *word) {
    return CODE; /* Placeholder implementation, should be replaced with actual logic */
}