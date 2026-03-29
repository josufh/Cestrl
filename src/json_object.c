#include "../include/json_object.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "../include/hash.h"

#define OBJECT_INIT_CAPACITY 1
#define OBJECT_MAX_LOAD_FACTOR 0.75

static size_t object_key_index(const char *key, size_t capacity) {
  unsigned long hash = djb2(key);
  return (size_t)(hash % capacity);
}

static int object_rehash(JsonObject *object, size_t new_capacity) {
  JsonObjectEntry **new_buckets;
  size_t i;

  new_buckets =
      (JsonObjectEntry **)calloc(new_capacity, sizeof(JsonObjectEntry *));
  if (!new_buckets)
    return 0;

  for (i = 0; i < object->capacity; i++) {
    JsonObjectEntry *current = object->buckets[i];

    while (current) {
      JsonObjectEntry *next = current->next;

      size_t index = object_key_index(current->key, new_capacity);

      current->next = new_buckets[index];
      new_buckets[index] = current;

      current = next;
    }
  }

  free(object->buckets);
  object->buckets = new_buckets;
  object->capacity = new_capacity;

  return 1;
}

static int object_maybe_grow(JsonObject *object) {
  double load_factor = (double)object->count / (double)object->capacity;

  if (load_factor >= OBJECT_MAX_LOAD_FACTOR) {
    size_t new_capacity = object->capacity * 2;

    if (new_capacity == 0)
      new_capacity = OBJECT_INIT_CAPACITY;

    return object_rehash(object, new_capacity);
  }

  return 1;
}

static JsonObjectEntry *object_create_entry(const char *key, JsonValue *value) {
  JsonObjectEntry *entry;

  entry = (JsonObjectEntry *)malloc(sizeof(JsonObjectEntry));
  if (!entry)
    return NULL;

  entry->key = key;
  entry->value = value;
  entry->next = NULL;

  return entry;
}

int json_object_set(JsonObject *object, const char *key, JsonValue *value) {
  if (!object) {
    object = malloc(sizeof(JsonObject));

    object->buckets = (JsonObjectEntry **)calloc(OBJECT_INIT_CAPACITY,
                                                 sizeof(JsonObjectEntry *));
    if (!object->buckets) {
      free(object);
      exit(1);
    }
    object->capacity = OBJECT_INIT_CAPACITY;
    object->count = 0;
  }

  if (!key || !value)
    return 0;

  size_t index = object_key_index(key, object->capacity);
  JsonObjectEntry *current = object->buckets[index];

  while (current) {
    if (strcmp(current->key, key) == 0) {
      JsonValue *new_value = value;

      free(current->value);
      current->value = new_value;
      return 1;
    }

    current = current->next;
  }

  if (!object_maybe_grow(object)) {
    return 0;
  }

  index = object_key_index(key, object->capacity);

  current = object_create_entry(key, value);
  if (!current)
    return 0;

  current->next = object->buckets[index];
  object->buckets[index] = current;
  object->count++;

  return 1;
}

JsonObjectEntryPair *_entry_start_iter(JsonObject *object) {}

JsonObjectEntryPair *_entry_next_iter(JsonObject *object,
                                      JsonObjectEntryPair *current) {}
