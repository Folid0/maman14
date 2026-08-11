#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"

/*returns the index of the first non-whitespace character from the given index*/
int skip_whitespace(char *line, int index){
    while (line[index] == ' ' || line[index] == '\t') {
        index++;
    }
    return index;
}


/*puts the next word into word, returns 1 if there was a word and 1 otherwise*/
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

/*returns 1 if its a reserved word and 0 otherwise*/
int is_reserved_word(char* word){
    int i = 0;
    for (i = 0; i < NUM_INSTRUCTIONS; i++){
        if (strcmp(word, INSTRUCTIONS[i])){
            return 1;
        }
    }
    for (i = 0; i < NUM_DIRECTIVES; i++){
        if (strcmp(word, DIRECTIVES[i])){
            return 1;
        }
    }
    for (i = 0; i < NUM_REGISTERS; i++){
        if (strcmp(word, REGISTERS[i])){
            return 1;
        }
    }
    for (i = 0; i < NUM_REGISTERS; i++){
        if (strcmp(word, REGISTERS[i])){
            return 1;
        }
    }

    /*its not a reserved word*/
    return 0;
}
