#define CLEKS_IMPLEMENTATION
#include "../cleks2.h"
#include "utils.h"

CleksWhitespace text_whitespaces[] = {' ', '\n'};
CleksSymbol text_symbols[] = {'.', ',', '?', '!', '"', ':', ';'};

CleksConfig text_config = {
    .symbols = text_symbols,
    .symbol_count = CLEKS_ARR_LEN(text_symbols),
    .whitespaces = text_whitespaces,
    .whitespace_count = CLEKS_ARR_LEN(text_whitespaces),
    .flags = CLEKS_FLAGS_KEEP_UNKNOWN
};

int main(int argc, char **argv)
{
    cleks_assert(argc >= 2, "No input file provided!");
    char *filename = argv[1];
    char *content = read_entire_file(filename);
    
    Clekser clekser = Cleks_create(content, strlen(content), text_config, filename, NULL);
    
    CleksToken token;
    while (Cleks_next(&clekser, &token)){
        Cleks_print(clekser, token);
    }
    
    free(content);
    return 0;
}