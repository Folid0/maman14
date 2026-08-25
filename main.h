#ifndef MAIN_H
#define MAIN_H

/*runs on a single file, calls pre_assembler, first_pass, and second_pass*/
/*returns 1 if successful, -1 if an error occurred*/
int run_file(char *file_path, int max_file_path_len);
/*runs on the file list and assembles each one*/
/*returns 1 if successful, -1 if an error occurred that means we cant run on all the files*/
/*if an error occurs in just one file, we keep runing on the other ones*/
int run_on_files(int num_of_files, char *file_paths[]);

#endif /* MAIN_H */
