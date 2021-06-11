#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
void symlink_package(pkg_t* pkg);
void unsymlink_package(pkg_t* pkg);

int main(int argc, char** argv) {
  opts_t opts = parse_argv(argc, argv);
  pkgs_t* packages = load_packages(get_flroot());
  if (!packages)
    ABORT("Error loading packages from FLROOT");

  if (opts.print) {
    print_packages(packages);
    exit(EXIT_SUCCESS);
  }

  for (int i = 0; i < packages->count; i++) {
    pkg_t* pkg = packages->map[i];
    if (!opts.cwd_only || str_is(pkg->abspath, cwd())) {
      opts.unlink ? unsymlink_package(pkg) : symlink_package(pkg);
    }
  }

  exit(EXIT_SUCCESS);
}

void unsymlink_package(pkg_t* pkg) {
  if (str_is(pkg->name, "@friends-library/native"))
    return;

  printf(C_GREY "Un-symlinking deps for package: %s%s\n", C_GREEN, pkg->name);
  for (int i = 0; i < pkg->num_deps; i++) {
    pkg_t* dep = pkg->deps[i];
    char* dep_path = path_joinv(pkg->abspath, "node_modules", dep->name, NULL);
    char* bak_path = str_join(dep_path, ".bak");

    if (is_symlink(dep_path) && unlink(dep_path))
      ABORT("unable to remove existing symlink at `%s`\n", dep_path);

    if (is_dir(bak_path) && rename(bak_path, dep_path))
      ABORT("unable to restore non-symlink dir at `%s`\n", bak_path);
  }
}

void symlink_package(pkg_t* pkg) {
  if (str_is(pkg->name, "@friends-library/native"))
    return;

  printf(C_GREY "Symlinking deps for package: %s%s\n", C_GREEN, pkg->name);
  for (int i = 0; i < pkg->num_deps; i++) {
    pkg_t* dep = pkg->deps[i];
    char* dep_path = path_joinv(pkg->abspath, "node_modules", dep->name, NULL);

    // step 1, rename any existing, non-symlink npm-install-ed dir
    if (is_dir(dep_path) && !is_symlink(dep_path))
      if (rename(dep_path, str_join(dep_path, ".bak")))
        ABORT("non-symlink rename of `%s` failed", dep_path);

    // step 2, if there is already a symlink, just delete it
    if (is_symlink(dep_path) && unlink(dep_path))
      ABORT("unable to remove existing symlink at `%s`\n", dep_path);

    // sanity check - target should not exist at this point
    if (is_dir(dep_path))
      ABORT("unexpected dir remaining at `%s`\n", dep_path);

    // create the symlink
    if (symlink(dep->abspath, dep_path))
      ABORT("error creating symblink %s -> %s\n", dep->abspath, dep_path);
  }
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
