#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "consts.h"
#include "utils.h"
#include "reserved_word.h"
#include "data_table.h"
#include "macro_table.h"

/*returns the index of the first non-whitespace character from the given index*/
int skip_whitespace(char *line, int index){
    while (line[index] == ' ' || line[index] == '\t') {
        index++;
    }
    return index;
}


/*puts the next word into word, returns 1 if there was a word and 0 otherwise, stops at white space*/
int get_next_word(char *line, int *index, char *word){
    int i = *index;
    int j = 0;

    i = skip_whitespace(line, *index);

    /*if there is no word*/
    if (line[i] == '\0' || line[i] == '\n') {
        word[0] = '\0';
        *index = i;
        return 0;
    }

    /*set the word*/
    while (line[i] != ' ' && line[i] != '\t' && line[i] != '\0'
         && line[i] != '\n' && j <MAX_LINE_LEN - 1){
        word[j] = line[i];
        j++;
        i++;
    }
    
    /*en*/
    word[j] = '\0';
    *index = i;
    return 1;
}


/*puts the next word into word, returns 1 if there was a word and 0 if there is no word, stops at the next word*/
/*a word has to be devided by a comma*/
/*index is at the end of the word*/
int get_next_command(char *line, int *index, char *word){
    int i = *index;
    int j = 0;
    int tmp_idx;
    char tmp_word[MAX_LINE_LEN];

    i = skip_whitespace(line, i);

    /*if there is no word*/
    if (line[i] == '\0' || line[i] == '\n' || line[i] == ',') {
        word[0] = '\0';
        *index = i;
        return 0;
    }

    /*set the word*/
    while (line[i] != ',' && line[i] != '\t' && line[i] != '\0'
         && line[i] != '\n' && line[i] != ' ' && j <MAX_LINE_LEN - 1){
        word[j] = line[i];
        j++;
        i++;
    }
    
    word[j] = '\0';

    i = skip_whitespace(line, i);

    if (line[i] == ',') {
        /*if the next character is a comma, skip it*/
        i++;
        tmp_idx = i;
        if (get_next_word(line, &tmp_idx, tmp_word) == 0) { /*there is a trailing comma without a word after it*/
            word[0] = '\0';
            *index = i;
            return 0;
        }
    }
    else if(line [i] != '\0' && line[i] != '\n') { /*if there are charecrers where they are not suppost to be*/
        word[0] = '\0';
        *index = i;
        return 0;
    }

    
    *index = i;
    return 1;
}

/*checks if its a mcro and then puts the name of the mcro into name*/
/*returns 0 if not mcro and 1 if mcro*/
/*returns -1 if there is an error*/
int get_macro_initialization_name_from_line(char *line, char* name){
    char word[MAX_LINE_LEN];
    int word_idx = 0;
    int i = 0;
    /*gets the first word in the line*/
    if (get_next_word(line, &word_idx, word)==0){
        return 0;
    }

    /*checks if its a mcro*/
    if (strcmp(word, "mcro") == 0){
        word_idx = skip_whitespace(line, word_idx);
        
        /*checks if valid mcro name*/
        if (get_next_word(line, &word_idx, word) == 0){
            fprintf(stdout, "Error: Macro name is missing.\n");
            return -1;
        }
        if (is_reserved_word(word) == 1){
            fprintf(stdout, "Error: Macro name is reserved.\n");
            return -1;
        } 
        if (!isalpha(word[0])) {
            fprintf(stdout, "Error: Macro name must start with a letter.\n");
            return -1; /* Macro name must start with a letter */;
        }

        if (strlen(word) >= MAX_MACRO_NAME_LEN) {
            fprintf(stdout, "Error: Macro name exceeds maximum length of %d characters.\n", MAX_MACRO_NAME_LEN);
            return -1; /* Macro name exceeds maximum length */;
        }
        for (i = 1; word[i] != '\0' && word[i] != '\n'; i++) { /*checks if the name contains only alphanumeric characters*/
            if (!isalnum(word[i]) && word[i] != '_') {
                fprintf(stdout, "Error: Macro name must contain only alphanumeric characters or underscores.\n");
                return -1; /* Macro name must contain only alphanumeric characters or underscores */;
            }
        }

        /*sets the name*/
        strcpy(name, word);

        if (get_next_word(line, &word_idx, word) == 1){ /*the name has a white space or illegal character*/
            fprintf(stdout, "Error: Macro name must be a single word.\n");
            return -1;
        }  



        return 1;
    }
    else{
        return 0;
    }
}


/*change comma to whitespace, dosent remove 2 commas in a row (will only replace one)*/
/*returns 1 if successful, -1 if error*/
int change_single_comma_to_whitespace(char *line){
    int len = strlen(line);

    char last = '\0';

    int i;
    for (i = 0; i < len; i++) {
        if (line[i] == ',' && last != ',') {
            return -1; /*there is a double comma*/
        } else {
            last = line[i];
        } 
    }
    
    
    return 1; /* No comma to remove */
}

/*returns 1 if its only digits, 0 otherwise */
int is_strictly_digits(const char *str) {
    if (str == NULL || *str == '\0') {
        /*check for empty words*/
        return 0;
    }

    while (*str != '\0' && *str != '\n') {
        /* Cast to unsigned char to avoid undefined behavior with negative values */
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

/*returns 1 if a label is found and extracted, -1 otherwise*/
int get_label_name(char *line, int *word_idx, char *label_name) {
    char label_tmp[MAX_LINE_LEN];
    if (get_next_word(line, word_idx, label_tmp) == 0) {
        return -1; /* No label found */
    }

    if (is_label(label_tmp) == 0) {
        return -1; /* Not a valid label */
    }

    label_tmp[strlen(label_tmp) - 1] = '\0'; /* Remove the trailing ':' */
    strcpy(label_name, label_tmp);
    return 1;


    return -1; /* Error */
}


/*flushes the rest of the line*/
void flush_line(FILE *fp) {
    int ch;
    while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
        /* discard characters */
    }
}

/*returns 1 if the line should be skipped, 0 otherwise*/
int should_skip_line(char *line) {
    int word_idx = 0;
    char word[MAX_LINE_LEN];

    if (get_next_word(line, &word_idx, word) == 0) {
        /* Empty line, should skip */
        return 1;
    }
    else if (word[0] == ';') {
        /* Comment line, should skip */
        return 1;
    }
    return 0; /* Line should not be skipped */
}

/*returns 1 if entry directive, 0 otherwise*/
int is_entry_directive(char *line, int word_idx) {
    char word[MAX_LINE_LEN];

    if (get_next_word(line, &word_idx, word) == 0) {
        return 0; /* No words in the line */
    }

    if (strcmp(word, ".entry") == 0) {
        return 1; /* It's an .entry directive */
    }

    return 0; /* Not an .entry directive */
}


/*returns 1 if incremented, 0 if not incremented, -1 if error*/
int increment_DC(char* line, int *word_idx, char *command, int *cur_DC,AssemblerData *data) {
    int size;
    if (is_data_directive(command) && strcmp(command, ".asciz") != 0) {
        size = (strcmp(command, ".db") == 0) ? 1 : ((strcmp(command, ".dh") == 0) ? 2 : (strcmp(command, ".dw") == 0) ? 4 : 0);
        if (size == 0) {
            fprintf(stdout, "Error: Unknown directive '%s'\n", command);
            data->error_flag = 1;
            return -1;
        } 
        *cur_DC += size; /* Increment DC by the size of the directive */
        return 1;        
    }
    else if (strcmp(command, ".asciz") == 0) {
        skip_whitespace(line, *word_idx);
        if (line[*word_idx] != '"') {
            fprintf(stdout, "Error: .asciz string must start with a quote\n");
            data->error_flag = 1;
            return -1;
        }
        (*word_idx)++; /* Skip the starting quote */
        while(line[*word_idx] != '\0' && line[*word_idx] != '\n' && line[*word_idx] != '"') {
            if ((unsigned char)line[*word_idx] < 32 || (unsigned char)line[*word_idx] > 126) { /*check for non ASCII characters*/
                fprintf(stdout, "Error: .asciz contains a non-printable ASCII character\n");
                data->error_flag = 1;
                return -1;
            }
            (*cur_DC)++; /* Increment DC for each character */
            (*word_idx)++;
        }

        return 1;
    }
    return 0; /*nothing was incremented*/
}

void free_everything(AssemblerData *data, MacroNode *macro_head, ExternUsageNode *extern_head,
     char *am_file_name, char *base_file_name, char *ob_file_name, char *ext_file_name, char *ent_file_name) {
    if (data != NULL) {
        free_label_table(data->label_head);
    }
    free_macro_table(macro_head);
    free_extern_usage_table(extern_head);
    free(am_file_name);
    free(base_file_name);
    free(ob_file_name);
    free(ext_file_name);
    free(ent_file_name);
}


/*returns 1 if the image has enough space, 0 otherwise*/
int does_image_have_space(int cur_size, int needed_bytes){
    if (cur_size < 0 || needed_bytes < 0) {
        return 0; /* Invalid parameters */
    }
    return (cur_size <= MAX_MEM_SIZE - needed_bytes);
}