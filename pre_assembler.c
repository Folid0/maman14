#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"
#include "macro_table.h"
#include "pre_assembler.h"
#include "reserved_word.h"

/*returns 0 if error occurred, 1 if successful*/
int run_pre_assembler(char *file_name, MacroNode **macro_head_ret) {
    char file_name_as[MAX_FILE_NAME_LEN];
    char file_name_am[MAX_FILE_NAME_LEN];
    int error_flag = 0; /*1 if there is an error*/
    int is_mcro_val;
    char cur_line[MAX_LINE_LEN];
    FILE *input_file_as;
    FILE *output_file_am;
    int line_idx = 0;
    char mcro_name[MAX_MACRO_NAME_LEN];

    MacroNode *mcro_node_head = NULL; /*pointer to head of the macro list*/


    /* Create the output file names, with as and am extensions */
    sprintf(file_name_as, "%s.as", file_name);
    sprintf(file_name_am, "%s.am", file_name);

    input_file_as = fopen(file_name_as, "r");
    /* Open the input file for reading */
    if (input_file_as == NULL) {
        fprintf(stderr, "Error: Could not open input file %s for reading.\n", file_name_as);
        return 0; /*file opening failed*/
    }

    output_file_am = fopen(file_name_am, "w");
    /* Open the output file for writing */
    if (output_file_am == NULL) {
        fprintf(stderr, "Error: Could not open output file %s for writing.\n", file_name_am);
        fclose(input_file_as);
        return 0; /*file opening failed*/
    }
    
    while (fgets(cur_line, sizeof(cur_line), input_file_as) != NULL){

        is_mcro_val = get_macro_initialization_name_from_line(cur_line, mcro_name);
        if (is_mcro_val == -1){
            /*there is an error with the mcro*/
            fprintf(stderr, "mcro Errror at line: %d ", line_idx);
            error_flag = 1;
        }
        else if(is_mcro_val == 1 && error_flag == 0){ /*its a mcro and there is no error*/
            if (add_macro(&mcro_node_head, mcro_name) == -1){
                fprintf(stderr, "memmory allocation ERROR");
                error_flag = 1;
            }
            else{ /*no problem with adding macro to the list*/
                if (error_flag == 0){
                    if (add_lineblock_to_macro(mcro_node_head, cur_line, input_file_as, &line_idx) == -1){
                        fprintf(stderr, "memmory allocation ERROR");
                        error_flag = 1;
                    }
                }
            }
        }
        else if (is_mcro_val == 0 && error_flag == 0){ /*its not a mcro and there is no error*/
            if (put_line(output_file_am, cur_line, mcro_node_head) == -1){
                fprintf(stderr, "Error putting line to output file at line: %d", line_idx);
                error_flag = 1;
            }
        }
    }

        line_idx++;
    




    
    /*if there is an error anywhere, return -1*/
    if (error_flag == 1) {
        fclose(input_file_as); /*closing files*/
        fclose(output_file_am);

        free_macro_table(mcro_node_head); /*free the mcro list*/
        remove(file_name_am); /*remove the output file*/
        return -1; /*error*/
    }

    /*closing files*/
    fclose(input_file_as);
    fclose(output_file_am);

    *macro_head_ret = mcro_node_head; /*setting the head of the ret node to the head of the macro list*/

    /*

    free_macro_table(mcro_node_head); 

    */
   
    return 1; /*successful*/
}