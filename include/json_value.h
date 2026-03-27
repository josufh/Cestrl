#ifndef JSON_VALUE_H
#define JSON_VALUE_H

typedef enum { String, Number, Object, Array, True, False, Null } JsonValueType;

typedef struct {
  JsonValueType type;
  void *value;
} JsonValue;

#endif // !JSON_VALUE_H
