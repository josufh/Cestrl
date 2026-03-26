#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stddef.h>

typedef unsigned int Dictionary;

typedef struct KeyValuePair {
  char *key;
  char *value;
} KeyValuePair;

#define dict_foreach(dictionary, pair)                                         \
  for (KeyValuePair *pair = _dict_start_iter(dictionary); pair;                \
       pair = _dict_next_iter(dictionary, pair))

Dictionary dict_create();
int dict_set(Dictionary dictionary, const char *key, const char *value);
char *dict_get(Dictionary dictionary, const char *key);
int dict_remove(Dictionary dictionary, const char *key);
void dict_print_stats(Dictionary dictionary);

KeyValuePair *_dict_start_iter(Dictionary dictionary);
KeyValuePair *_dict_next_iter(Dictionary dictionary, KeyValuePair *current);

#endif // !DICTIONARY_H
