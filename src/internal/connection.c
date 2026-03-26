#include "../../include/internal/connection.h"
#include <stdlib.h>
#include <string.h>

Connection *new_connection(int fd) {
  Connection *connection = malloc(sizeof(Connection));
  memset(connection, 0, sizeof(Connection));
  connection->fd = fd;
  connection->state = CONN_STATE_READING;
  return connection;
}
