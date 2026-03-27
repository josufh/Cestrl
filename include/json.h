#ifndef JSON_H
#define JSON_H

#include "json_value.h"

typedef struct {
  JsonValue top_node;
} JsonDocument;

typedef JsonDocument *(*DeserializeFn)(const char *json);

typedef struct {
  DeserializeFn Deserialize;
} JsonSerializerStruct;

extern const JsonSerializerStruct JsonSerializer;

#endif // !JSON_H
