#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <stdint.h>

typedef struct {
  char *Host;
  char *Method;
  
} HttpRequest;

typedef struct {
  HttpRequest Request;
} HttpContext;

#endif // !HTTP_REQUEST_H
