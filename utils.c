#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "consts.h"
#include "utils.h"
#include "reserved_word.h"


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

/*checks if its a mcro and then puts the name of the mcro into name*/
/*returns 0 if not mcro and 1 if mcro*/
/*returns -1 if there is an error*/
int check_set_mcro(char *line, char* name){
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
            fprintf(stderr, "Error: Macro name is missing.\n");
            return -1;
        }
        if (is_reserved_word(word) == 1){
            fprintf(stderr, "Error: Macro name is reserved.\n");
            return -1;
        } 
        if (!isalpha(word[0])) {
            fprintf(stderr, "Error: Macro name must start with a letter.\n");
            return -1; /* Macro name must start with a letter */;
        }

        for (i = 1; word[i] != '\0' && word[i] != '\n'; i++) { /*checks if the name contains only alphanumeric characters*/
            if (!isalnum(word[i])) {
                fprintf(stderr, "Error: Macro name must contain only alphanumeric characters.\n");  
                return -1; /* Macro name must contain only alphanumeric characters */;
            }
        }

        /*sets the name*/
        strcpy(name, word);
        
        if (get_next_word(line, &word_idx, word) == 1){ /*the name has a white space or illegal character*/
            fprintf(stderr, "Error: Macro name must be a single word.\n");
            return -1;
        }  



        return 1;
    }
    else{
        return 0;
    }


}


