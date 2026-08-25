#include "data_table.h"
#include "reserved_word.h"
#include "writing_to_binary.h"
#include "consts.h"
#include "utils.h"

#include <ctype.h>

/*returns 1 if label was added successfully, -1 otherwise, -2 for memory allocation error*/
int add_label(LabelNode **head, char *name, int address, LabelType type) {
    LabelNode *new_node = (LabelNode *)malloc(sizeof(LabelNode));
    if (new_node == NULL) {
        fprintf(stdout, "Memory allocation failed for new label node.\n");
        return MEMORY_ALLOCATION_ERROR; /*memory allocation failed*/
    }
    /* Initialize the new label node */
    strncpy(new_node->name, name, MAX_SYMBOL_NAME_LEN);
    new_node->name[MAX_SYMBOL_NAME_LEN - 1] = '\0'; /* Ensure correct termination */
    new_node->address = address;
    new_node->type = type;
    new_node->next = NULL;
    new_node->is_entry = 0;
    
    /* Add to the head of the list */
    if (*head == NULL) {
        *head = new_node;
    } else {
        new_node->next = *head;
        *head = new_node;
    }


    return 1; 
}

/*gets a label, returns the label type*/
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
    char copy_word[MAX_SYMBOL_NAME_LEN];
    if (len == 0 || len > MAX_SYMBOL_NAME_LEN) {
        return 0; /* Not a label */
    }
    if (!isalpha((unsigned char)word[0])) {
        return 0; /* Must start with a letter */
    }
    for (i = 1; i < len-1; i++) {
        if (!isalnum((unsigned char)word[i])) {
            return 0; /* Must be alphanumeric*/
        }
    }
    if (word[len - 1] != ':') {
        return 0; /* Must end with : */
    }
    
    strncpy(copy_word, word, len - 1);
    copy_word[len - 1] = '\0'; /*remove the trailing ":"*/
    if (is_reserved_word(copy_word) == 1) {
        return 0; /* Cannot be a reserved word */
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

/*checking if its a valid symbol name without the ":"*/
/*returns 1 if the symbol name is valid, 0 otherwise*/
int is_valid_label_name(const char *name)
{
    int i;
    int length;

    if (name == NULL) {
        return 0;
    }

    length = strlen(name);

    if (length == 0 || length >= MAX_SYMBOL_NAME_LEN) {
        return 0;
    }

    if (!isalpha((unsigned char)name[0])) {
        return 0;
    }

    for (i = 1; i < length; i++) {
        if (!isalnum((unsigned char)name[i])) {
            return 0;
        }
    }

    if (is_reserved_word(name) == 1) {
        return 0;
    }

    return 1;
}

/*adds a new extern usage node to the list*/
/*returns 1 if successful, -1 otherwise, returns -2 if memory allocation fails*/
int add_ExternUsage_node(ExternUsageNode **head, char *name, long address) {
    ExternUsageNode *new_node = (ExternUsageNode *)malloc(sizeof(ExternUsageNode));
    if (new_node == NULL) {
        fprintf(stdout, "Memory allocation failed for new extern usage node.\n");
        return MEMORY_ALLOCATION_ERROR; /* Memory allocation failed */
    }

    /* Initialize the new extern usage node */
    strncpy(new_node->name, name, MAX_SYMBOL_NAME_LEN);
    new_node->name[MAX_SYMBOL_NAME_LEN - 1] = '\0'; /* Ensure correct termination */
    new_node->address = address;
    new_node->next = NULL;

    /* Add to the head of the list */
    if (*head == NULL) {
        *head = new_node;
    } else {
        new_node->next = *head;
        *head = new_node;
    }

    return 1; /* Successfully added */

}

/*frees the nodes in the ExternUsageNode list*/
void free_extern_usage_table(ExternUsageNode *head){
    ExternUsageNode *tmp = head;
    ExternUsageNode *next;

    while (tmp != NULL) {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
}

/*frees the nodes in the LabelNode list*/
void free_label_table(LabelNode *head){
    LabelNode *tmp = head;
    LabelNode *next;

    while (tmp != NULL) {
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
}


/*initialize AssemblerData*/
/*returns 1 if successful, -1 if failed*/
int initialize_assembler_data(AssemblerData *data) {
    if (data == NULL) {
        fprintf(stdout, "Error: AssemblerData pointer is NULL\n");
        return -1;
    }
    memset(data->code_image, 0, sizeof(data->code_image));
    memset(data->data_image, 0, sizeof(data->data_image));
    data->IC = 100; /* IC starts at 100, like the rules say */
    data->DC = 0;
    data->label_head = NULL;
    data->error_flag = 0;
    return 1;
}