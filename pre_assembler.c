#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"
#include "macro_table.h"
#include "pre_assembler.h"


/*returns 0 if error occurred, 1 if successful*/
int run_pre_assembler(char *file_name){
    char file_name_as[Max_FILE_NAME_LEN];
    char file_name_am[Max_FILE_NAME_LEN];

    char cur_line[MAX_LINE_LEN];
    FILE *input_file_as;
    int idx = 0;
    /* Create the output file names, with as and am extensions */
    sprintf(file_name_as, "%s.as", file_name);
    sprintf(file_name_am, "%s.am", file_name);

    input_file_as = fopen(file_name_as, "r");
    /* Open the input file for reading */
    if (input_file_as == NULL) {
        fprintf(stderr, "Error: Could not open input file %s for reading.\n", file_name_as);
        return 0; /*file opening failed*/
    }

    while (fgets(cur_line, sizeof(cur_line), input_file_as) != NULL){
        printf("%s", cur_line);

        idx++;
    }

    /*dont forger to close the file*/

    return 1; /*successful*/
}