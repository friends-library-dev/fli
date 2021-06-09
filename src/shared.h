#ifndef __SHARED_H__
#define __SHARED_H__

#define MAX_LINE 256
#define MAX_PATH 256

#define ABORT(msg)      \
  do {                  \
    puts(msg);          \
    exit(EXIT_FAILURE); \
  } while (0)

#endif
