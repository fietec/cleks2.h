# cleks2.h
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://github.com/fietec/cleks.h/blob/master/LICENSE)

`cleks2.h` is a highly customizable header-only lexer written in C.
It is the second iteration of [cleks.h](https://github.com/fietec/cleks.h).

## Table of contents
- [How to build](#how-to-build)
- [How to use](#how-to-use)
    - [Initialization](#initialization)
    - [Configuration](#configuration)
        - [Flags](#flags)
    - [Running](#running)
- [API](#api)
    - [Clekser](#clekser)
    - [Tokens](#tokens)
        - [Token type](#token-type)
        - [Token index](#token-index)
        - [Token loc](#token-loc))
    - [Config](#config)
    - [Words](#words)
    - [Symbols](#symbols)
    - [Strings](#strings)
    - [Comments](#comments)
    - [Whitespaces](#whitespaces)
    - [Functions](#functions)
    - [Macros](#macros)
    

## How to build

1. Download `cleks2.h` or clone the repository for all templates and examples.
2. Include `cleks2.h` in your project.
3. To access the function bodies, define `CLEKS_IMPLEMENTATION`

```c 
#define CLEKS_IMPLEMENTATION
#include "cleks2.h"
```

## How to use

### Initialization

First create a [Clekser](#clekser) struct by calling 
```c 
Clekser Cleks_create(char *buffer, size_t buffer_size, CleksConfig config, char *filename, CleksPrintFn print_fn);
```
Arguments:
- `buffer` : [char*] the string buffer to lex
- `buffer_size` : [size_t] the length of the buffer
- `config` : [[CleksConfig](#config)] the configuration struct
- `filename` : [`char*`] a filename indicating where the content of the buffer originated, only used when printing
- `print_fn` : [`CleksPrintFn`] a custom function for printing tokens (*NULL* for `Cleks_print_default`)

### Configuration

**cleks2** is highly customizable via the [CleksConfig](#config) struct.
A `CleksConfig` can be understood as a description of the "language" rules the lexer should follow.

```c 
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
```

Currently, there are five customizable fields, each with a corresponding `_count` field, and a field for further flags:
- `words`: [[CleksWord*](#words)] an array of string literals to find
- `symbols` : [[CleksSymbol*](#symbols)] an array of character literals to find (highest priority when lexing)
- `comments` : [[CleksComment*](#comments)] an array of comment definitions
- `strings` : [[CleksString*](#strings)] an array of string delimeter definitions
- `whitespaces` : [[CleksWhitespace*](#whitespaces)] an array of whitespace delimeter definitions
- `flags` : [uint8_t] a bit-mask containing further lexing instructions

The created `CleksConfig` struct is simply assinged to a `Clekser` via `Cleks_create`, as seen above.
#### Flags
- `CLEKS_FLAGS_INTEGERS` : enable integer recognition
- `CLEKS_FLAGS_FLOATS` : enable float recognition
- `CLEKS_FLAGS_HEX` : enable hex number recognition
- `CLEKS_FLAGS_BIN` : enable bin number recognition
- `CLEKS_FLAGS_ALL` : enable all the above
- `CLEKS_FLAGS_NO_UNKNOWN` : don't allow unknown tokens

### Running
To begin extracting tokens, start calling [Cleks_next](#functions) in a loop. With each iteration, the lexer will try to find a new token and set the provided [CleksToken](#tokens) accordingly, returning *true* on success.
If an error occures or when the end of the buffer is reached *false* is returned.

```c 
CleksToken token;
while (Cleks_next(&clekser, &token)){
    // do something with the token
}
// lexing finished
```

## API

### Clekser
```c
typedef struct{
	char *buffer;
	size_t buffer_size;
	CleksLoc loc;
	size_t index;
	CleksConfig config;
    CleksPrintFn print_fn;
} Clekser;
```

### Tokens
A `CleksToken` is defined as follows:
```c 
typedef struct{
	CleksTokenID id;
	CleksLoc loc;
	char *start;
	char *end;
} CleksToken;
```
Fields:
- `id` : [CleksTokenTypeID] a 64-bit integer encoding the [CleksTokenType](#token-type) and [CleksTokenIndex](#token-index)
- `loc` : [[CleksLoc](#token-loc)] a structure containing information about the location of a token in the buffer
- `start` : [char*] the pointer to the starting point of the token in the buffer
- `end` : [char*] the pointer to the end point of the token in the buffer

#### Token type
The `CleksTokenType` of a token is one of the following:
- `CLEKS_WORD`
- `CLEKS_SYMBOL`
- `CLEKS_STRING`
- `CLEKS_UNKNOWN` (an unknown word, can be disabled via the `CLEKS_FLAGS_NO_UNKNOWN` flags)
- `CLEKS_INTEGER` (has to be enabled via the `CLEKS_FLAGS_INTEGERS` or `CLEKS_FLAGS_ALL` flags)
- `CLEKS_FLOATS` (has to be enabled via the `CLEKS_FLAGS_FLOATS` or `CLEKS_FLAGS_ALL` flags)
- `CLEKS_HEX` (has to be enabled via the `CLEKS_FLAGS_HEX` or `CLEKS_FLAGS_ALL` flags)
- `CLEKS_BIN` (has to be enabled via the `CLEKS_FLAGS_BIN` or `CLEKS_FLAGS_ALL` flags)

The token type can be obtained via:
```c 
CleksTokenType type = cleks_token_type(token.id);
```

A string representation of a `CleksTokenType` can be obtained via:
```c
char *name = cleks_token_type_name(type);
```

#### Token index
The *index* is referring to the index in the array in the config of the token's type.
It can be obtained via:
```c 
CleksTokenIndex index = cleks_token_index(token.id);
```
For example given an array of `CleksWord`s: `{"do", "if", "else"}`
if the lexer comes upon "if" in the buffer the corresponding token will be of type `CLEKS_WORD` and index `1`.

#### Token loc
The *location* of a token is stored in a `CleksLoc` struct.
```c 
typedef struct{
	size_t row;
	size_t column;
	char *filename;
} CleksLoc;
```

### Config
```c 
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
```
### Words
```c 
typedef const char* CleksWord;
```

### Symbols
```c 
typedef const char CleksSymbol;
```

### Strings
```c 
typedef struct{
	char start_del;
	char end_del;
} CleksString;
```

### Comments
```c 
typedef struct{
	char *start_del;
	char *end_del;
} CleksComment;
```

### Whitespaces
```c 
typedef const char CleksWhitespace;
```

### Functions
```c 
Clekser Cleks_create(char *buffer, size_t buffer_size, CleksConfig config, char *filename, CleksPrintFn print_fn);
bool Cleks_next(Clekser *clekser, CleksToken *token);
bool Cleks_expect(Clekser *clekser, CleksToken *token, CleksTokenID id);
bool Cleks_extract(CleksToken *token, char *buffer, size_t buffer_size);
void Cleks_print(Clekser clekser, CleksToken token);
void Cleks_print_default(CleksToken token);
```
### Macros
```c 
#define cleks_token_type(id) ((CleksTokenType) (((CleksTokenID)(id)) >> 32))
#define cleks_token_type_name(type) (CleksTokenTypeNames[(CleksTokenID)(type)])
#define cleks_token_index(id) (CleksTokenIndex)((id) & 0xFFFFFFFF)
#define cleks_token_id(type, index) ((CleksTokenID) ((CleksTokenID) (type) << 32) | ((CleksTokenIndex)(index)))
#define cleks_token_value(token) (token).start
#define cleks_token_value_length(token) ((token).end - (token).start)
```