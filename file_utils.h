#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include "data_table.h"

int extract_file_name_without_extension(const char *file_path, char *output_name, int max_file_name_len);
size_t find_max_file_path_length(char *file_paths[], int num_files);
int add_file_extension(char *file_name_no_extension, char *extension, char *output_path, int max_file_path_len);

int write_ob_file(FILE *ob_file, const AssemblerData *data);
int write_ext_file(FILE **ext_file, char *ext_file_name, ExternUsageNode *extern_head);
int write_ent_file(FILE **ent_file, char *ent_file_name, LabelNode *label_head);

int close_files(FILE *am_file, FILE *ob_file, FILE *ext_file, FILE *ent_file,
     const char *am_file_name, const char *ob_file_name, const char *ext_file_name, const char *ent_file_name);
int remove_files(FILE *am_file, FILE *ob_file, FILE *ext_file, FILE *ent_file, char *am_file_name, char *ob_file_name, char *ext_file_name, char *ent_file_name);
int remove_file(FILE **file, char *file_name);

#endif /* FILE_UTILS_H */