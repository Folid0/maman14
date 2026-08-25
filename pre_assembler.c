#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "consts.h"
#include "utils.h"
#include "macro_table.h"
#include "pre_assembler.h"
#include "reserved_word.h"

/*returns -1 if error occurred, 1 if successful*/
int run_pre_assembler(MacroNode **macro_head_ret, char *file_name_as, char *file_name_am, ErrorInfo *error_info) {
    int error_flag = 0; /*1 if there is an error*/
    int is_mcro_val;
    char cur_line[MAX_LINE_LEN];
    FILE *input_file_as;
    FILE *output_file_am;
    int line_idx = 0;
    char mcro_name[MAX_MACRO_NAME_LEN];
    int error_value; /*used to store the return value of functions that may have MEMORY ALLOCATION ERROR*/
    MacroNode *mcro_node_head = NULL; /*pointer to head of the macro list*/
    int input_file_close_status = 0;
    int output_file_close_status = 0;


    input_file_as = fopen(file_name_as, "r");
    /* Open the input file for reading */
    if (input_file_as == NULL) {
        report_errorf(error_info, "Could not open input file %s for reading.", file_name_as);
        return -1; /*file opening failed*/
    }

    output_file_am = fopen(file_name_am, "w");
    /* Open the output file for writing */
    if (output_file_am == NULL) {
        report_errorf(error_info, "Could not open output file %s for writing.", file_name_am);
        fclose(input_file_as);
        return -1; /*file opening failed*/
    }
    
    while (fgets(cur_line, sizeof(cur_line), input_file_as) != NULL){
        line_idx++;
        set_error_line(error_info, line_idx);
        
        if (strchr(cur_line, '\n') == NULL && !feof(input_file_as)) {
            report_errorf(error_info, "Line exceeds maximum allowed length (%d characters).",
                    MAX_LINE_LEN - 2); /* -2 to account for newline and null terminator */
            error_flag = 1;
            /* Skip the rest of the line to avoid processing it */
            flush_line(input_file_as);
        }
        else{
            /* Process the current line */
            is_mcro_val = get_macro_initialization_name_from_line(cur_line, mcro_name, error_info);
            if (is_mcro_val == -1){
                /*there is an error with the mcro*/
                report_error(error_info, "Macro error.");
                error_flag = 1;
            }
            else if(is_mcro_val == 1){ /*its a mcro*/
                error_value = add_macro(&mcro_node_head, mcro_name, error_info);
                if (error_value == MEMORY_ALLOCATION_ERROR){
                    report_error(error_info, "Memory allocation error while adding macro.");
                    error_flag = 1;
                    fclose(input_file_as); /*closing files*/
                    fclose(output_file_am);
                    free_macro_table(mcro_node_head); /*free the mcro list*/
                    remove(file_name_am); /*remove the output file*/
                    return MEMORY_ALLOCATION_ERROR; /*memory allocation failed*/
                }
                else if (error_value == -1){ /*there is an error with the mcro*/
                    report_error(error_info, "Macro error.");
                    error_flag = 1;
                }
                else{ /*no problem with adding macro to the list*/
                    error_value = add_lineblock_to_macro(mcro_node_head, cur_line, input_file_as, &line_idx, error_info);
                    if (error_value == MEMORY_ALLOCATION_ERROR){
                        report_error(error_info, "Memory allocation error while reading macro.");
                        error_flag = 1;
                        fclose(input_file_as); /*closing files*/
                        fclose(output_file_am);
                        free_macro_table(mcro_node_head); /*free the mcro list*/
                        remove(file_name_am); /*remove the output file*/
                        return MEMORY_ALLOCATION_ERROR; /*memory allocation failed*/
                    }
                    else if (error_value == -1){
                        report_error(error_info, "Error adding line block to macro.");
                        error_flag = 1;
                    }
                }
            }
            else if (is_mcro_val == 0){ /*its not a mcro and there is no error*/
                if (put_line(output_file_am, cur_line, mcro_node_head, error_info) == -1){
                    report_error(error_info, "Error putting line to output file.");
                    error_flag = 1;
                }
            }
        }
    }



    
    /*if there is an error anywhere, return -1*/
    if (error_flag == 1) {
        fclose(input_file_as); /*closing files*/
        fclose(output_file_am);

        free_macro_table(mcro_node_head); /*free the mcro list*/
        remove(file_name_am); /*remove the output file*/
        return -1; /*error*/
    }
    
    set_error_line(error_info, 0);

    /*closing files*/
    input_file_close_status = fclose(input_file_as);
    output_file_close_status = fclose(output_file_am);

    /*checking file close errors*/
    if (input_file_close_status != 0) {
        report_errorf(error_info, "Failed to close input file %s.", file_name_as);
        free_macro_table(mcro_node_head);
        remove(file_name_am); /*remove the output file*/
        return -1; /*error*/
    }
    if (output_file_close_status != 0) {
        report_errorf(error_info, "Failed to close output file %s.", file_name_am);
        free_macro_table(mcro_node_head);
        remove(file_name_am); /*remove the output file*/
        return -1; /*error*/
    }

    *macro_head_ret = mcro_node_head; /*setting the head of the ret node to the head of the macro list*/

    return 1; /*successful*/
}
