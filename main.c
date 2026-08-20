#include <stdio.h>
#include "macro_table.h"
#include "utils.h"
#include <stdlib.h>
#include "pre_assembler.h"
#include "writing_to_binary.h"
#include "first_pass.h"
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
    AssemblerData data;
    FILE *am_file;
    int i;
    MacroNode *macro_head = NULL; /* Initialize macro_head to NULL */
    run_pre_assembler("test", &macro_head);

    am_file = fopen("test.am", "r");
    /*
    char thing[MAX_LINE_LEN] = "add $1, $1, $1";
    int word_idx = 3;
    char name[MAX_LINE_LEN] = "add";
    */

    data.IC = 100; /* Standard starting IC address for the project */
    data.DC = 0;
    data.label_head = NULL;
    data.error_flag = 0;
    memset(data.code_image, 0, sizeof(data.code_image));
    memset(data.data_image, 0, sizeof(data.data_image));


    printf("%d\n", run_first_pass(am_file, &data, macro_head));
    
    fclose(am_file);

    
    printf("Code Image (Hex):\n");
    /* Loop through the code image. The size is IC - 100 */
    for (i = 0; i < data.IC - 100; i++) {
        printf("%02X ", data.code_image[i]);
        
        /* Print a newline every 4 bytes (1 instruction) for easy reading */
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }

    printf("\nData Image (Hex):\n");
    /* Loop through the data image. The size is DC */
    for (i = 0; i < data.DC; i++) {
        printf("%02X ", data.data_image[i]);
        
        /* Print a newline every 4 bytes */
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    free_macro_table(macro_head); /* Free the macro list after use */
    return 0;
}