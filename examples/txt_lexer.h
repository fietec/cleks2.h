#include <stdio.h>
#define CLEKS_IMPLEMENTATION
#include "../cleks2.h"

CleksSymbol txt_symbols[] = {',', '.', '!', '?', ':', ';'};
CleksWhitespace txt_whitespaces[] = {' ', '\n'};

CleksConfig txt_config = {
    .symbols = txt_symbols,
    .symbol_count = CLEKS_ARR_LEN(txt_symbols),
    .whitespaces = txt_whitespaces,
    .whitespace_count = CLEKS_ARR_LEN(txt_whitespaces)
};