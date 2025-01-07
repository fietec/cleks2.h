#define CLEKS_IMPLEMENTATION
#include "../cleks2.h"
#include "utils.h"


CleksWhitespace xml_whitespaces[] = {' ', '\n'};
CleksField xml_fields[] = {
    {"</", ">"},
    {"<", ">"}
};

CleksConfig xml_config = {
    .fields = xml_fields,
    .field_count = CLEKS_ARR_LEN(xml_fields),
    .whitespaces = xml_whitespaces,
    .whitespace_count = CLEKS_ARR_LEN(xml_whitespaces),
    .flags = CLEKS_FLAGS_ALL
};

int main(int argc, char **argv)
{
    cleks_assert(argc >= 2, "No input file provided!");
    char *filename = argv[1];
    char *content = read_entire_file(filename);
    Clekser clekser = Cleks_create(content, strlen(content), xml_config, filename, NULL);
    
    CleksToken token;
    while (Cleks_next(&clekser, &token)){
        Cleks_print(clekser, token);
    }
    return 0;
}