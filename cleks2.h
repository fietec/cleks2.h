/* 
    =========================================
    cleks2.h <https://github.com/fietec/cleks2.h>
    =========================================
    Copyright (c) 2025 Constantijn de Meer

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef _CLEKS_H
#define _CLEKS_H

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
#define CLEKS_LOC_EXPAND(loc) (loc).filename, (loc).row, (loc).column

#define CLEKS_FLAGS_ALL_NUMS 0xf           // enable all number parsing
#define CLEKS_FLAGS_INTEGERS 0x1           // enable parsing of CLEKS_INTEGER
#define CLEKS_FLAGS_FLOATS 0x2             // enable parsing of CLEKS_FLOAT
#define CLEKS_FLAGS_HEX 0x4                // enable parsing of CLEKS_HEX
#define CLEKS_FLAGS_BIN 0x8                // enable parsing of CLEKS_BIN
#define CLEKS_FLAGS_KEEP_UNKNOWN 0x10      // do not mark CLEKS_UNKNOWN when printing using Cleks_print_default
#define CLEKS_FLAGS_DISABLE_UNKNOWN 0x20   // do not allow CLEKS_UNKNOWN, throw error instead

/* Debugging */
#define cleks_info(msg, ...) (printf("%s%s:%d: " msg CLEKS_ANSI_END "\n", CLEKS_ANSI_RGB(255, 255, 255), __FILE__, __LINE__, ## __VA_ARGS__))
#ifdef CLEKS_DEBUG // define this to enable debugging messages using cleks_debug
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
// extract the CleksTokenType from the id of a token
#define cleks_token_type(id) ((CleksTokenType) (((CleksTokenID)(id)) >> 32))
// get the string name of a CleksTokenType
#define cleks_token_type_name(type) (CleksTokenTypeNames[(CleksTokenID)(type)])
// extract the CleksTokenIndex from the id of a token
#define cleks_token_index(id) (CleksTokenIndex)((id) & 0xFFFFFFFF)
// buid a CleksTokenID from a type and index
#define cleks_token_id(type, index) ((CleksTokenID) ((CleksTokenID) (type) << 32) | ((CleksTokenIndex)(index)))
// just a shorthand for accesing the string value of a token
#define cleks_token_value(token) (token).start
// calculate the string length of a token in the original buffer
#define cleks_token_value_length(token) ((token).end - (token).start)

// used with cleks_token_id(type, index) if no index is supposed to be defined
#define CLEKS_ANY_INDEX 0xFFFFFFFF

/* Type definitions */
typedef const char CleksSymbol;     // CLEKS_SYMBOL 
typedef const char* CleksWord;      // CLEKS_WORD
typedef const char CleksWhitespace; // token delimeters to be ignored

// the overall type of a token
typedef enum{
	CLEKS_WORD,             // word literals
	CLEKS_SYMBOL,           // character literals
	CLEKS_STRING,           // region within character delimeters
    CLEKS_FIELD,            // region within string delimeters
	CLEKS_INTEGER,          // decimal integers
	CLEKS_FLOAT,            // floating pointer numbers
	CLEKS_HEX,              // heximal integers
	CLEKS_BIN,              // binary integers
	CLEKS_UNKNOWN,          // unknown literals not within CLEKS_WORD
	CLEKS_TOKEN_TYPE_COUNT  // not a type, the amount of types
} CleksTokenType;

// Printing names for each TokenType
const char* CleksTokenTypeNames[] = {
	[CLEKS_WORD] = "Word",
	[CLEKS_SYMBOL] "Symbol",
	[CLEKS_STRING] = "String",
    [CLEKS_FIELD] = "Field",
	[CLEKS_INTEGER] = "Int",
	[CLEKS_FLOAT] = "Float",
	[CLEKS_HEX] = "Hex",
	[CLEKS_BIN] = "Bin",
	[CLEKS_UNKNOWN] = "Unknown"
};

_Static_assert(CLEKS_TOKEN_TYPE_COUNT == CLEKS_ARR_LEN(CleksTokenTypeNames), "CleksTokenTypeNames out of sync !");

// the original location of a token in the buffer
typedef struct{
	size_t row;
	size_t column;
	char *filename;
} CleksLoc;

// the index of a token within a type config
typedef uint32_t CleksTokenIndex;
// a mask containing the CleksTokenType and CleksTokenIndex
typedef uint64_t CleksTokenID;

// an extracted token
typedef struct{
	CleksTokenID id; // the type and index of a token
	CleksLoc loc;    // the token's location in the buffer
	char *start;     // pointer to the start of the token within the buffer
	char *end;       // pointer to the end of the token within the buffer
} CleksToken;

// a function used for printing a token representation
typedef void (*CleksPrintFn) (CleksToken);

// a region within which everything is ignored
typedef struct{
	char *start_del;  // start delimeter 
	char *end_del;    // end delimeter
} CleksComment;

// a character string within char delimeters
typedef struct{
	char start_del;   // start delimeter
	char end_del;     // end delimeter
} CleksString;

// a character string within string delimeters
typedef struct{
    char *prefix;
    char *suffix;
} CleksField;

// config structure containing all custom sub-type definitions
typedef struct{
	CleksWord *words;             // definitions of CLEKS_WORD
	size_t word_count;          
	CleksSymbol *symbols;         // definitions of CLEKS_SYMBOL
	size_t symbol_count;
	CleksString *strings;         // definitions of CLEKS_STRING
	size_t string_count;
    CleksField *fields;           // definitions of CLEKS_FIELD
    size_t field_count;
	CleksComment *comments;       // definitions of ignored regions
	size_t comment_count;
	CleksWhitespace *whitespaces; // definitiosn of ignored characters
	size_t whitespace_count;
	uint8_t flags;                // additional lexing rules
    CleksPrintFn print_fn;
} CleksConfig;

// the lexing structure containing runtime lexing information
typedef struct{
	char *buffer;
	size_t buffer_size;
	CleksLoc loc;
	size_t index;
	CleksConfig config;
} Clekser;

/* Function declarations */

// 'public' functions
// initialization of a Clekser structure
Clekser Cleks_create(char *buffer, size_t buffer_size, CleksConfig config, char *filename);
// retreive the next token, returns `true` on success
bool Cleks_next(Clekser *clekser, CleksToken *token);
// retreive the next token and fail when not of specified type  
bool Cleks_expect(Clekser *clekser, CleksToken *token, CleksTokenID id);
// extract the content of a token into seperate buffer
bool Cleks_extract(CleksToken *token, char *buffer, size_t buffer_size);
// use the print dialog associated with the Clekser to print a token
void Cleks_print(Clekser clekser, CleksToken token);
// the default print dialog
void Cleks_print_default(Clekser clekser, CleksToken token);

// 'private' functions
void Cleks__trim_left(Clekser *clekser);
void Cleks__skip_string(Clekser *clekser, char *del);
bool Cleks__try_skip_comments(Clekser *clekser, bool *result);
bool Cleks__find_string(Clekser *clekser, char *del);
bool Cleks__find_char(Clekser *clekser, char del);
bool Cleks__try_find_string(Clekser *clekser, char *del);
bool Cleks__try_find_char(Clekser *clekser, char del);
void Cleks__set_token(CleksToken *token, uint32_t type, uint32_t id, CleksLoc loc, char *start, char *end);
bool Cleks__is_symbol(Clekser *clekser, char symbol);
bool Cleks__is_whitespace(Clekser *clekser, char c);
bool Cleks__starts_with(Clekser *clekser, char *str);
bool Cleks__str_is_float(char *s, char *e);
bool Cleks__str_is_int(char *s, char *e);
bool Cleks__str_is_hex(char *s, char *e);
bool Cleks__str_is_bin(char *s, char *e);

#endif // _CLEKS_H

/* 
	cleks2.c 
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
		if (clekser->index >= clekser->buffer_size) return false;
		// skip comments
		bool comment_found = false;
		if (!Cleks__try_skip_comments(clekser, &comment_found)) return false;
		if (!comment_found) break;
	}
	// zero-initialize the token
	memset(token, 0, sizeof(*token));
	// try to lex string
	for (size_t i=0; i<clekser->config.string_count; ++i){
		CleksString string = clekser->config.strings[i];
		if (clekser__get_char(clekser) == string.start_del){
			CleksLoc start_loc = clekser->loc;
			clekser->index++;
			clekser->loc.column++;
			char *p_start = clekser__get_pointer(clekser);
			if (!Cleks__try_find_char(clekser, string.end_del)){
                cleks_error("Expected matching '%c' after string delimeter '%c' at %s:%d:%d", string.end_del, string.start_del, CLEKS_LOC_EXPAND(start_loc));
                return false;
            }
			char *p_end = clekser__get_pointer(clekser);
			Cleks__set_token(token, CLEKS_STRING, i, start_loc, p_start, p_end); 
			clekser_inc(clekser);
			return true;
		}
	}
	// try to lex symbols
	for (size_t i=0; i<clekser->config.symbol_count; ++i){
		if (clekser__get_char(clekser) == clekser->config.symbols[i]){
			char *p_start = clekser__get_pointer(clekser);
			Cleks__set_token(token, CLEKS_SYMBOL, i, clekser->loc, p_start, p_start+1);
			clekser_inc(clekser);
			return true;
		}
	}
    
    // lex fields
    for (size_t i=0; i<clekser->config.field_count; ++i){
        CleksField field = clekser->config.fields[i];
        if (Cleks__starts_with(clekser, field.prefix)){
            Cleks__skip_string(clekser, field.prefix);
            char *p_start = clekser__get_pointer(clekser);
            CleksLoc loc = clekser->loc;
            if (!Cleks__try_find_string(clekser, field.suffix)){
                cleks_error("Expected \"%s\" for matching field prefix \"%s\" at %s:%d:%d!", field.suffix, field.prefix, CLEKS_LOC_EXPAND(loc));
                return false;
            }
            char *p_end = clekser__get_pointer(clekser);
            Cleks__skip_string(clekser, field.suffix);
            Cleks__set_token(token, CLEKS_FIELD, i, loc, p_start, p_end);
            return true;
        }
    }
    
	char *p_start = clekser__get_pointer(clekser);
	CleksLoc start_loc = clekser->loc;
	char c;
	while (true){
		c = clekser__get_char(clekser);
		if (Cleks__is_symbol(clekser, c) || Cleks__is_whitespace(clekser, c) || cleks__is_special(c)) break;
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
        for (size_t i=0; i<clekser->config.field_count; ++i){
            if (Cleks__starts_with(clekser, clekser->config.fields[i].prefix)){
                delimeter_found = true;
                break;
            }
        }
        if (delimeter_found) break;
		clekser_inc(clekser);
	}
	char *p_end = clekser__get_pointer(clekser);
	for (size_t i=0; i<clekser->config.word_count; ++i){
		if (memcmp(p_start, clekser->config.words[i], p_end-p_start) == 0){
			Cleks__set_token(token, CLEKS_WORD, i, start_loc, p_start, p_end);
			return true;
		}
	}
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
	if ((clekser->config.flags & CLEKS_FLAGS_DISABLE_UNKNOWN) == 0){
		Cleks__set_token(token, CLEKS_UNKNOWN, (clekser->config.flags & CLEKS_FLAGS_KEEP_UNKNOWN)? 1:0, start_loc, p_start, p_end);
		return true;
	}
	cleks_error("Unknown word found (flags: %d) at %s:%d:%d \"%.*s\"\n", clekser->config.flags, start_loc.filename, start_loc.row, start_loc.column, p_end-p_start, p_start);
	return false;
}

bool Cleks_expect(Clekser *clekser, CleksToken *token, CleksTokenID id)
{
	CleksToken t_token;
	if (!Cleks_next(clekser, &t_token)) return false;
    CleksTokenType type = cleks_token_type(id);
    CleksTokenIndex index = cleks_token_index(id);
	if (cleks_token_type(t_token.id) != type || (index != CLEKS_ANY_INDEX && cleks_token_index(t_token.id) != index)){
		cleks_error("Expected: %s:%d, but got %s:%d!", cleks_token_type_name(type), (index == CLEKS_ANY_INDEX ? 0 : index), cleks_token_type_name(cleks_token_type(t_token.id)), cleks_token_index(t_token.id));
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
	if (cleks_token_type(token->id) == CLEKS_STRING){
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
		sprintf(buffer, "%.*s", value_len, token->start);
	}
	return true;
}

void Cleks_print_default(Clekser clekser, CleksToken token)
{
	// TODO: probably best to do this with string builders instead
    if (token.loc.filename != NULL) printf("%s:", token.loc.filename);
	CleksTokenType type = cleks_token_type(token.id);
    CleksTokenIndex index = cleks_token_index(token.id);
    bool keep_unknown = (type == CLEKS_UNKNOWN && index == 1);
	printf("%d:%d %s: ", token.loc.row, token.loc.column, (keep_unknown)? "":cleks_token_type_name(type));
	cleks_assert(type < CLEKS_TOKEN_TYPE_COUNT, "Invalid token type: %u!", type);
	switch(type){
		case CLEKS_WORD:
		case CLEKS_SYMBOL: printf("'%.*s'", token.end-token.start, token.start); break;
        case CLEKS_FIELD: printf("<%s '%.*s' %s>", clekser.config.fields[index].prefix, token.end-token.start, token.start, clekser.config.fields[index].suffix); break;
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

void Cleks_print(Clekser clekser, CleksToken token)
{
	if (clekser.config.print_fn != NULL){
        clekser.config.print_fn(token);
    }
    else{
        Cleks_print_default(clekser, token);
    }
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

bool Cleks__find_string(Clekser *clekser, char *del)
{
	cleks_assert(clekser != NULL && del != NULL, "Invalid arguments clekser:%p, del:%p", clekser, del);
	while (!Cleks__starts_with(clekser, del)){
		if (clekser->index >= clekser->buffer_size) return false;
		clekser__check_line(clekser);
		clekser->index++;
	}
    return true;
}

bool Cleks__find_char(Clekser *clekser, char del)
{
	cleks_assert(clekser != NULL, "Invalid argument clekser:%p", clekser);
	while (clekser__get_char(clekser) != del){
		if (clekser->index >= clekser->buffer_size) return false;
		clekser__check_line(clekser);
		clekser->index++;
	}
    return true;
}

bool Cleks__try_find_string(Clekser *clekser, char *del)
{
    cleks_assert(clekser != NULL && del != NULL, "Invalid argumens clekser:%p, del:%p", clekser, del);
    while (!Cleks__starts_with(clekser, del)){
        if (clekser->index >= clekser->buffer_size) return false;
        bool comment_found = false;
        if (!Cleks__try_skip_comments(clekser, &comment_found)) return false;
        if (comment_found) continue;
        clekser__check_line(clekser);
        clekser->index++;
    }
    return true;
}

bool Cleks__try_find_char(Clekser *clekser, char del)
{
    cleks_assert(clekser != NULL, "Invalid arguments clekser:%p", clekser);
    while (clekser__get_char(clekser) != del){
        if (clekser->index >= clekser->buffer_size) return false;
        bool comments_found = false;
        if (!Cleks__try_skip_comments(clekser, &comments_found)) return false;
        if (comments_found) continue;
        clekser__check_line(clekser);
        clekser->index++;
    }
    return true;
}

bool Cleks__try_skip_comments(Clekser *clekser, bool *result)
{
    for (size_t i=0; i<clekser->config.comment_count; ++i){
        CleksComment comment = clekser->config.comments[i];
        if (Cleks__starts_with(clekser, comment.start_del)){
            CleksLoc loc = clekser->loc;
            Cleks__skip_string(clekser, comment.start_del);
            if (!Cleks__find_string(clekser, comment.end_del)){
                cleks_error("Expected \"%s\" for matching comment delimeter \"%s\" at %s:%d:%d!", comment.end_del, comment.start_del, CLEKS_LOC_EXPAND(loc));
                return false;
            }
            Cleks__skip_string(clekser, comment.end_del);
            *result = true;
            break;
        }
    }
    return true;
}

void Cleks__set_token(CleksToken *token, uint32_t type, uint32_t index, CleksLoc loc, char *start, char *end)
{
	cleks_assert(token != NULL, "Invalid argument token:%p", token);
	token->id = cleks_token_id(type, index);
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
	char *curr = clekser->buffer + clekser->index;
	for (size_t i=0, n=strlen(str); i<n; ++i, ++curr){
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