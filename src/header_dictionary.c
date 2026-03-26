#include "../include/header_dictionary.h"

#include <stddef.h>
#include <string.h>

HeaderDictionary create_header_dictionary() {
  return (HeaderDictionary)dict_create();
}

int set_header(HeaderDictionary dictionary, const char *header,
               const char *value) {
  return dict_set(dictionary, header, value);
}

const char *get_header(HeaderDictionary dictionary, const char *header) {
  return dict_get(dictionary, header);
}

HeaderDictionary parse_new_headers(const char *headers_raw) {
  HeaderDictionary headers = create_header_dictionary();

  parse_append_headers(headers, headers_raw);

  return headers;
}

void parse_append_headers(HeaderDictionary headers, const char *headers_raw) {
  const char *line_start = headers_raw;
  const char *line_end;

  while ((line_end = strstr(line_start, "\r\n")) != NULL) {
    size_t line_length = line_end - line_start;

    if (line_length == 0)
      break;

    const char *colon_pos = memchr(line_start, ':', line_length);
    if (colon_pos) {
      size_t header_length = colon_pos - line_start;
      size_t value_length = line_length - header_length - 1;

      char header[header_length + 1];
      char value[value_length + 1];

      memcpy(header, line_start, header_length);
      header[header_length] = '\0';

      memcpy(value, colon_pos + 1, value_length);
      value[value_length] = '\0';

      char *value_trimmed = value;
      while (*value_trimmed == ' ') {
        value_trimmed++;
      }

      set_header(headers, header, value_trimmed);
    }

    line_start = line_end + 2;
  }
}
