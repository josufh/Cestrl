#ifndef APP_H
#define APP_H

typedef void (*RunFn)(void);

typedef struct {
  RunFn const Run;
} WebApplication;

extern WebApplication App;

#endif // !APP_H
