#include "../include/json.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/json_object.h"
#include "../include/string_builder.h"

#define JSON_SPACE 0x20
#define JSON_LF 0x0A
#define JSON_CR 0x0D
#define JSON_HT 0x09

static JsonValue *parse_value(const char **json_raw);

static bool is_whitespace(const char c) {
  return c == JSON_SPACE || c == JSON_LF || c == JSON_CR || c == JSON_HT;
}

static void skip_whitespace(const char **json_raw) {
  while (is_whitespace(**json_raw))
    (*json_raw)++;
}

static JsonValueType get_value_type(const char **json_raw) {
  switch (**json_raw) {
  case '"':
    return String;
  case '{':
    return Object;
  case '[':
    return Array;
  case 't':
    return True;
  case 'f':
    return False;
  case 'n':
    return Null;
  default:
    return Number;
  }
}

static const char *parse_string(const char **json_raw) {
  StringBuilder *sb = sb_new();

  (*json_raw)++;

  while (**json_raw != '"' && **json_raw != '\0') {
    if (**json_raw == '\\') {
      (*json_raw)++;
      if (**json_raw == '\0') {
        break;
      }
    }
    sb_appendc(sb, **json_raw);
    (*json_raw)++;
  }

  if (**json_raw != '"') {
    fprintf(stderr, "Expected '\"' at the end of a string\n");
    exit(EXIT_FAILURE);
  }

  (*json_raw)++;

  return sb_to_string(sb);
}

static bool is_part_of_number(const char c) {
  return c == '-' || c == '+' || (c >= '0' && c <= '9') || c == 'e' ||
         c == 'E' || c == '.';
}

// TODO number validation
static double parse_number(const char **json_raw) {
  const char *c = *json_raw;
  while (is_part_of_number(*c)) {
    c++;
  }

  char *end = NULL;
  double number = strtod(*json_raw, &end);

  if (end != c) {
    fprintf(stderr, "Invalid number found\n");
    exit(EXIT_FAILURE);
  }

  *json_raw = c;

  return number;
}

static JsonObject *parse_object(const char **json_raw) {
  JsonObject *object = NULL;

  // Skip {
  (*json_raw)++;

  while (**json_raw != '}') {
    skip_whitespace(json_raw);
    const char *string = parse_string(json_raw);
    skip_whitespace(json_raw);

    if (**json_raw != ':') {
      fprintf(stderr, "Expected ':'\n");
      exit(EXIT_FAILURE);
    }
    (*json_raw)++; // Skip :

    JsonValue *value = parse_value(json_raw);
    json_object_set(object, string, value);

    if (**json_raw == ',')
      (*json_raw)++;
  }

  return object;
}

static void *alloc_number() { return malloc(sizeof(double)); }

static JsonValue *new_node(JsonValueType type, void *value) {
  JsonValue *node = malloc(sizeof(JsonValue));
  node->type = type;

  switch (type) {
  case String:
    node->value = value;
  case Number:
    node->value = alloc_number();
    memcpy(node->value, value, sizeof(double));
  default:
    fprintf(stderr, "Not implemented new_node type\n");
    exit(EXIT_FAILURE);
  }

  return node;
}

static JsonValue *parse_value(const char **json_raw) {
  skip_whitespace(json_raw);
  JsonValueType type = get_value_type(json_raw);
  JsonValue *node = NULL;

  if (type == String) {
    const char *string = parse_string(json_raw);
    node = new_node(type, (void *)string);
  } else if (type == Number) {
    double number = parse_number(json_raw);
    node = new_node(type, &number);
  } else if (type == Object) {
    JsonObject *object = parse_object(json_raw);
  }

  skip_whitespace(json_raw);

  return node;
}

static void parse_json(const char *json_raw) { parse_value(&json_raw); }

static JsonDocument *DeserializeImpl(const char *json) {
  parse_json(json);
  return NULL;
}

const JsonSerializerStruct JsonSerializer = {.Deserialize = DeserializeImpl};
