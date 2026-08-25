#ifndef PRE_ASSMBLER_H
#define PRE_ASSMBLER_H
#include "macro_table.h"
#include "error_information.h"
/*returns -1 if error occurred, 1 if successful*/
int run_pre_assembler(MacroNode **macro_head_ret, char *file_name_as, char *file_name_am, ErrorInfo *error_info);


#endif /*PRE_ASSMBLER_H*/
