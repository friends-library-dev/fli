#ifndef __SHARED_H__
#define __SHARED_H__

#include "colors.h"

#define MAX_LINE 256
#define MAX_PATH 256

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"

#define ABORT(msg, ...)         \
  do {                          \
    printf(C_RED "\nABORT: ");  \
    printf(msg, ##__VA_ARGS__); \
    printf("%s", "\n\n");       \
    exit(EXIT_FAILURE);         \
  } while (0)

#pragma clang diagnostic pop

#endif
