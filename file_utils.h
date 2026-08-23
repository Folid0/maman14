#ifndef FILE_UTILS_H
#define FILE_UTILS_H


int extract_file_name_without_extension(const char *file_path, char *output_name, int max_file_name_len);
size_t find_max_file_path_length(char *file_paths[], int num_files);
int add_file_extension(char *file_name_no_extension, char *extension, char *output_path, int max_file_path_len);
int write_ob_file(const char *ob_file_name, const AssemblerData *data);


#endif /* FILE_UTILS_H */