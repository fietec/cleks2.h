#define CLEKS_IMPLEMENTATION
#include "../cleks2.h"

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
    .flags = CLEKS_FLAGS_ALL_NUMS
};