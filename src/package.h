#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#include <stdbool.h>

#define MAX_PKGS 50

typedef struct pkg_t {
  char* name;
  char* version;
  char* abspath;
  char* relpath;
  int num_deps;
  bool is_app;
  bool is_lib;
  struct pkg_t** deps;
  int hash;
} pkg_t;

typedef struct pkgs_t {
  int count;
  pkg_t** map;
} pkgs_t;

void print_package(pkg_t* pkg);
void print_packages(pkgs_t* pkgs);
pkgs_t* load_packages(char* root_path);

#endif
