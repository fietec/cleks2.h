#define CLEKS_IMPLEMENTATION
#include "utils.h"
#include "../cleks2.h"
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
	.flags = CLEKS_FLAGS_ALL 
};

void json_print(CleksToken token)
{
    if (token.loc.filename != NULL) printf("%s:", token.loc.filename);
    CleksTokenType type = cleks_token_type(token.id);
    CleksTokenIndex index = cleks_token_index(token.id);
    printf("%d:%d %s: ", token.loc.row, token.loc.column, cleks_token_type_name(type));
	cleks_assert(type < CLEKS_TOKEN_TYPE_COUNT, "Invalid token type: %u!", type);
	switch(type){
		case CLEKS_WORD: printf("%s", JsonWordNames[index]); break;
		case CLEKS_SYMBOL: printf("%s", JsonSymbolNames[index]); break;
		case CLEKS_STRING: printf("\"%.*s\"", token.end-token.start, token.start); break;
		case CLEKS_INTEGER:
		case CLEKS_FLOAT: 
		case CLEKS_HEX: 
		case CLEKS_BIN: printf("%.*s", token.end-token.start, token.start); break;
		case CLEKS_UNKNOWN: printf("<%.*s>", token.end-token.start, token.start); break;
		default: cleks_error("Uninplemented type in json_print: %s", cleks_token_type_name(type)); exit(1);
	}
    putchar('\n');
}

int main(int argc, char **argv)
{
	cleks_assert(argc == 2, "Not enough parameters provided!");
	char *filename = argv[1];
	char *file_content = read_entire_file(filename);
	Clekser clekser = Cleks_create(file_content, strlen(file_content), JsonConfig, filename, json_print);
	CleksToken token;
	
	while (Cleks_next(&clekser, &token)){
		Cleks_print(clekser, token);
	}
	// free(buffer);
	return 0;
}