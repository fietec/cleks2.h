#include <stdio.h>
#define CLEKS_IMPLEMENTATION
#include "../cleks2.h"
#include "utils.h"

CleksSymbol txt_symbols[] = {',', '.', '!', '?', ':', ';'};
CleksWhitespace txt_whitespaces[] = {' ', '\n'};

CleksConfig txt_config = {
    .symbols = txt_symbols,
    .symbol_count = CLEKS_ARR_LEN(txt_symbols),
    .whitespaces = txt_whitespaces,
    .whitespace_count = CLEKS_ARR_LEN(txt_whitespaces)
};

int main(int argc, char **argv)
{
    cleks_assert(argc > 1, "No input file provided!");
    char *filename = argv[1];
    char *content = read_entire_file(filename);
    cleks_assert(content != NULL, "Failed to read file: %s", filename);
    Clekser clekser = Cleks_create(content, strlen(content), txt_config, filename, NULL);
    
    CleksToken token;
    while (Cleks_next(&clekser, &token)){
        if (cleks_token_type(token.id) == CLEKS_UNKNOWN){
            Cleks_print(clekser, token);
        }
    }
    free(content);
    return 0;
}