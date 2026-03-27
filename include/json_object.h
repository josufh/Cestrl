#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include <stddef.h>

#include "json.h"
#include "json_value.h"

typedef struct JsonObjectEntry {
  const char *key;
  JsonValue *value;
  struct JsonObjectEntry *next;
} JsonObjectEntry;

typedef struct {
  JsonObjectEntry **buckets;
  size_t capacity;
  size_t count;
} JsonObject;

int json_object_set(JsonObject *object, const char *key, JsonValue *value);

#endif // !JSON_OBJECT_H
