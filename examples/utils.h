#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

unsigned long long file_size(const char* file_path){
    struct stat file;
    if (stat(file_path, &file) == -1){
        return 0;
    }
    return (unsigned long long) file.st_size;
}

// allocate and populate a string with the file's content
char* read_entire_file(char *file_path)
{
    if (file_path == NULL) return NULL;
    FILE *file = fopen(file_path, "r");
    if (file == NULL) return NULL;
    unsigned long long size = file_size(file_path);
    char *content = (char*) calloc(size+1, sizeof(*content));
    if (!content){
        fclose(file);
        return NULL;
    }
    fread(content, 1, size, file);
    fclose(file);
    return content;
}

#endif // _UTILS_H