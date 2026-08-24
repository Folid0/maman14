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
        fprintf(ob_file, " %02X", (unsigned int)image[cur_idx + byte_idx]);
    }
    fprintf(ob_file, "\n");
}

int write_ob_file(FILE *ob_file, const AssemblerData *data){
    int code_size;
    int data_size;
    int code_idx, byte_idx;

    if (ob_file == NULL || data == NULL) {
        return -1; /*Error*/
    }
    code_size = data->IC - 100; /* Assuming IC starts at 100 */
    data_size = data->DC;

    fprintf(ob_file, "%d %d\n", code_size, data_size);    

    for (code_idx = 0; code_idx < code_size; code_idx+=4) {
        fprintf(ob_file, "%04d", 100 + code_idx); /* writing the address */

        write_bytes_to_ob_file(ob_file, data->code_image, code_idx, code_size);
    }


    for (byte_idx = 0; byte_idx < data_size; byte_idx+=4) {
        fprintf(ob_file, "%04d", 100 + code_size + byte_idx); /* writing the address */

        write_bytes_to_ob_file(ob_file, data->data_image, byte_idx, data_size);
    }

    return 1;
}

/*closes a file safly*/
/*returns 1 if succsful and -1 if error*/
int close_file(FILE *file, const char *file_name) {
    if (file == NULL){
        return 1; 
    }
    if (fclose(file) != 0) {
        fprintf(stdout, "Error: Failed to close file %s\n", file_name);
        if (file_name != NULL) {
            if (remove(file_name) != 0) {
                fprintf(stdout, "Error: Failed to remove file %s\n", file_name);
            }
            return -1;
        }
        else {
            fprintf(stdout, "Error: file_name is NULL, cannot remove file\n");
            return -1;
        }
    }
    
    return 1;
}

/*returns 1 if successful and -1 if error*/
int close_files(FILE *am_file, FILE *ob_file, FILE *ext_file, FILE *ent_file, const char *am_file_name, const char *ob_file_name, const char *ext_file_name, const char *ent_file_name) {
    int error_flag = 0;
    if (am_file != NULL) {
        if (close_file(am_file, am_file_name) == -1) {
            error_flag = 1;
        }
    }
    if (ob_file != NULL) {
        if (close_file(ob_file, ob_file_name) == -1) {
            error_flag = 1;
        }
    }
    if (ext_file != NULL) {
        if (close_file(ext_file, ext_file_name) == -1) {
            error_flag = 1;
        }
    }
    if (ent_file != NULL) {
        if (close_file(ent_file, ent_file_name) == -1) {
            error_flag = 1;
        }
    }
    return error_flag == 0 ? 1 : -1;
    
}


/*deletes the files*/
/*first closes the files and then deletes them*/
int remove_files(FILE *am_file, FILE *ob_file, FILE *ext_file, FILE *ent_file,
     char *am_file_name, char *ob_file_name, char *ext_file_name, char *ent_file_name) {
    int error_flag = 0;
    if (am_file != NULL) {
        if (fclose(am_file) != 0) {
            fprintf(stdout, "Error: Failed to close intermediate file %s\n", am_file_name);
            error_flag = 1;
        }
    }
    if (am_file_name != NULL) {
        remove(am_file_name);
    }
    if (ob_file != NULL) {
        if (fclose(ob_file) != 0) {
            fprintf(stdout, "Error: Failed to close output file %s\n", ob_file_name);
            error_flag = 1;
        }
    }
    if (ob_file_name != NULL) {
        remove(ob_file_name);
    }

    if (ext_file != NULL) {
        if (fclose(ext_file) != 0) {
            fprintf(stdout, "Error: Failed to close external file %s\n", ext_file_name);
            error_flag = 1;
        }
    }
    if (ext_file_name != NULL) {
        remove(ext_file_name);
    }
    if (ent_file != NULL) {
        if (fclose(ent_file) != 0) {
            fprintf(stdout, "Error: Failed to close entry file %s\n", ent_file_name);
            error_flag = 1;
        }
    }
    if (ent_file_name != NULL) {
        remove(ent_file_name);
    }
    return error_flag == 0 ? 1 : -1; /* success if no errors */
    
}


int write_ext_file(FILE **ext_file, char *ext_file_name, ExternUsageNode *extern_head) {
    ExternUsageNode *current = extern_head;
    
    if (ext_file == NULL || *ext_file == NULL || ext_file_name == NULL) {
        fprintf(stdout, "Error: ext_file is NULL or ext_file_name is NULL\n ");
        return -1;
    }

    if (extern_head == NULL) {
        if (fclose(*ext_file) != 0) {
            fprintf(stdout, "Error: Failed to close external file %s\n", ext_file_name);
            remove(ext_file_name);
            *ext_file = NULL; /* Set the pointer to NULL to indicate that the file is closed */
            return -1;
        }
        if (remove(ext_file_name) != 0) { /*removing existing file, to make sure that if the extern_head is an
        empty list there wont be any ext files from previos runs*/
            fprintf(stdout, "Error: Failed to remove external file %s\n", ext_file_name);
            *ext_file = NULL; /* Set the pointer to NULL to indicate that the file is closed */
            return -1;
        }
        *ext_file = NULL; /* Set the pointer to NULL to indicate that the file is closed */
        return 1; /* No extern symbols to write */
    }
    
    while (current != NULL) {
        if (fprintf(*ext_file, "%s %04ld\n", current->name, current->address) < 0) {
            fprintf(stdout, "Error: Failed to write to external file %s\n", ext_file_name);
            return -1;
        }
        current = current->next;
    }

    return 1; /* success */
}

int write_ent_file(FILE **ent_file, char *ent_file_name, LabelNode *label_head) {
    LabelNode *current = label_head;
    int entry_exist = 0;

    if (ent_file == NULL || *ent_file == NULL ||ent_file_name == NULL) {
        fprintf(stdout, "Error: ent_file is NULL or ent_file_name is NULL\n ");
        return -1;
    }

    if (label_head == NULL) {
        if (fclose(*ent_file) != 0) {
            fprintf(stdout, "Error: Failed to close entry file %s\n", ent_file_name);
            remove(ent_file_name);
            *ent_file = NULL; /* Set the pointer to NULL to indicate that the file is closed */
            return -1;
        }
        if (remove(ent_file_name) != 0) { /*removing existing file, to make sure that if the label_head is an
        empty list there wont be any ent files from previos runs*/
            fprintf(stdout, "Error: Failed to remove entry file %s\n", ent_file_name);
            *ent_file = NULL; /* Set the pointer to NULL to indicate that the file is closed */
            return -1;
        }
        *ent_file = NULL; /* Set the pointer to NULL to indicate that the file is closed */
        return 1; /* No entry symbols to write */
    }
    
    while (current != NULL) {
        if (current->is_entry) {
            entry_exist = 1;
            if (fprintf(*ent_file, "%s %04ld\n", current->name, current->address) < 0) {
                fprintf(stdout, "Error: Failed to write to entry file %s\n", ent_file_name);
                return -1;
            }
        }
        current = current->next;
    }

    if (entry_exist == 0) {
        /* No entry symbols to write, remove the file */
        return remove_file(ent_file, ent_file_name);
    }
    return 1; /* success */
}


int remove_file(FILE **file, char *file_name) {
    if (file == NULL || *file == NULL || file_name == NULL) {
        fprintf(stdout, "Error: file is NULL or file_name is NULL\n");
        return -1;
    }
    if (close_file(*file, file_name) != 1) {
        fprintf(stdout, "Error: Failed to close file %s\n", file_name);
        remove(file_name);
        *file = NULL; 
        return -1;
    }
    *file = NULL; /* Set the pointer to NULL to indicate that the file is closed */

    if (remove(file_name) != 0) { 
        fprintf(stdout, "Error: Failed to remove file %s\n", file_name);
        *file = NULL; /* Set the pointer to NULL to indicate that the file is closed */
        return -1;
    }
    return 1; /* success */
}