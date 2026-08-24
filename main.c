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
    int max_file_path_len = find_max_file_path_length(file_paths, num_of_files) + 2; /* +2 for the null terminator */
    int i;

    if (max_file_path_len == 0) { /*error*/
        return -1;
    }
    
    for (i = 0; i < num_of_files; i++) {
        if (file_paths[i] == NULL) {
            fprintf(stdout, "Error: file_paths[%d] is NULL\n", i);
        }
        else{
            run_file(file_paths[i], max_file_path_len);
        }
    }
    return 1; /*success*/

}


/*runs on a single file, calls pre_assembler, first_pass, and second_pass*/
/*returns 1 if successful, -1 if an error occurred*/
int run_file(char *file_path, int max_file_path_len) {
    MacroNode *macro_head = NULL;
    AssemblerData data;
    FILE *am_file, *ob_file;
    ExternUsageNode *extern_head = NULL; 
    char *base_file_name = (char *) malloc(max_file_path_len);
    char *am_file_name = (char *) malloc(max_file_path_len);
    char *ob_file_name = (char *) malloc(max_file_path_len);

    if (initialize_assembler_data(&data) == -1) {
        fprintf(stderr, "Error: Failed to initialize assembler data\n");
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1; /* Error */
    }

    if (base_file_name == NULL || am_file_name == NULL || ob_file_name == NULL) {
        fprintf(stderr, "Memory allocation failed for file name buffers\n");
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1; /*Error*/
    }

    if (extract_file_name_without_extension(file_path, base_file_name, max_file_path_len) == -1) {
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1;
    }

    if (add_file_extension(base_file_name, ".am", am_file_name, max_file_path_len) == -1) {
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1;
    }

    if (add_file_extension(base_file_name, ".ob", ob_file_name, max_file_path_len) == -1) {
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1;
    }

    if (run_pre_assembler(&macro_head, file_path, am_file_name) != 1) {
        fprintf(stdout, "Error: Pre-assembler failed for file %s\n", file_path);
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1;
    }

    am_file = fopen(am_file_name, "r");
    if (am_file == NULL) {
        fprintf(stdout, "Error: Could not open intermediate file %s for reading.\n", am_file_name);
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1; 
    }

    if (run_first_pass(am_file, &data, macro_head) != 1) {
        fprintf(stdout, "Error: First pass failed for file %s\n", file_path);
        
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        fclose(am_file);
        return -1;
    }

    if (run_second_pass(am_file, &data, &extern_head) != 1) {
        fprintf(stdout, "Error: Second pass failed for file %s\n", file_path);
        fclose(am_file);
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1;
    }

    ob_file = fopen(ob_file_name, "w");
    if (ob_file == NULL) {
        fprintf(stdout, "Error: Could not open output file %s for writing.\n", ob_file_name);
        fclose(am_file);
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1; 
    }

    if (write_ob_file(ob_file, &data) != 1) {
        fprintf(stdout, "Error: Writing to output file %s failed\n", ob_file_name);
        fclose(am_file);
        fclose(ob_file);
        free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
        return -1;
    }

    /*you still need to make the other ouput files, i belive there are 2 left*/

    free_everything(&data, macro_head, extern_head, am_file_name, base_file_name, ob_file_name);
    fclose(am_file);
    fclose(ob_file);
    return 1; /* Success */
}

int main(int argc, char *argv[]) {
    /*run on all of the files*/
    run_on_files(argc - 1, argv + 1); /* Skip the program name */

    return 0;
}