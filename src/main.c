#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ABORT(msg)      \
  do {                  \
    puts(msg);          \
    exit(EXIT_FAILURE); \
  } while (0)

#define MAX_PKGS 50
#define MAX_LINE 256
#define MAX_PATH 256

typedef struct Package {
  char* name;
  char* version;
  char* abspath;
  char* relpath;
  int num_deps;
  char deps[MAX_PKGS];
  int hash;
} Package;

typedef struct Packages {
  int num_pkgs;
  Package pkgs[MAX_PKGS];
} Packages;

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

char* cwd(void);
char* get_flroot(void);

int main(void) {
  Packages* packages = load_packages(get_flroot());
  print_packages(packages);
  return 0;
}

char* get_flroot(void) {
  char bashrc_path[MAX_PATH];
  sprintf(bashrc_path, "/Users/%s/.bashrc", getenv("USER"));

  FILE* bashrc = fopen(bashrc_path, "r");
  if (!bashrc)
    ABORT("Error loading ~/.bashrc");

  char line[MAX_LINE];
  for (;;) {
    if (fgets(line, MAX_LINE, bashrc) == NULL)
      ABORT("unable to find FLROOT in ~/.bashrc");

    if (strncmp(line, "FLROOT=\"", 8) != 0)
      continue;

    char* flroot = calloc(1, MAX_PATH);
    for (char *lp = &line[8], *fp = flroot; *lp != '"'; fp++, lp++) {
      *fp = *lp;
    }
    return flroot;
  }
  ABORT("unable to find FLROOT in ~/.bashrc");
}

char* cwd(void) {
  static char* cwd = NULL;
  if (cwd)
    return cwd;
  cwd = getcwd(NULL, 256);
  if (cwd)
    return cwd;
  ABORT("getcwd err");
}
