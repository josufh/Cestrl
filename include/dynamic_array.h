#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>

typedef struct {
  size_t length;
  size_t capacity;
} Header;

#define DARR_INIT_CAPACITY 8

#define darr_add(darr, x)                                                      \
  do {                                                                         \
    if ((darr) == NULL) {                                                      \
      Header *header =                                                         \
          malloc(sizeof(*darr) * DARR_INIT_CAPACITY + sizeof(Header));         \
      header->length = 0;                                                      \
      header->capacity = DARR_INIT_CAPACITY;                                   \
      (darr) = (void *)(header + 1);                                           \
    }                                                                          \
    Header *header = (Header *)(darr) - 1;                                     \
    if (header->length >= header->capacity) {                                  \
      header->capacity *= 2;                                                   \
      header = realloc(header,                                                 \
                       sizeof((*darr)) * header->capacity + sizeof(Header));   \
      darr = (void *)(header + 1);                                             \
    }                                                                          \
    (darr)[header->length++] = (x);                                            \
  } while (0)

#define darr_len(darr) ((Header *)(darr) - 1)->length

#define darr_free(darr) free((Header *)(darr) - 1)

#endif // !DYNAMIC_ARRAY_H
