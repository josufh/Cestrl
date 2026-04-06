#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include <stddef.h>

#include "json.h"
#include "json_value.h"

typedef struct {
  const char *key;
  JsonValue *value;
} JsonObjectEntryPair;

typedef struct JsonObjectEntry {
  const char *key;
  JsonValue *value;
  struct JsonObjectEntry *next;
} JsonObjectEntry;

typedef struct {
  JsonObjectEntry **buckets;
  size_t capacity;
  size_t count;
  JsonObjectEntryPair *iter_current;
} JsonObject;

int json_object_set(JsonObject **object, const char *key, JsonValue *value);

#define json_object_foreach(object, pair)                                      \
  for (JsonObjectEntryPair *pair = _entry_start_iter(object); pair;            \
       pair = _entry_next_iter(object, pair))

JsonObjectEntryPair *_entry_start_iter(JsonObject *object);
JsonObjectEntryPair *_entry_next_iter(JsonObject *object,
                                      JsonObjectEntryPair *current);

#endif // !JSON_OBJECT_H
