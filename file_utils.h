#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "data_table.h"

/*gets file path with an as extension, puts the base file path without the extension into output_name*/
/*returns 1 if successful, -1 if error*/
int extract_file_name_without_extension(const char *file_path, char *output_name, int max_file_name_len, ErrorInfo *error_info);

/*gets a list of file paths and returns the length of the longest file path */
/*returns 0 if the input is invalid */
size_t find_max_file_path_length(char *file_paths[], int num_files, ErrorInfo *error_info);

/*adds extension to a file name, puts it in output_path*/
/*returns 1 if successful, -1 if error*/
int add_file_extension(char *file_name_no_extension, char *extension, char *output_path, int max_file_path_len, ErrorInfo *error_info);


/*writes the .ob file*/
/*returns 1 if successful, -1 if error*/
int write_ob_file(FILE *ob_file, const AssemblerData *data, ErrorInfo *error_info);

/*writes the .ext file*/
/*returns 1 if successful, -1 if error*/
int write_ext_file(FILE **ext_file, char *ext_file_name, ExternUsageNode *extern_head, ErrorInfo *error_info);

/*writes the .ent file*/
/*returns 1 if successful, -1 if error*/
int write_ent_file(FILE **ent_file, char *ent_file_name, LabelNode *label_head, ErrorInfo *error_info);


/*returns 1 if successful and -1 if error*/
int close_files(FILE *am_file, FILE *ob_file, FILE *ext_file, FILE *ent_file,
     const char *am_file_name, const char *ob_file_name, const char *ext_file_name, const char *ent_file_name, ErrorInfo *error_info);

/*deletes the files*/
/*first closes the files and then deletes them*/
int remove_files(FILE *am_file, FILE *ob_file, FILE *ext_file, FILE *ent_file, char *am_file_name, char *ob_file_name, char *ext_file_name, char *ent_file_name, ErrorInfo *error_info);

/*removes the file from the system*/
/*returns 1 if successful, -1 if error*/
int remove_file(FILE **file, char *file_name, ErrorInfo *error_info);

#endif /* FILE_UTILS_H */
