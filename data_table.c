#include "data_table.h"
#include "reserved_word.h"
#include "writing_to_binary.h"
#include "consts.h"

#include <ctype.h>

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
    if (is_data_directive(word)) {
        return DATA;
    } else if (is_instruction(word)) {
        return CODE;
    } else if (strcmp(word, ".extern") == 0) {
        return EXTERN;
    } else if (strcmp(word, ".entry") == 0) {
        return ENTRY;
    } else {
        return -1; /* Not a recognized label type */
    }
}

/*returns 1 if the word is a label, 0 otherwise*/
int is_label(const char *word) {
    int len = strlen(word);
    int i;
    if (len == 0 || len > MAX_SYMBOL_NAME_LEN) {
        return 0; /* Not a label */
    }
    if (!isalpha(word[0])) {
        return 0; /* Must start with a letter */
    }
    for (i = 1; i < len-1; i++) {
        if (!isalnum(word[i])) {
            return 0; /* Must be alphanumeric*/
        }
    }
    if (word[len - 1] != ':') {
        return 0; /* Must end with : */
    }

    return 1; /* Valid label */
}

/* returns a pointer to the label node if found, NULL otherwise */
LabelNode* find_label(LabelNode *head, const char *name) {
    LabelNode *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current; /* Label found */
        }
        current = current->next;
    }
    return NULL; /* Label not found */
}