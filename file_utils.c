#include "file_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "consts.h"
#include "data_table.h"

/*gets a list of file paths and returns the length of the longest file path */
/*returns 0 if the input is invalid */
size_t find_max_file_path_length(char *file_paths[], int num_files) {
    size_t max;
    size_t cur_len;
    int i;

    if (file_paths == NULL || num_files <= 0) {
        fprintf(stdout, "Error: Invalid input to find_max_file_path_length\n");
        return 0; 
    }

    max = 0;

    for (i = 0; i < num_files; i++) {
        if (file_paths[i] == NULL) {
            fprintf(stdout, "Error: file_paths[%d] is NULL\n", i);
        }
        else{
            cur_len = strlen(file_paths[i]);
            if (cur_len > max) {
                max = cur_len;
            }
        }
    }

    return max;
}

int extract_file_name_without_extension(const char *file_path, char *output_name, int max_file_name_len) {
    size_t file_path_len;
    size_t output_name_len;

    if (file_path == NULL|| output_name == NULL) {
        fprintf(stdout, "Error: failed to extract file name\n");
        return -1;
    }

    if (max_file_name_len <= 0) {
        fprintf(stdout, "Error: max_file_name_len must be greater than 0\n");
        return -1;
    }
    file_path_len = strlen(file_path);

    if (file_path_len <= 3){
        fprintf(stdout, "Error: file path is too short\n");
        return -1;
    }

    if (strcmp(file_path + file_path_len - 3, ".as") != 0) {
        fprintf(stdout, "Error: file path \"%s\" does not end with .as\n", file_path);
        return -1;
    }

    output_name_len = file_path_len - 3;

    if (output_name_len +1  > (size_t)max_file_name_len) {
        fprintf(stdout, "Error: output_name string is too small\n");
        return -1;
    }

    memcpy(output_name, file_path, output_name_len);
    output_name[output_name_len] = '\0'; 

    return 1;
}

int add_file_extension(char *file_name_no_extension, char *extension, char *output_path, int max_file_path_len){
    size_t file_name_len;
    size_t extension_len;

    if (file_name_no_extension == NULL || extension == NULL || output_path == NULL) {
        fprintf(stdout, "Error: Invalid input to add_file_extension\n");
        return -1;
    }

    if (max_file_path_len <= 0) {
        fprintf(stdout, "Error: max_file_path_len must be greater than 0\n");
        return -1;
    }

    file_name_len = strlen(file_name_no_extension);
    extension_len = strlen(extension);


        if (file_name_len + extension_len + 1 > (size_t)max_file_path_len) {
        fprintf(stdout, "Error: Output path buffer is too small.\n");
        return -1;
    }

    memcpy(output_path, file_name_no_extension, file_name_len);
    memcpy(output_path + file_name_len, extension, extension_len);
    output_path[file_name_len + extension_len] = '\0';

    return 1;
}

void write_bytes_to_ob_file(FILE *ob_file, const unsigned char *image, int cur_idx, int total_size) {
    int byte_idx;
    for (byte_idx = 0; byte_idx < 4 && cur_idx + byte_idx < total_size; byte_idx++) {
        fprintf(ob_file, " %02X ", image[cur_idx + byte_idx]);
    }
    fprintf(ob_file, "\n");
}

int write_ob_file(FILE *ob_file, const AssemblerData *data){
    int code_size = data->IC - 100; /* Assuming IC starts at 100 */
    int data_size = data->DC;

    int code_idx, byte_idx;

    fprintf(ob_file, "%d %d\n", code_size, data_size);

    if (ob_file == NULL) {
        fprintf(stderr, "Error: the file is NULL\n");
        return -1;
    }
    

    for (code_idx = 0; code_idx < code_size; code_idx+=4) {
        fprintf(ob_file, "%04d", 100 + code_idx); /* writing the address */

        write_bytes_to_ob_file(ob_file, data->code_image, code_idx, code_size);
    }


    for (byte_idx = 0; byte_idx < data_size; byte_idx+=4) {
        fprintf(ob_file, "%04d", 100 + code_size + byte_idx); /* writing the address */

        write_bytes_to_ob_file(ob_file, data->data_image, byte_idx, data_size);
    }
    fprintf(ob_file, "\n");

    return 1;
}

