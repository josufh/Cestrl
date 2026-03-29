#ifndef JSON_H
#define JSON_H

#include "json_value.h"

typedef struct {
  JsonValue *top_node;
} JsonDocument;

typedef void (*PrintFn)(JsonDocument *document);
typedef JsonDocument *(*DeserializeFn)(const char *json);

typedef struct {
  PrintFn Print;
  DeserializeFn Deserialize;
} JsonSerializerStruct;

extern const JsonSerializerStruct JsonSerializer;

#endif // !JSON_H
