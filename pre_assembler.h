#ifndef PRE_ASSMBLER_H
#define PRE_ASSMBLER_H
#include "macro_table.h"
/*returns -1 if error occurred, 1 if successful*/
int run_pre_assembler(MacroNode **macro_head_ret, char *file_name_as, char *file_name_am);


#endif /*PRE_ASSMBLER_H*/
