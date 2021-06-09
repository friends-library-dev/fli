#ifndef __STRING_H__
#define __STRING_H__

#include <stdbool.h>

char* line_extract(const char* line, const char* match_prefix, char stop_ch);
char* path_append(const char* start, const char* end);
char* join_str(const char* start, const char* glue, const char* end);
bool same_str(const char* s1, const char* s2);

#endif
