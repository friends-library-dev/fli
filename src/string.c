#include "string.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char* join_str(const char* start, const char* glue, const char* end) {
  char* joined = malloc(strlen(start) + strlen(glue) + strlen(end) + 1);
  sprintf(joined, "%s%s%s", start, glue, end);
  return joined;
}

char* path_append(const char* start, const char* end) {
  return join_str(start, "/", end);
}

bool same_str(const char* s1, const char* s2) {
  return strcmp(s1, s2) == 0;
}
