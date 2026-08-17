#include <stdio.h>
#include "macro_table.h"
#include "utils.h"
#include <stdlib.h>
#include "pre_assembler.h"
#include "writing_to_binary.h"

int main(int argc, char *argv[]) {
    /*this is all for testing*/

    /*
    char* line1 = "hellojkolj world";
    char* word = (char*)malloc(sizeof(82));
    int idx = 0;
    */
    
    /*
    printf("%d\n", get_next_word(line1, &idx, word));
    printf("%s\n", word);
    idx = skip_whitespace(line1, idx);
    printf("%d\n", get_next_word(line1, &idx, word));
    printf("%s\n", word);
    */    

    /*run_pre_assembler("test");*/
    
    char thing[MAX_LINE_LEN] = "add $1, $1, $1";
    int word_idx = 3;
    char name[MAX_LINE_LEN] = "add";
    unsigned long ret = 0;
    AssemblerData data;

    data.IC = 100; /* Standard starting IC address for the project */
    data.DC = 0;
    data.label_head = NULL;
    data.error_flag = 0;
    memset(data.code_image, 0, sizeof(data.code_image));
    memset(data.data_image, 0, sizeof(data.data_image));
    printf("%d\n", handle_r_type_instruction(thing, &word_idx, name, &data, &ret));
    printf("%lu\n", ret);


    return 0;
}