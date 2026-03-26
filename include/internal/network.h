#ifndef NETWORK_H
#define NETWORK_H

int set_non_blocking(int fd);
int create_listen_socket(int port, int backlog);

#endif // !NETWORK_H
