#ifndef CONNECTION_H
#define CONNECTION_H

#include <stddef.h>
#define READ_BUFFER_SIZE 8192
#define WRITE_BUFFER_SIZE 8192

typedef enum {
  CONN_STATE_READING,
  CONN_STATE_WRITING,
  CONN_STATE_CLOSING,
} ConnectionState;

typedef struct {
  int fd;
  ConnectionState state;

  char read_buffer[READ_BUFFER_SIZE];
  size_t read_length;

  char write_buffer[WRITE_BUFFER_SIZE];
  size_t write_length;
  size_t write_offset;
} Connection;

Connection *new_connection(int fd);

#endif // !CONNECTION_H
