#include "package.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "shared.h"
#include "string.h"

typedef struct Deps {
  char* hash_to_name[MAX_PKGS];
  char* pkg_deps_names[MAX_PKGS][MAX_PKGS];
} Deps;

static bool is_dot_path(const char* path);
static bool is_dir(const char* path);
static void examine_subdir(
  char* root_path, char* subdir, Packages* pkgs, Deps* deps);
static Package* make_package(
  char* dirname, char* abspath, char* subdir, int hash, Deps* deps);

Packages* load_packages(char* root_path) {
  Deps deps = {0};
  Packages* pkgs = calloc(1, sizeof(Packages));
  pkgs->map = calloc(MAX_PKGS, sizeof(Package*));
  examine_subdir(root_path, "libs", pkgs, &deps);
  examine_subdir(root_path, "apps", pkgs, &deps);

  for (int hash = 0; hash < pkgs->num_pkgs; hash++) {
    Package* pkg = pkgs->map[hash];
    for (int i = 0; i < pkg->num_deps; i++) {
      char* pkg_name = deps.pkg_deps_names[hash][i];
      for (int dep_hash = 0; dep_hash < pkgs->num_pkgs; dep_hash++) {
        if (strcmp(pkg_name, deps.hash_to_name[dep_hash]) == 0) {
          pkg->deps[i] = pkgs->map[dep_hash];
        }
      }
    }
  }
  return pkgs;
}

static void examine_subdir(
  char* root_path, char* subdir, Packages* pkgs, Deps* deps) {
  char* path = path_append(root_path, subdir);
  DIR* dir = opendir(path);
  if (!dir)
    ABORT("unable to open `libs/` or `app/` dir");

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    char* abspath = path_append(path, entry->d_name);
    if (!is_dot_path(entry->d_name) && is_dir(path)) {
      int hash = pkgs->num_pkgs;
      Package* pkg = make_package(entry->d_name, abspath, subdir, hash, deps);
      if (pkg != NULL) {
        deps->hash_to_name[hash] = pkg->name;
        pkgs->map[hash] = pkg;
        pkgs->num_pkgs++;
      }
    }
  }
}

static Package* make_package(
  char* dirname, char* abspath, char* subdir, int hash, Deps* deps) {
  Package* pkg = calloc(1, sizeof(Package));
  const char* package_json_path = path_append(abspath, "package.json");
  FILE* package_json = fopen(package_json_path, "r");
  if (!package_json)
    return NULL;

  char line[MAX_LINE];
  char* name = NULL;
  char* version = NULL;
  char* dep = NULL;
  for (;;) {
    if (fgets(line, MAX_LINE, package_json) == NULL)
      break;

    if (strstr(line, "\"private\": true,"))
      return NULL;

    if (!name)
      name = line_extract(line, "  \"name\": \"", '"');

    if (!version)
      version = line_extract(line, "  \"version\": \"", '"');

    dep = line_extract(line, "    \"@friends-library/", '"');
    if (dep)
      deps->pkg_deps_names[hash][pkg->num_deps++] =
        join_str("@friends-library", "/", dep);
  }

  if (!name || !version)
    ABORT("required info missing from package.json");

  pkg->hash = hash;
  pkg->name = name;
  pkg->version = version;
  pkg->abspath = abspath;
  pkg->relpath = path_append(subdir, dirname);
  pkg->deps = calloc(pkg->num_deps, sizeof(Package*));
  return pkg;
}

void print_package(Package* pkg) {
  printf("\nPKG: `%s`\n", pkg->name);
  printf("  -> version: %s\n", pkg->version);
  printf("  -> abspath: %s\n", pkg->abspath);
  printf("  -> relpath: %s\n", pkg->relpath);
  printf("  -> num_deps: %d\n", pkg->num_deps);
  printf("  -> hash: %d\n", pkg->hash);
  printf("  -> dependencies:\n");
  for (int i = 0; i < pkg->num_deps; i++) {
    printf("    • %s\n", pkg->deps[i]->name);
  }
}

void print_packages(Packages* pkgs) {
  printf("Number of packages: %d\n", pkgs->num_pkgs);
  for (int i = 0; i < pkgs->num_pkgs; i++) {
    print_package(pkgs->map[i]);
  }
}

static bool is_dot_path(const char* path) {
  if (strlen(path) == 1 && path[0] == '.') {
    return true;
  }
  return strlen(path) == 2 && strncmp(path, "..", 2) == 0;
}

static bool is_dir(const char* path) {
  struct stat inode;
  return stat(path, &inode) == 0 && S_ISDIR(inode.st_mode);
}
