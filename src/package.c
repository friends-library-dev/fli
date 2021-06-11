#include "package.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shared.h"
#include "string.h"

typedef struct Deps {
  char* hash_to_name[MAX_PKGS];
  char* pkg_deps_names[MAX_PKGS][MAX_PKGS];
} Deps;

static bool is_dot_path(const char* path);
static void examine_subdir(
  char* root_path, char* subdir, Packages* pkgs, Deps* deps);
static Package* make_package(
  char* dirname, char* abspath, char* subdir, int hash, Deps* deps);

Packages* load_packages(char* root_path) {
  Deps deps = {0};
  Packages* pkgs = calloc(1, sizeof(Packages));
  pkgs->map = calloc(MAX_PKGS, sizeof(Package*));

  // first pass over all libs
  examine_subdir(root_path, "libs", pkgs, &deps);
  examine_subdir(root_path, "apps", pkgs, &deps);

  // now that we have seen all libs, fill in references to deps
  for (int hash = 0; hash < pkgs->count; hash++) {
    Package* pkg = pkgs->map[hash];
    for (int i = 0; i < pkg->num_deps; i++) {
      char* pkg_name = deps.pkg_deps_names[hash][i];
      for (int dep_hash = 0; dep_hash < pkgs->count; dep_hash++) {
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
  char* path = path_join(root_path, subdir);
  DIR* dir = opendir(path);
  if (!dir)
    ABORT("unable to open `%s/` subdir", subdir);

  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {
    char* abspath = path_join(path, entry->d_name);
    if (!is_dot_path(entry->d_name) && is_dir(abspath)) {
      int hash = pkgs->count;
      Package* pkg = make_package(entry->d_name, abspath, subdir, hash, deps);
      if (pkg != NULL) {
        deps->hash_to_name[hash] = pkg->name;
        pkgs->map[hash] = pkg;
        pkgs->count++;
      }
    }
  }
}

static Package* make_package(
  char* dirname, char* abspath, char* subdir, int hash, Deps* deps) {
  FILE* package_json = fopen(path_join(abspath, "package.json"), "r");
  if (!package_json)
    return NULL;

  Package* pkg = calloc(1, sizeof(Package));
  pkg->is_app = false;
  pkg->is_lib = true;

  char line[MAX_LINE];
  char* name = NULL;
  char* version = NULL;
  char* dep = NULL;

  for (;;) {
    if (fgets(line, MAX_LINE, package_json) == NULL)
      break;

    if (line_extract(line, "  \"private\": true", ',')) {
      pkg->is_lib = false;
      pkg->is_app = true;
    }

    if (!name)
      name = line_extract(line, "  \"name\": \"", '"');

    if (!version)
      version = line_extract(line, "  \"version\": \"", '"');

    dep = line_extract(line, "    \"@friends-library/", '"');
    if (dep)
      deps->pkg_deps_names[hash][pkg->num_deps++] =
        str_join("@friends-library/", dep);
  }

  if (!name)
    ABORT("name not found for pkg at path `%s`", abspath);

  pkg->hash = hash;
  pkg->name = name;
  pkg->version = version ? version : strdup("<none>");
  pkg->abspath = abspath;
  pkg->relpath = path_join(subdir, dirname);
  pkg->deps = calloc(pkg->num_deps, sizeof(Package*));
  return pkg;
}

void print_package(Package* pkg) {
  printf("\nPKG: `%s`\n", pkg->name);
  printf("  -> type: %s\n", pkg->is_lib ? "LIB" : "APP");
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
  printf("Number of packages: %d\n", pkgs->count);
  for (int i = 0; i < pkgs->count; i++) {
    print_package(pkgs->map[i]);
  }
}

static bool is_dot_path(const char* path) {
  if (strlen(path) == 1 && path[0] == '.') {
    return true;
  }
  return strlen(path) == 2 && strncmp(path, "..", 2) == 0;
}
