#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "colors.h"
#include "env.h"
#include "package.h"
#include "shared.h"
#include "string.h"

typedef struct opts_t {
  bool print;
  bool cwd_only;
  bool unlink;
} opts_t;

opts_t parse_argv(int argc, char** argv);
void process_pkg(pkg_t* pkg, pkgs_t* pkgs, bool unlink);
void symlink_pkg(const char* dep_path, const char* bak_path, pkg_t* dep);
void unsymlink_pkg(const char* dep_path, const char* bak_path);

int main(int argc, char** argv) {
  opts_t opts = parse_argv(argc, argv);
  pkgs_t* pkgs = load_packages(get_flroot());
  if (!pkgs)
    ABORT("Error loading packages from FLROOT");

  if (opts.print) {
    print_packages(pkgs);
    exit(EXIT_SUCCESS);
  }

  for (int i = 0; i < pkgs->count; i++) {
    pkg_t* pkg = pkgs->map[i];
    if (!opts.cwd_only || str_is(pkg->abspath, cwd())) {
      process_pkg(pkg, pkgs, opts.unlink);
    }
  }

  exit(EXIT_SUCCESS);
}

void process_pkg(pkg_t* pkg, pkgs_t* pkgs, bool unlink) {
  if (str_is(pkg->name, "@friends-library/native"))
    return;

  if (pkg->num_deps == 0)
    return;

  char* node_modules = path_join(pkg->abspath, "node_modules/@friends-library");
  if (!is_dir(node_modules) && mkdir(node_modules, 0755))
    ABORT("error creating missing dir `%s`", node_modules);

  printf(C_GREY "%symlinking deps for package: %s%s\n" C_RESET,
    unlink ? "Un-s" : "S", C_GREEN, pkg->name);

  // instead of trying to recursively descend thru the whole dependency graph
  // we just symlink ALL the @friends-library/* deps - much easier & no downside
  for (int i = 0; i < pkgs->count; i++) {
    pkg_t* dep = pkgs->map[i];
    if (pkg->hash == dep->hash || dep->is_app) {
      continue;
    }

    char* dep_path = path_joinv(pkg->abspath, "node_modules", dep->name, NULL);
    char* bak_path = str_join(dep_path, ".bak");

    if (unlink)
      unsymlink_pkg(dep_path, bak_path);
    else
      symlink_pkg(dep_path, bak_path, dep);
  }
}

void unsymlink_pkg(const char* dep_path, const char* bak_path) {
  if (is_symlink(dep_path) && unlink(dep_path))
    ABORT("unable to remove existing symlink at `%s`\n", dep_path);

  if (is_dir(bak_path) && rename(bak_path, dep_path))
    ABORT("unable to restore non-symlink dir at `%s`\n", bak_path);
}

void symlink_pkg(const char* dep_path, const char* bak_path, pkg_t* dep) {
  // step 1, rename any existing, non-symlink npm-install-ed dir
  if (is_dir(dep_path) && !is_symlink(dep_path))
    if (rename(dep_path, bak_path))
      ABORT("non-symlink rename of `%s` failed", dep_path);

  // step 2, if there is already a symlink, just delete it
  if (is_symlink(dep_path) && unlink(dep_path))
    ABORT("unable to remove existing symlink at `%s`\n", dep_path);

  // sanity check - target should not exist at this point
  if (is_dir(dep_path))
    ABORT("unexpected dir remaining at `%s`\n", dep_path);

  // create the symlink
  if (symlink(dep->abspath, dep_path))
    ABORT("error creating symlink %s -> %s\n", dep->abspath, dep_path);
}

opts_t parse_argv(int argc, char** argv) {
  opts_t opts = {0};
  for (int i = 1; i < argc; i++) {
    char* arg = argv[i];
    if (str_is(arg, "--print") || str_is(arg, "-p")) {
      opts.print = true;
    } else if (str_is(arg, "--unlink") || str_is(arg, "-u")) {
      opts.unlink = true;
    } else if (str_is(arg, ".")) {
      opts.cwd_only = true;
    }
  }
  return opts;
}
