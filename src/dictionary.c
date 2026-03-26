#include "../include/dictionary.h"

#include "../include/dynamic_array.h"
#include "../include/hash.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DICT_INIT_CAPACITY 8
#define DICT_MAX_LOAD_FACTOR 0.75

#define RESOLVE_DICT_STRUCT DictionaryStruct *dict = dictionaries[dictionary]

typedef struct Entry {
  char *key;
  char *value;
  struct Entry *next;
} Entry;

typedef struct {
  Entry **buckets;
  size_t capacity;
  size_t count;
  KeyValuePair *iter_current;
} DictionaryStruct;

static DictionaryStruct **dictionaries = NULL;

static char *dict_strdup(const char *s) {
  size_t len;
  char *copy;

  if (!s)
    return NULL;

  len = strlen(s);
  copy = malloc(len + 1);
  if (!copy)
    return NULL;

  memcpy(copy, s, len + 1);
  return copy;
}

static Entry *dict_create_entry(const char *key, const char *value) {
  Entry *entry;

  entry = (Entry *)malloc(sizeof(Entry));
  if (!entry)
    return NULL;

  entry->key = dict_strdup(key);
  if (!entry->key) {
    free(entry);
    return NULL;
  }

  entry->value = dict_strdup(value);
  if (!entry->value) {
    free(entry->key);
    free(entry);
    return NULL;
  }

  entry->next = NULL;
  return entry;
}

Dictionary dict_create() {
  DictionaryStruct *dictionary =
      (DictionaryStruct *)malloc(sizeof(DictionaryStruct));
  if (!dictionary)
    exit(1);

  dictionary->buckets = (Entry **)calloc(DICT_INIT_CAPACITY, sizeof(Entry *));
  if (!dictionary->buckets) {
    free(dictionary);
    exit(1);
  }

  dictionary->capacity = DICT_INIT_CAPACITY;
  dictionary->count = 0;

  darr_add(dictionaries, dictionary);
  return darr_len(dictionaries) - 1;
}

static size_t dict_key_index(const char *key, size_t capacity) {
  unsigned long hash = djb2(key);
  return (size_t)(hash % capacity);
}

static int dict_rehash(DictionaryStruct *dict, size_t new_capacity) {
  Entry **new_buckets;
  size_t i;

  new_buckets = (Entry **)calloc(new_capacity, sizeof(Entry *));
  if (!new_buckets)
    return 0;

  for (i = 0; i < dict->capacity; i++) {
    Entry *current = dict->buckets[i];

    while (current) {
      Entry *next = current->next;

      size_t index = dict_key_index(current->key, new_capacity);

      current->next = new_buckets[index];
      new_buckets[index] = current;

      current = next;
    }
  }

  free(dict->buckets);
  dict->buckets = new_buckets;
  dict->capacity = new_capacity;

  return 1;
}

static int dict_maybe_grow(DictionaryStruct *dict) {
  double load_factor = (double)dict->count / (double)dict->capacity;

  if (load_factor >= DICT_MAX_LOAD_FACTOR) {
    size_t new_capacity = dict->capacity * 2;

    if (new_capacity == 0)
      new_capacity = DICT_INIT_CAPACITY;

    return dict_rehash(dict, new_capacity);
  }

  return 1;
}

int dict_set(Dictionary dictionary, const char *key, const char *value) {
  RESOLVE_DICT_STRUCT;

  if (!key || !value)
    return 0;

  size_t index = dict_key_index(key, dict->capacity);
  Entry *current = dict->buckets[index];

  while (current) {
    if (strcmp(current->key, key) == 0) {
      char *new_value = dict_strdup(value);
      if (!new_value)
        return 0;

      free(current->value);
      current->value = new_value;
      return 1;
    }

    current = current->next;
  }

  if (!dict_maybe_grow(dict))
    return 0;

  index = dict_key_index(key, dict->capacity);

  current = dict_create_entry(key, value);
  if (!current)
    return 0;

  current->next = dict->buckets[index];
  dict->buckets[index] = current;
  dict->count++;

  return 1;
}

char *dict_get(Dictionary dictionary, const char *key) {
  RESOLVE_DICT_STRUCT;

  if (!key)
    return NULL;

  size_t index = dict_key_index(key, dict->capacity);
  Entry *current = dict->buckets[index];

  while (current) {
    if (strcmp(current->key, key) == 0)
      return current->value;
    current = current->next;
  }

  return NULL;
}

int dict_remove(Dictionary dictionary, const char *key) {
  RESOLVE_DICT_STRUCT;

  if (!key)
    return 0;

  size_t index = dict_key_index(key, dict->capacity);

  Entry *current = dict->buckets[index];
  Entry *prev = NULL;

  while (current) {
    if (strcmp(current->key, key) == 0) {
      if (!prev) {
        dict->buckets[index] = current->next;
      } else {
        prev->next = current->next;
      }

      free(current->key);
      free(current->value);
      free(current);
      dict->count--;

      return 1;
    }

    prev = current;
    current = current->next;
  }

  return 0;
}

void dict_print_stats(Dictionary dictionary) {
  RESOLVE_DICT_STRUCT;

  double load_factor = (double)dict->count / (double)dict->capacity;

  printf("count=%zu capacity=%zu load_factor=%.3f\n", dict->count,
         dict->capacity, load_factor);
}

__attribute__((destructor)) static void dict_free() {
  if (!dictionaries)
    return;

  for (size_t i = 0; i < darr_len(dictionaries); i++) {
    DictionaryStruct *dict = dictionaries[i];

    for (size_t j = 0; j < dict->capacity; j++) {
      Entry *current = dict->buckets[j];

      while (current) {
        Entry *next = current->next;
        free(current->key);
        free(current->value);
        free(current);
        current = next;
      }
    }

    free(dict->buckets);
    free(dict);
  }

  darr_free(dictionaries);
}

KeyValuePair *_dict_next_iter(Dictionary dictionary, KeyValuePair *current) {
  RESOLVE_DICT_STRUCT;

  if (!current) {
    for (size_t i = 0; i < dict->capacity; i++) {
      if (dict->buckets[i]) {
        dict->iter_current = (KeyValuePair *)dict->buckets[i];
        return dict->iter_current;
      }
    }
  } else {
    Entry *current_entry = (Entry *)current;
    Entry *next = current_entry->next;

    if (next) {
      dict->iter_current = (KeyValuePair *)next;
      return dict->iter_current;
    }

    size_t index = dict_key_index(dict->iter_current->key, dict->capacity);

    for (size_t i = index + 1; i < dict->capacity; i++) {
      if (dict->buckets[i]) {
        dict->iter_current = (KeyValuePair *)dict->buckets[i];
        return dict->iter_current;
      }
    }
  }

  return NULL;
}

KeyValuePair *_dict_start_iter(Dictionary dictionary) {
  RESOLVE_DICT_STRUCT;

  dict->iter_current = NULL;
  return _dict_next_iter(dictionary, NULL);
}
