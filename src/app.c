#include "../include/app.h"
#include "../include/header_dictionary.h"
#include "../include/internal/connection.h"
#include "../include/internal/network.h"

#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BACKLOG 64
#define MAX_EVENTS 64

static int epoll_add(int epfd, int fd, void *ptr, uint32_t events) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = events;
  ev.data.ptr = ptr;
  return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

static int epoll_mod(int epfd, int fd, void *ptr, uint32_t events) {
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = events;
  ev.data.ptr = ptr;
  return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}

static void connection_close(int epfd, Connection *conn) {
  epoll_ctl(epfd, EPOLL_CTL_DEL, conn->fd, NULL);
  close(conn->fd);
  free(conn);
}

static int read_full_request(const char *buffer) {
  const char *end = strstr(buffer, "\r\n\r\n");
  if (!end)
    return 0;
  return 1;
}

static void connection_on_readable(int epfd, Connection *conn) {
  for (;;) {
    size_t n = read(conn->fd, conn->read_buffer + conn->read_length,
                    READ_BUFFER_SIZE - conn->read_length);

    if (n > 0) {
      conn->read_length += n;

      if (read_full_request(conn->read_buffer)) {
        HeaderDictionary headers = parse_new_headers(conn->read_buffer);

        conn->state = CONN_STATE_WRITING;
        epoll_mod(epfd, conn->fd, conn, EPOLLOUT);
        return;
      }

      continue;
    }

    if (n == 0) {
      conn->state = CONN_STATE_CLOSING;
      return;
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK)
      return;

    conn->state = CONN_STATE_CLOSING;
    return;
  }
}

static void connection_on_writable(int epdf, Connection *conn) {
  const char *body = "<body><h1>Hello, World!</h1></body>";
  int body_length = (int)strlen(body);

  const char *response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/html\r\n"
                         "Content-Length: %d\r\n"
                         "\r\n"
                         "%s";

  conn->write_length = (size_t)snprintf(conn->write_buffer, WRITE_BUFFER_SIZE,
                                        response, body_length, body);
  size_t n = write(conn->fd, conn->write_buffer, conn->write_length);

  if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
    return;

  conn->state = CONN_STATE_CLOSING;
  return;
}

static void RunImpl(void) {
  int listenfd = create_listen_socket(PORT, BACKLOG);

  printf("App listening on port %d\n", PORT);

  int epfd = epoll_create1(0);
  if (epfd < 0) {
    perror("epoll_create1");
    exit(1);
  }

  struct epoll_event ev = {0};
  ev.events = EPOLLIN;
  ev.data.fd = listenfd;

  epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev);

  struct epoll_event events[MAX_EVENTS];

  while (1) {
    int n = epoll_wait(epfd, events, MAX_EVENTS, -1);

    for (int i = 0; i < n; i++) {
      if (events[i].data.fd == listenfd) {
        while (1) {
          int clientfd = accept(listenfd, NULL, NULL);
          if (clientfd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
              break;
            continue;
          }

          set_non_blocking(clientfd);

          Connection *conn = new_connection(clientfd);
          epoll_add(epfd, clientfd, conn, EPOLLIN);
        }
      } else {
        Connection *conn = events[i].data.ptr;

        if (events[i].events & (EPOLLERR | EPOLLHUP)) {
          conn->state = CONN_STATE_CLOSING;
        } else if (conn->state == CONN_STATE_READING &&
                   (events[i].events & EPOLLIN)) {
          connection_on_readable(epfd, conn);
        } else if (conn->state == CONN_STATE_WRITING &&
                   (events[i].events & EPOLLOUT)) {
          connection_on_writable(epfd, conn);
        }

        if (conn->state == CONN_STATE_CLOSING) {
          connection_close(epfd, conn);
        }
      }
    }
  }
}

WebApplication App = {.Run = RunImpl};
