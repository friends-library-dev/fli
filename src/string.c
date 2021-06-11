#include "string.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "shared.h"

char* line_extract(const char* line, const char* match_prefix, char stop_ch) {
  int len = strlen(match_prefix);
  if (strncmp(line, match_prefix, len) != 0)
    return NULL;

  char* extracted = calloc(MAX_LINE, sizeof(char));
  for (int i = 0; line[i + len] != stop_ch; i++) {
    extracted[i] = line[i + len];
  }

  return extracted;
}

char* str_join(const char* start, const char* end) {
  char* joined = malloc(strlen(start) + strlen(end) + 1);
  sprintf(joined, "%s%s", start, end);
  return joined;
}

char* path_join(const char* start, const char* end) {
  char* joined = malloc(strlen(start) + strlen(end) + 2);
  sprintf(joined, "%s/%s", start, end);
  return joined;
}

char* path_joinv(const char* seg1, const char* seg2, ...) {
  char* path = path_join(seg1, seg2);

  va_list ap;
  va_start(ap, seg2);
  char* next_seg;

  for (;;) {
    next_seg = va_arg(ap, char*);
    if (next_seg == NULL) {
      va_end(ap);
      return path;
    }
    path = path_join(path, next_seg);
  }
}

bool str_is(const char* s1, const char* s2) {
  return strcmp(s1, s2) == 0;
}

bool is_dir(const char* path) {
  struct stat sb;
  return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

bool is_symlink(const char* path) {
  struct stat sb;
  return lstat(path, &sb) == 0 && S_ISLNK(sb.st_mode);
}

bool is_symlink_dir(const char* path) {
  return is_dir(path) && is_symlink(path);
}
