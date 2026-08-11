#include <stdio.h>
#include "macro_table.h"
#include "utils.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
    /*this is all for testing*/
    char* line1 = "hellojkolj world";
    char* word = (char*)malloc(sizeof(82));
    int idx = 0;

    
    /*should maybe add /0 */
    printf("%d\n", get_next_word(line1, &idx, word));
    printf("%s\n", word);
    idx = skip_whitespace(line1, idx);
    printf("%d\n", get_next_word(line1, &idx, word));
    printf("%s\n", word);
    return 0;
}