#include <stdio.h>
#include "macro_table.h"
#include "utils.h"
#include <stdlib.h>
#include "pre_assembler.h"
#include "writing_to_binary.h"
#include "first_pass.h"
#include "second_pass.h"
#include "data_table.h"
#include "file_utils.h"
#include "main.h"

#include <string.h>

/*runs on the file list and assembles each one*/
/*returns 1 if successful, -1 if an error occurred that means we cant run on all the files*/
/*if an error occurs in just one file, we keep runing on the other ones*/
int run_on_files(int num_of_files, char *file_paths[]) {
    int max_file_path_len;
    int i;
    int error_flag = 0; /*flag to indicate if there was an error in any file*/
    int cur_run_value; /*used to store the return value of run_file*/
    ErrorInfo error_info;

    initialize_error_info(&error_info, NULL);
    max_file_path_len = find_max_file_path_length(file_paths, num_of_files, &error_info);

    if (max_file_path_len == 0) { /*error*/
        return -1;
    }
    max_file_path_len += 2;  /* +2 for the null terminator */

    for (i = 0; i < num_of_files; i++) {
        if (file_paths[i] == NULL) {
            report_errorf(&error_info, "file_paths[%d] is NULL", i);
        }
        else{
            initialize_error_info(&error_info, file_paths[i]);
            cur_run_value = run_file(file_paths[i], max_file_path_len);
            if (cur_run_value == MEMORY_ALLOCATION_ERROR) {
                report_error(&error_info, "Memory allocation failed while processing file");
                return MEMORY_ALLOCATION_ERROR; /* Stop processing further files */
            }
            else if (cur_run_value != 1) {
                report_error(&error_info, "Failed to assemble file");
                error_flag = 1; /* Set the error flag but continue processing other files */
            }
        }
    }
    return (error_flag == 0) ? 1 : -1;

}


/*runs on a single file, calls pre_assembler, first_pass, and second_pass*/
/*returns 1 if successful, -1 if an error occurred*/
int run_file(char *file_path, int max_file_path_len) {
    MacroNode *macro_head = NULL;
    ExternUsageNode *extern_head = NULL; 
    AssemblerData data;
    ErrorInfo error_info;
    int error_value; /*used to store the return value of functions that may have MEMORY ALLOCATION ERROR*/
    FILE *am_file = NULL, *ob_file = NULL, *ext_file = NULL, *ent_file = NULL;
    char *ext_file_name = (char *) malloc(max_file_path_len);
    char *ent_file_name = (char *) malloc(max_file_path_len);
    char *base_file_name = (char *) malloc(max_file_path_len);
    char *am_file_name = (char *) malloc(max_file_path_len);
    char *ob_file_name = (char *) malloc(max_file_path_len);

    initialize_error_info(&error_info, file_path);

    if (initialize_assembler_data(&data, &error_info) == -1) {
        report_error(&error_info, "Failed to initialize assembler data");
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1; /* Error */
    }

    if (base_file_name == NULL || am_file_name == NULL || ob_file_name == NULL || ext_file_name == NULL || ent_file_name == NULL) {
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return MEMORY_ALLOCATION_ERROR; /*Error*/
    }

    /*extract base file name and create file names with extensions */
    if (extract_file_name_without_extension(file_path, base_file_name, max_file_path_len, &error_info) == -1 ||
     add_file_extension(base_file_name, ".am", am_file_name, max_file_path_len, &error_info) == -1||
     add_file_extension(base_file_name, ".ob", ob_file_name, max_file_path_len, &error_info) == -1||
     add_file_extension(base_file_name, ".ext", ext_file_name, max_file_path_len, &error_info) == -1||
     add_file_extension(base_file_name, ".ent", ent_file_name, max_file_path_len, &error_info) == -1
    ) {
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1;
    }

    error_value = run_pre_assembler(&macro_head, file_path, am_file_name, &error_info);
    if (error_value != 1) {
        set_error_line(&error_info, 0);
        report_error(&error_info, "Pre-assembler failed for file");
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return error_value;
    }

    am_file = fopen(am_file_name, "r");
    if (am_file == NULL) {
        report_errorf(&error_info, "Could not open intermediate file %s for reading.", am_file_name);
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1; 
    }

    error_value = run_first_pass(am_file, &data, macro_head, &error_info);
    if (error_value != 1) {
        set_error_line(&error_info, 0);
        report_error(&error_info, "First pass failed for file");
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return error_value;
    }

    error_value = run_second_pass(am_file, &data, &extern_head, &error_info);
    if (error_value != 1) {
        set_error_line(&error_info, 0);
        report_error(&error_info, "Second pass failed for file");
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return error_value;
    }

    set_error_line(&error_info, 0);
    ob_file = fopen(ob_file_name, "w");
    if (ob_file == NULL) {
        set_error_line(&error_info, 0);
        report_errorf(&error_info, "Could not open output file %s for writing.", ob_file_name);
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1; 
    }
    
    if (write_ob_file(ob_file, &data, &error_info) != 1) {
        report_errorf(&error_info, "Writing to output file %s failed", ob_file_name);
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1;
    }

    ext_file = fopen(ext_file_name, "w");
    if (ext_file == NULL) {
        report_errorf(&error_info, "Could not open external file %s for writing.", ext_file_name);
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1; 
    }
    
    if (write_ext_file(&ext_file, ext_file_name, extern_head, &error_info) != 1) {
        report_errorf(&error_info, "Writing to external file %s failed", ext_file_name);
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1;
    }

    ent_file = fopen(ent_file_name, "w");
    if (ent_file == NULL) {
        report_errorf(&error_info, "Could not open entry file %s for writing.", ent_file_name);
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1; 
    }
    if (write_ent_file(&ent_file, ent_file_name, data.label_head, &error_info) != 1) {
        report_errorf(&error_info, "Writing to entry file %s failed", ent_file_name);
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1;
    }

    if (close_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info) != 1) {
        report_error(&error_info, "Failed to close files.");
        remove_files(am_file, ob_file, ext_file, ent_file, am_file_name, ob_file_name, ext_file_name, ent_file_name, &error_info); /* Remove the files that were allready created */
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
        return -1;
    }
    free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name, ext_file_name, ent_file_name);
    return 1; /* Success */
}

/*the main function*/
/*returns 1 for success, -2 for MEMORY ALLOCATION ERROR*/
int main(int argc, char *argv[]) {
    int result;
    if (argc < 2) { /*check if any input files are provided*/
        report_error(NULL, "No input files provided");
        return 1;
    }

    /*run on all of the files*/
    result = run_on_files(argc - 1, argv + 1); /* Skip the program name */

    return result == 1 ? 0 : 1; /* Return 0 for success, 1 for error */
}
