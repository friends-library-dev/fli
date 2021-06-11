#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#include <stdbool.h>

#define MAX_PKGS 50

typedef struct Package {
  char* name;
  char* version;
  char* abspath;
  char* relpath;
  int num_deps;
  bool is_app;
  bool is_lib;
  struct Package** deps;
  int hash;
} Package;

typedef struct Packages {
  int count;
  Package** map;
} Packages;

void print_package(Package* pkg);
void print_packages(Packages* pkgs);
Packages* load_packages(char* root_path);

#endif
