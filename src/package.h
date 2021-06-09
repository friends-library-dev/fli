#ifndef __PACKAGE_H__
#define __PACKAGE_H__

#define MAX_PKGS 50

typedef struct Package {
  char* name;
  char* version;
  char* abspath;
  char* relpath;
  int num_deps;
  struct Package** deps;
  int hash;
} Package;

typedef struct Packages {
  int num_pkgs;
  Package** map;
} Packages;

void print_package(Package* pkg);
void print_packages(Packages* pkgs);
Packages* load_packages(char* root_path);

#endif
