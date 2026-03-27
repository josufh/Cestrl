#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

typedef struct {
  char *string;
} StringBuilder;

StringBuilder *sb_new();
StringBuilder *sb_news(const char *value);
void sb_appendc(StringBuilder *sb, char c);
void sb_appends(StringBuilder *sb, const char *s);
const char *sb_to_string(StringBuilder *sb);
void sb_free(StringBuilder *sb);

#endif // !STRING_BUILDER_H
