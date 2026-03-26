#ifndef HEADER_DICTIONARY_H
#define HEADER_DICTIONARY_H

#include "dictionary.h"

#define HeaderDictionary Dictionary

HeaderDictionary create_header_dictionary();

int set_header(HeaderDictionary dictionary, const char *header,
               const char *value);
const char *get_header(HeaderDictionary dictionary, const char *header);

HeaderDictionary parse_new_headers(const char *headers_raw);
void parse_append_headers(HeaderDictionary dictionary, const char *headers);

#endif // HEADER_DICTIONARY_H
