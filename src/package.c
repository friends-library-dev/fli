#include "package.h"
#include <stdio.h>
#include <stdlib.h>

void print_package(Package* pkg) {
  printf("PKG: %s\n", pkg->name);
}

void print_packages(Packages* pkgs) {
  printf("Number of packages: %d\n", pkgs->num_pkgs);
  for (int i = 0; i < pkgs->num_pkgs; i++) {
    print_package(&pkgs->pkgs[i]);
  }
}

Packages* load_packages(char* root_dir) {
  Packages* pkgs = calloc(1, sizeof pkgs);
  return pkgs;
}
