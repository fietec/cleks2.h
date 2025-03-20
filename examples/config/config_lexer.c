#define CLEKS_IMPLEMENTATION
#include "../../cleks2.h"
#include "../utils.h"

typedef enum {
    CONFIG_SEP,
    CONFIG_NEW_LINE
} ConfigSymbols;

CleksSymbol config_symbols[] = {
    [CONFIG_SEP] = '=',
    [CONFIG_NEW_LINE] = '\n'
};

const char* const config_symbol_names[] = {
    [CONFIG_SEP] = "Seperator",
    [CONFIG_NEW_LINE] = "NewLine"
};

CleksString config_strings[] = {{'"', '"'}};
CleksWhitespace config_whitespaces[] = {' '};

void config_print(CleksToken token)
{
    if (token.loc.filename != NULL) printf("%s:", token.loc.filename);
	CleksTokenType type = cleks_token_type(token.id);
    CleksTokenIndex index = cleks_token_index(token.id);
    bool keep_unknown = (type == CLEKS_UNKNOWN && index == 1);
	printf("%d:%d %s: ", token.loc.row, token.loc.column, (keep_unknown)? "":cleks_token_type_name(type));
	cleks_assert(type < CLEKS_TOKEN_TYPE_COUNT, "Invalid token type: %u!", type);
	switch(type){
		case CLEKS_SYMBOL: printf("%s: '%.*s'", config_symbol_names[index], token.end-token.start, token.start); break;
		case CLEKS_STRING: printf("\"%.*s\"", token.end-token.start, token.start); break;
		case CLEKS_INTEGER:
		case CLEKS_FLOAT: 
		case CLEKS_HEX: 
		case CLEKS_BIN: printf("%.*s", token.end-token.start, token.start); break;
		case CLEKS_UNKNOWN: {
            if (keep_unknown){printf("%.*s", token.end-token.start, token.start);}
            else {printf("`%.*s`", token.end-token.start, token.start);}
        } break;
		default: cleks_error("Uninplemented type in print: %s", cleks_token_type_name(type)); exit(1);
	}
    putchar('\n');
}

CleksConfig config_config = {
    .symbols = config_symbols,
    .symbol_count = CLEKS_ARR_LEN(config_symbols),
    .strings = config_strings,
    .string_count = CLEKS_ARR_LEN(config_strings),
    .whitespaces = config_whitespaces,
    .whitespace_count = CLEKS_ARR_LEN(config_whitespaces),
    .flags = CLEKS_FLAGS_ALL_NUMS,
    .print_fn = config_print
};

int main(int argc, char **argv)
{
    cleks_assert(argc > 1, "No input file provided!");
    char *filename = argv[1];
    char *content = read_entire_file(filename);
    cleks_assert(content != NULL, "Failed to read file: %s", filename);
    printf("\"%s\"\n", content);
    Clekser clekser = Cleks_create(content, strlen(content), config_config, filename);
    
    CleksToken token;
    while (Cleks_next(&clekser, &token)){
        Cleks_print(clekser, token);
    }
    cleks_info("finished");
    free(content);
    return 0;
}
