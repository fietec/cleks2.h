#ifndef _CLEKS_H
#define _CLEKS_H

/*
	cleks.h by Constantijn de Meer
	Cleks is a lightweight and highly customizable general-purpose lexer purely written in C.
	A custom config struct is used to define the features of a language or syntax.
	This library does not dynamically allocate any memory.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#define CLEKS_ARR_LEN(arr) (arr != NULL ? (sizeof((arr))/sizeof((arr)[0])) : 0) // get the length of an array
#define CLEKS_ANSI_END "\e[0m" // reset ansi color
#define CLEKS_ANSI_RGB(r, g, b) ("\e[38;2;" #r ";" #g ";" #b "m") // set ansi color to rgb value
#define CLEKS_VALUE_FORMAT "%.*s"

#define CLEKS_FLAGS_ALL 0xf
#define CLEKS_FLAGS_INTEGERS 0x1
#define CLEKS_FLAGS_FLOATS 0x2
#define CLEKS_FLAGS_HEX 0x4
#define CLEKS_FLAGS_BIN 0x8
#define CLEKS_FLAGS_NO_UNKNOWN 0x10

#define CLEKS_NO_LOC (CleksLoc) {0}

/* Debugging */
#define cleks_info(msg, ...) (printf("%s%s:%d: " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 255, 255), __FILE__, __LINE__, ## __VA_ARGS__))
#ifdef CLEKS_DEBUG
#define cleks_debug(msg, ...) (printf("%s%s:%d: [DEBUG] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 124, 0), __FILE__, __LINE__, ## __VA_ARGS__))
#else
#define cleks_debug(msg, ...) (0)
#endif
#define cleks_error(msg, ...) (fprintf(stderr, "%s%s:%d: [ERROR] " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 0, 0), __FILE__, __LINE__, ## __VA_ARGS__))

#define cleks_assert(statement, msg, ...) do{if (!(statement)) {cleks_error(msg, ##__VA_ARGS__); exit(1);}} while (0);

/* Helper macros */
#define clekser_inc(clekser) do{(clekser)->index++; (clekser)->loc.column++;}while(0);
#define clekser__get_char(clekser) (clekser)->buffer[(clekser)->index]
#define clekser__get_pointer(clekser) (clekser)->buffer + (clekser)->index
#define clekser__check_line(clekser) do{if (clekser__get_char((clekser)) == '\n'){(clekser)->loc.row++; (clekser)->loc.column=1;}else{clekser->loc.column++;}}while(0);

#define cleks__is_special(c) ((c) == '\0' || (c) == EOF)

/* User macros */
// #define cleks_token_type(token) (CleksTokenTypeNames[(uint64_t)(token)])
#define cleks_token_type(token) ((uint64_t) ((token).id) >> 32)
#define cleks_token_type_name(type) (CleksTokenTypeNames[(type)])
#define cleks_token_id(token) (uint32_t)(token).id
#define cleks_token_value(token) (token).start
#define cleks_token_value_length(token) ((token).end - (token).start)
#define cleks_token(type, id) ((CleksTokenID) ((uint64_t) type) << 32)

#define CLEKS_ANY_ID 0xFFFFFFFF

/* Type definitions */
typedef const char CleksSymbol;
typedef const char* CleksWord;
typedef const char CleksWhitespace;

typedef enum{
	CLEKS_WORD,
	CLEKS_SYMBOL,
	CLEKS_STRING,
	CLEKS_INTEGER,
	CLEKS_FLOAT,
	CLEKS_HEX,
	CLEKS_BIN,
	CLEKS_UNKNOWN,
	CLEKS_TOKEN_TYPE_COUNT
} CleksTokenType;

const char* CleksTokenTypeNames[] = {
	[CLEKS_WORD] = "Word",
	[CLEKS_SYMBOL] "Symbol",
	[CLEKS_STRING] = "String",
	[CLEKS_INTEGER] = "Int",
	[CLEKS_FLOAT] = "Float",
	[CLEKS_HEX] = "Hex",
	[CLEKS_BIN] = "Bin",
	[CLEKS_UNKNOWN] = "Unknown"
};
_Static_assert(CLEKS_TOKEN_TYPE_COUNT == CLEKS_ARR_LEN(CleksTokenTypeNames), "CleksTokenTypeNames out of sync !");

typedef struct{
	size_t row;
	size_t column;
	char *filename;
} CleksLoc;

typedef uint64_t CleksTokenID;

typedef struct{
	CleksTokenID id;
	CleksLoc loc;
	char *start;
	char *end;
} CleksToken;

typedef struct{
	char *start_del;
	char *end_del;
} CleksComment;

typedef struct{
	char start_del;
	char end_del;
} CleksString;

typedef struct{
	CleksWord *words;
	size_t word_count;
	CleksSymbol *symbols;
	size_t symbol_count;
	CleksComment *comments;
	size_t comment_count;
	CleksString *strings;
	size_t string_count;
	CleksWhitespace *whitespaces;
	size_t whitespace_count;
	uint8_t flags;
} CleksConfig;

typedef struct{
	char *buffer;
	size_t buffer_size;
	CleksLoc loc;
	size_t index;
	CleksConfig config;
} Clekser;

/* Function declerations */

// 'public' functions
Clekser Cleks_create(char *buffer, size_t buffer_size, CleksConfig config, char *filename);
bool Cleks_next(Clekser *clekser, CleksToken *token);
bool Cleks_expect(Clekser *clekser, CleksToken *token, CleksTokenType type, uint32_t id);
bool Cleks_extract(CleksToken *token, char *buffer, size_t buffer_size);
void Cleks_print(CleksToken token);

// 'private' functions
void Cleks__trim_left(Clekser *clekser);
void Cleks__skip_string(Clekser *clekser, char *del);
void Cleks__find_string(Clekser *clekser, char *del);
void Cleks__find_char(Clekser *clekser, char del);
void Cleks__set_token(CleksToken *token, uint32_t type, uint32_t id, CleksLoc loc, char *start, char *end);
bool Cleks__is_symbol(Clekser *clekser, char symbol);
bool Cleks__is_whitespace(Clekser *clekser, char c);
bool Cleks__starts_with(Clekser *clekser, char *str);
bool Cleks__str_is_float(char *s, char *e);
bool Cleks__str_is_int(char *s, char *e);
bool Cleks__str_is_hex(char *s, char *e);
bool Cleks__str_is_bin(char *s, char *e);
void Cleks__print_default(CleksToken token);

#endif // _CLEKS_H

/* 
	cleks.c 
	define ClEKS_IMPLEMENTATION to gain access to the function implementations
*/

#ifdef CLEKS_IMPLEMENTATION

Clekser Cleks_create(char *buffer, size_t buffer_size, CleksConfig config, char *filename)
{
	cleks_assert(buffer != NULL, "Invalid parameter buffer:%p", buffer);
	return (Clekser) {.buffer = buffer, .buffer_size=buffer_size, .loc=(CleksLoc){1, 1, filename}, .index=0, .config=config};
}

bool Cleks_next(Clekser *clekser, CleksToken *token)
{
	cleks_assert(clekser != NULL && token != NULL, "Invalid arguments clekser:%p, token:%p", clekser, token);

	while (true){
		if (clekser->index >= clekser->buffer_size) return false;
		// skip spaces
		Cleks__trim_left(clekser);
		cleks_debug("Position after trim: %d:%d", clekser->loc.row, clekser->loc.column);
		if (clekser->index >= clekser->buffer_size) return false;
		// skip comments
		bool comment_found = false;
		for (size_t i=0; i<clekser->config.comment_count; ++i){
			CleksComment comment = clekser->config.comments[i];
			cleks_debug("Looking for comment '%s'", comment.start_del);
			if (Cleks__starts_with(clekser, comment.start_del)){
				cleks_debug("Found Comment: '%s'", comment.start_del);
				Cleks__skip_string(clekser, comment.start_del);
				Cleks__find_string(clekser, comment.end_del);
				Cleks__skip_string(clekser, comment.end_del);
				comment_found = true;
				break;
			}
		}
		if (!comment_found) break;
	}
	// zero-initialize the token
	memset(token, 0, sizeof(*token));
	cleks_debug("Trying to lex string");
	// try to lex string
	for (size_t i=0; i<clekser->config.string_count; ++i){
		CleksString string = clekser->config.strings[i];
		if (clekser__get_char(clekser) == string.start_del){
			CleksLoc start_loc = clekser->loc;
			clekser->index++;
			clekser->loc.column++;
			char *p_start = clekser__get_pointer(clekser);
			Cleks__find_char(clekser, string.end_del);
			char *p_end = clekser__get_pointer(clekser);
			Cleks__set_token(token, CLEKS_STRING, 0, start_loc, p_start, p_end); 
			// printf("Found String at (%d, %d) with length %d, index is %d\n", clekser->loc.row, clekser->loc.column, p_end-p_start,clekser->index);
			clekser_inc(clekser);
			return true;
		}
	}
	cleks_debug("Trying to lex symbol");
	// try to lex symbols
	for (size_t i=0; i<clekser->config.symbol_count; ++i){
		if (clekser__get_char(clekser) == clekser->config.symbols[i]){
			char *p_start = clekser__get_pointer(clekser);
			Cleks__set_token(token, CLEKS_SYMBOL, i, clekser->loc, p_start, p_start+1);
			// printf("Found Symbol %d at (%d, %d), index is %d\n", i, clekser->loc.row, clekser->loc.column, clekser->index);
			clekser_inc(clekser);
			return true;
		}
	}
	// TODO: try to lex words
	cleks_debug("Trying to lex word");
	char *p_start = clekser__get_pointer(clekser);
	CleksLoc start_loc = clekser->loc;
	char c;
	while (true){
		c = clekser__get_char(clekser);
		if (Cleks__is_symbol(clekser, c) || Cleks__is_whitespace(clekser, c) || cleks__is_special(c)) break;
		cleks_debug("Char is not end condition");
		bool delimeter_found = false;
		for (size_t i=0; i<clekser->config.string_count; ++i){
			if (c == clekser->config.strings[i].start_del){
				delimeter_found = true;
				break;
			}
		}
		if (delimeter_found) break;
		for (size_t i=0; i<clekser->config.comment_count; ++i){
			if (Cleks__starts_with(clekser, clekser->config.comments[i].start_del)){
				delimeter_found = true;
				break;
			}
		}
		if (delimeter_found) break;
		clekser_inc(clekser);
	}
	char *p_end = clekser__get_pointer(clekser);
	cleks_debug("Found end, length=%d", p_end-p_start);
	for (size_t i=0; i<clekser->config.word_count; ++i){
		cleks_debug("Comparing against '%s'", clekser->config.words[i]);
		if (memcmp(p_start, clekser->config.words[i], p_end-p_start) == 0){
			Cleks__set_token(token, CLEKS_WORD, i, start_loc, p_start, p_end);
			return true;
		}
	}
	cleks_debug("Trying to lex nums");
	// no matching words found
	if ((clekser->config.flags & CLEKS_FLAGS_INTEGERS) && Cleks__str_is_int(p_start, p_end)){
		Cleks__set_token(token, CLEKS_INTEGER, 0, start_loc, p_start, p_end);
		return true;
	}
	if ((clekser->config.flags & CLEKS_FLAGS_FLOATS)&& Cleks__str_is_float(p_start, p_end)){
		Cleks__set_token(token, CLEKS_FLOAT, 0, start_loc, p_start, p_end);
		return true;
	}
	if ((clekser->config.flags & CLEKS_FLAGS_HEX) && Cleks__str_is_hex(p_start, p_end)){
		Cleks__set_token(token, CLEKS_HEX, 0, start_loc, p_start, p_end);
		return true;
	}
	if ((clekser->config.flags & CLEKS_FLAGS_BIN) && Cleks__str_is_bin(p_start, p_end)){
		Cleks__set_token(token, CLEKS_BIN, 0, start_loc, p_start, p_end);
		return true;
	}
	if ((clekser->config.flags & CLEKS_FLAGS_NO_UNKNOWN) == 0){
		Cleks__set_token(token, CLEKS_UNKNOWN, 0, start_loc, p_start, p_end);
		return true;
	}
	cleks_error("Unknown word found (flags: %d) at %s:%d:%d \"%.*s\"\n", clekser->config.flags, start_loc.filename, start_loc.row, start_loc.column, p_end-p_start, p_start);
	return false;
}

bool Cleks_expect(Clekser *clekser, CleksToken *token, CleksTokenType type, uint32_t id)
{
	CleksToken t_token;
	if (!Cleks_next(clekser, &t_token)) return false;
	if (cleks_token_type(t_token) != type || (id != CLEKS_ANY_ID && cleks_token_id(t_token) != id)){
		cleks_error("Expected: %s:%d, but got %s:%d!", cleks_token_type_name(type), (id == CLEKS_ANY_ID ? 0 : id), cleks_token_type_name(cleks_token_type(t_token)), cleks_token_id(t_token));
		return false;
	}
	memcpy(token, &t_token, sizeof(CleksToken));
	return true;
}

bool Cleks_extract(CleksToken *token, char *buffer, size_t buffer_size)
{
	if (token == NULL || buffer == NULL) return false;
	size_t value_len = token->end - token->start;
	if (value_len >= buffer_size) return false;
	if (cleks_token_type(*token) == CLEKS_STRING){
		char temp_buffer[value_len+1];
		memset(temp_buffer, 0, value_len+1);
		char *pr = token->start;
		char *pw = temp_buffer;
		while (pr != token->end){
			if (*pr == '\\'){
				switch(*++pr){
					case '\'': *pw = 0x27; break;
					case '"':  *pw = 0x22; break;
					case '?':  *pw = 0x3f; break;
					case '\\': *pw = 0x5c; break;
					case 'a':  *pw = 0x07; break;
					case 'b':  *pw = 0x08; break;
					case 'f':  *pw = 0x0c; break;
					case 'n':  *pw = 0x0a; break;
					case 'r':  *pw = 0x0d; break;
					case 't':  *pw = 0x09; break;
					case 'v':  *pw = 0x0b; break;
					default:{
						*pw++ = '\\';
						*pw = *pr;
					}
				}
			}
			else{
				*pw = *pr;
			}
			pr++;   
			pw++;
		}
		snprintf(buffer, pw-temp_buffer+2, "%s", temp_buffer);
	}
	else{
		sprintf(buffer, "%.*s\0", value_len, token->start);
	}
	return true;
}

void Cleks__print_default(CleksToken token)
{
	// TODO: probably best to do this with string builders instead
	CleksTokenType type = cleks_token_type(token);
	cleks_assert(type < CLEKS_TOKEN_TYPE_COUNT, "Invalid token type: %u!", type);
	switch(type){
		case CLEKS_WORD:
		case CLEKS_SYMBOL: printf("'%.*s'", token.end-token.start, token.start); break;
		case CLEKS_STRING: printf("\"%.*s\"", token.end-token.start, token.start); break;
		case CLEKS_INTEGER:
		case CLEKS_FLOAT: 
		case CLEKS_HEX: 
		case CLEKS_BIN: printf("%.*s", token.end-token.start, token.start); break;
		case CLEKS_UNKNOWN: printf("<%.*s>", token.end-token.start, token.start); break;
		default: cleks_error("Uninplemented type in print: %s", cleks_token_type_name(type)); exit(1);
	}
}

void Cleks_print(CleksToken token)
{
	if (token.loc.filename != NULL) printf("%s:", token.loc.filename);
	printf("%d:%d %s: ", token.loc.row, token.loc.column, cleks_token_type_name(cleks_token_type(token)));
	Cleks__print_default(token);
	putchar('\n');
}

void Cleks__trim_left(Clekser *clekser)
{
	cleks_assert(clekser != NULL, "Invalid argument clekser:%p", clekser);
	char c;
	while (Cleks__is_whitespace(clekser, (c = clekser__get_char(clekser))) || cleks__is_special(c)){
		clekser__check_line(clekser);
		clekser->index++;
	}
}

void Cleks__skip_string(Clekser *clekser, char *str)
{
	cleks_assert(clekser != NULL && str != NULL, "Invalid arguments clekser:%p, str:%p", clekser, str);
	while (clekser__get_char(clekser) == *str++){
		clekser__check_line(clekser);
		clekser->index++;
	}
}

void Cleks__find_string(Clekser *clekser, char *del)
{
	cleks_assert(clekser != NULL && del != NULL, "Invalid arguments clekser:%p, del:%p", clekser, del);
	while (!Cleks__starts_with(clekser, del)){
		cleks_assert(clekser->index < clekser->buffer_size, "OutOfBounce! Could not find \"%s\"", del);
		clekser__check_line(clekser);
		clekser->index++;
	}
	cleks_debug("Found %s at %d:%d", del, clekser->loc.row, clekser->loc.column);
}

void Cleks__find_char(Clekser *clekser, char del)
{
	cleks_assert(clekser != NULL, "Invalid argument clekser:%p", clekser);
	// printf("searching for %c\n", del);
	while (clekser__get_char(clekser) != del){
		cleks_assert(clekser->index < clekser->buffer_size, "OutOfBounce! Could not find \"%s\"", del);
		clekser__check_line(clekser);
		clekser->index++;
	}
}

void Cleks__set_token(CleksToken *token, uint32_t type, uint32_t id, CleksLoc loc, char *start, char *end)
{
	cleks_assert(token != NULL, "Invalid argument token:%p", token);
	// cleks_debug("Setting token: %s", cleks_token_type(type));
	token->id = ((uint64_t) type << 32) | id;
	token->loc = loc;
	token->start = start;
	token->end = end;
}

bool Cleks__is_whitespace(Clekser *clekser, char c)
{
	cleks_assert(clekser != NULL, "Invalid argument clekser:%p", clekser);
	for (size_t i=0; i<clekser->config.whitespace_count; ++i){
		if (clekser->config.whitespaces[i] == c) return true;
	}
	return false;
}

bool Cleks__is_symbol(Clekser *clekser, char c)
{
	cleks_assert(clekser != NULL, "Invalid argument clekser:%p", clekser);
	for (size_t i=0; i<clekser->config.symbol_count; ++i){
		if (c == clekser->config.symbols[i]) return true;
	}
	return false;
}

bool Cleks__starts_with(Clekser *clekser, char *str)
{
	cleks_assert(clekser != NULL, "Invalid arguments clekser:%p, str:%p", clekser, str);
	cleks_debug("Starts_with ('%s'): Arguments valid", str);
	char *curr = clekser->buffer + clekser->index;
	for (size_t i=0; i<strlen(str); ++i, ++curr){
		if (clekser->index + i >= clekser->buffer_size || *curr != *(str+i)) return false;
	}
	return true;
}

bool Cleks__str_is_int(char *s, char *e)
{
	if (!s || !e || e-s < 1) return false;
	if (*s == '-' || *s == '+') s++;
	while (s < e){
		if (!isdigit(*s++)) return false;
	}
	return true;
}

bool Cleks__str_is_float(char *s, char *e)
{
    char* ep = NULL;
    strtod(s, &ep);
    return (ep && ep == e);
}

bool Cleks__str_is_hex(char *s, char *e)
{
	if (s == NULL || e == NULL || e-s < 3) return false;
	if (*s++ != '0' || *s++ != 'x') return false;
	while (s < e){
		if (!isxdigit(*s++)) return false;
	}
	return true;
}

bool Cleks__str_is_bin(char *s, char *e)
{
	if (s == NULL || e == NULL || e-s < 3) return false;
	if (*s++ != '0' || *s++ != 'b') return false;
	while (s < e){
		char c = *s++;
		if (c != '0' && c != '1') return false;
		
	}
	return true;
}

#endif // CLEKS_IMPLEMENTATION