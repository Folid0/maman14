#ifndef ERROR_INFORMATION_H
#define ERROR_INFORMATION_H

typedef struct ErrorInfo { /*saves tje error information so we will be able to print it later*/
    const char *file_name;
    int line_idx;
} ErrorInfo;

/* Starts an error context */
/* A line_idx 0 means no line yet. */
void initialize_error_info(ErrorInfo *error_info, const char *file_name);

/* Updates the source line associated with future errors */
void set_error_line(ErrorInfo *error_info, int line_idx);

/* Reports a message that does not require formatting arguments. */
void report_error(const ErrorInfo *error_info, const char *message);

/* Reports a printf-style formatted message with a number of arguments */
void report_errorf(const ErrorInfo *error_info, const char *format, ...);

#endif /* ERROR_INFORMATION_H */
