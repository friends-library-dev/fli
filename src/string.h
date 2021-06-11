#ifndef __STRING_H__
#define __STRING_H__

#include <stdbool.h>

char* line_extract(const char* line, const char* match_prefix, char stop_ch);
char* path_join(const char* start, const char* end);
char* str_join(const char* start, const char* end);
bool str_is(const char* s1, const char* s2);
bool is_dir(const char* path);
bool is_symlink(const char* path);
bool is_symlink_dir(const char* path);

/**
 * Variadic path_join. Pass NULL to terminate args.
 */
char* path_joinv(const char* seg1, const char* seg2, ...);

#endif
