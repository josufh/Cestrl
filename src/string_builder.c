#include "../include/string_builder.h"
#include "../include/dynamic_array.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

StringBuilder *sb_new() { return malloc(sizeof(StringBuilder)); }

void sb_appendc(StringBuilder *sb, char c) {
  if (!sb)
    sb = sb_new();
  darr_add(sb->string, c);
}

void sb_appends(StringBuilder *sb, const char *s) {
  if (!sb)
    sb = sb_news(s);
  else
    for (const char *c = s; *c != '\0'; c++) {
      sb_appendc(sb, *c);
    }
}

StringBuilder *sb_news(const char *value) {
  StringBuilder *sb = sb_new();
  sb_appends(sb, value);
  return sb;
}

const char *sb_to_string(StringBuilder *sb) {
  if (!sb)
    return NULL;

  size_t length;

  if (!sb->string)
    length = 0;
  else
    length = darr_len(sb->string);

  char *string = malloc(length + 1);
  memcpy(string, sb->string, length);
  string[length] = '\0';

  return string;
}

void sb_free(StringBuilder *sb) {
  if (!sb)
    return;

  darr_free(sb->string);
  free(sb);
}
