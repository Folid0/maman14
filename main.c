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

int run_on_files(int num_of_files, char *file_paths[]) {
    int max_file_path_len = find_max_file_path_length(file_paths, num_of_files);
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


}


/*runs on a single file, calls pre_assembler, first_pass, and second_pass*/
/*returns 1 if successful, -1 if an error occurred*/
int run_file(char *file_path, int max_file_path_len) {
    MacroNode *macro_head = NULL;
    int error_flag = 0; /*1 means error, 0 ok*/
    AssemblerData data;
    FILE *am_file;
    ExternUsageNode *extern_head = NULL; 
    char *base_file_name = (char *) malloc(max_file_path_len);
    char *am_file_name = (char *) malloc(max_file_path_len);


    if (base_file_name == NULL || am_file_name == NULL) {
        fprintf(stderr, "Memory allocation failed for file name buffers\n");
        free(base_file_name);
        free(am_file_name);
        return -1; 
    }

    if (run_pre_assembler(file_path, &macro_head) != 1) {
        fprintf(stdout, "Error: Pre-assembler failed for file %s\n", file_path);
        error_flag = 1;
        free(base_file_name);
        free(am_file_name); /*the macro is allready being freed when if error accours in the pre_assmbler*/
        return -1;
    }
    add_file_extension(base_file_name, ".am", am_file_name, max_file_path_len);

    am_file = fopen(am_file_name, "r");
    if (run_first_pass(am_file_name, &data, macro_head) != 1) {
        fprintf(stdout, "Error: First pass failed for file %s\n", file_path);
        error_flag = 1;
        
        free(base_file_name);
        free(am_file_name);
        free_macro_table(macro_head);
        free_label_table(data.label_head);
        fclose(am_file);
        return -1;
    }
    else{
        if (run_second_pass(am_file_name, &data, &extern_head) != 1) {
            fprintf(stdout, "Error: Second pass failed for file %s\n", file_path);
            error_flag = 1;
            free(base_file_name);
            free(am_file_name);
            fclose(am_file);
            free_macro_table(macro_head);
            free_label_table(data.label_head);
            free_extern_usage_table(extern_head);
            return -1;
        }




    }
}


    free(base_file_name);
    free(am_file_name);
    fclose(am_file);
    free_macro_table(macro_head);
    free_label_table(data.label_head);
    free_extern_usage_table(extern_head);
    return error_flag == 1 ? -1 : 1; /*return 1 if successful, -1 if an error occurred*/
}

int main(int argc, char *argv[]) {

    
    













    AssemblerData data;
    FILE *am_file;
    int i;
    MacroNode *macro_head = NULL; /* Initialize macro_head to NULL */
    ExternUsageNode *extern_head = NULL; /* Initialize extern_head to NULL */
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
    
    printf("%d\n", run_second_pass(am_file, &data, &extern_head));
    
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
    fclose(am_file);
    return 0;
}