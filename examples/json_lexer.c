#define CLEKS_IMPLEMENTATION
#include "utils.h"
#include "cleks2.h"
#include <stdio.h>

typedef enum{
	JSON_TRUE,
	JSON_FALSE,
	JSON_NULL,
} JsonWord;

typedef enum{
	JSON_MAP_OPEN,
	JSON_MAP_CLOSE,
	JSON_ARRAY_OPEN,
	JSON_ARRAY_CLOSE,
	JSON_SEP,
	JSON_MAP_SEP
} JsonSymbol;

CleksWord JsonWords[] = {
	[JSON_TRUE] = "true",
	[JSON_FALSE] = "false",
	[JSON_NULL] = "null"
};

char* const JsonWordNames[] = {
	[JSON_TRUE] = "True",
	[JSON_FALSE] = "False",
	[JSON_NULL] = "Null"
};

CleksSymbol JsonSymbols[] = {
	[JSON_MAP_OPEN] = '{',
	[JSON_MAP_CLOSE] = '}',
	[JSON_MAP_SEP] = ':',
	[JSON_ARRAY_OPEN] = '[',
	[JSON_ARRAY_CLOSE] = ']',
	[JSON_SEP] = ','
};

char* const JsonSymbolNames[] = {
	[JSON_MAP_OPEN] = "MapOpen",
	[JSON_MAP_CLOSE] = "MapClose",
	[JSON_MAP_SEP] = "MapSep",
	[JSON_ARRAY_OPEN] = "ArrayOpen",
	[JSON_ARRAY_CLOSE] = "ArrayClose",
	[JSON_SEP] = "Sep"
};

CleksComment JsonComments[] = {};

CleksString JsonStrings[] = {
	{'"', '"'}
};

CleksWhitespace JsonWhitespaces[] = {
	' ', '\n'
};

CleksConfig JsonConfig = {
	.words = JsonWords,
	.word_count = CLEKS_ARR_LEN(JsonWords),
	.symbols = JsonSymbols,
	.symbol_count = CLEKS_ARR_LEN(JsonSymbols),
	.comments = JsonComments,
	.comment_count = CLEKS_ARR_LEN(JsonComments),
	.strings = JsonStrings,
	.string_count = CLEKS_ARR_LEN(JsonStrings),
	.whitespaces = JsonWhitespaces,
	.whitespace_count = CLEKS_ARR_LEN(JsonWhitespaces),
	.flags = CLEKS_FLAGS_INTEGERS | CLEKS_FLAGS_FLOATS | CLEKS_FLAGS_NO_UNKNOWN
};

char* json_print_string(CleksToken token)
{
	uint32_t id = cleks_token_id(token);
	switch (cleks_token_type(token)){
		case CLEKS_WORD: return JsonWordNames[id];
		case CLEKS_SYMBOL: return JsonSymbolNames[id];
		default: return "";
	}
}

int main(int argc, char **argv)
{
	cleks_assert(argc == 2, "Not enough parameters provided!");
	char *filename = argv[1];
	char *file_content = read_entire_file(filename);
	Clekser clekser = Cleks_create(file_content, strlen(file_content), JsonConfig, filename);
	
	CleksToken token;
	
	while (Cleks_next(&clekser, &token)){
		switch(cleks_token_type(token)){
			case CLEKS_WORD:
			case CLEKS_SYMBOL:{
				printf("%s:%d:%d %s: %s\n", token.loc.filename, token.loc.row, token.loc.column, cleks_token_type_name(cleks_token_type(token)), json_print_string(token));
			}break;
			default: Cleks_print(token);
		}
	}
	// free(buffer);
	return 0;
}