#include <stdio.h>
#include <stdarg.h>

#include "error_information.h"

/* Prints the common part that appears before every error message */
void print_error_constant(const ErrorInfo *error_info)
{
    if (error_info != NULL && error_info->file_name != NULL &&
        error_info->file_name[0] != '\0') {
        fprintf(stdout, "%s", error_info->file_name);

        if (error_info->line_idx > 0) {
            fprintf(stdout, ":%d", error_info->line_idx);
        }

        fprintf(stdout, ": error: ");
    }
    else if (error_info != NULL && error_info->line_idx > 0) {
        fprintf(stdout, "line %d: error: ", error_info->line_idx);
    }
    else {
        fprintf(stdout, "error: ");
    }
}

/* Starts an error context */
/* A line_idx 0 means no line yet. */
void initialize_error_info(ErrorInfo *error_info, const char *file_name)
{
    if (error_info != NULL) {
        error_info->file_name = file_name;
        error_info->line_idx = 0;
    }
    else {
        report_error(NULL, "error_info is NULL in initialize_error_info");
    }
}

/* updates the source line associated with future errors. */
void set_error_line(ErrorInfo *error_info, int line_idx)
{
    if (error_info != NULL) {
        error_info->line_idx = line_idx;
    }
    else {
        report_error(NULL, "error_info is NULL in set_error_line");
    }
}

/* Reports a message that does not require formatting arguments. */
void report_error(const ErrorInfo *error_info, const char *message)
{
    print_error_constant(error_info); /*prints the first part of the error message*/

    if (message != NULL) {
        fprintf(stdout, "%s", message);
    }
    else {
        fprintf(stdout, "unknown error");
    }

    fprintf(stdout, "\n");
}

/* Reports an error message with a number of arguments */
void report_errorf(const ErrorInfo *error_info, const char *format, ...)
{
    va_list arguments;

    print_error_constant(error_info); /*prints the first part of the error message*/

    if (format != NULL) {
        va_start(arguments, format);
        vfprintf(stdout, format, arguments);
        va_end(arguments);
    }
    else {
        fprintf(stdout, "unknown error");
    }

    fprintf(stdout, "\n");
}
