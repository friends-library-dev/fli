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

typedef struct Flags {
  bool print;
} Flags;

Flags parse_argv(int argc, char** argv);
void symlink_package(Package* pkg);

int main(int argc, char** argv) {
  Flags flags = parse_argv(argc, argv);
  Packages* packages = load_packages(get_flroot());
  if (!packages)
    ABORT("Error loading packages from FLROOT");

  if (flags.print) {
    print_packages(packages);
    exit(EXIT_SUCCESS);
  }

  for (int i = 0; i < packages->count; i++) {
    symlink_package(packages->map[i]);
  }

  exit(EXIT_SUCCESS);
}

void symlink_package(Package* pkg) {
  if (str_is(pkg->name, "@friends-library/native"))
    return;

  printf(C_GREY "Symlinking deps for package: %s%s\n", C_GREEN, pkg->name);
  for (int i = 0; i < pkg->num_deps; i++) {
    Package* dep = pkg->deps[i];
    char* dep_path = path_joinv(pkg->abspath, "node_modules", dep->name, NULL);

    // step 1, rename any existing, non-symlink npm-install-ed dir
    if (is_dir(dep_path) && !is_symlink(dep_path))
      if (rename(dep_path, str_join(dep_path, ".bak")) != 0)
        ABORT("non-symlink rename of `%s` failed", dep_path);

    // step 2, if there is already a symlink, just delete it
    if (is_symlink(dep_path) && unlink(dep_path))
      ABORT("unable to remove existing symlink at `%s`\n", dep_path);

    // sanity check - target should not exist at this point
    if (is_dir(dep_path))
      ABORT("unexpected dir remaining at `%s`\n", dep_path);

    // create the symlink
    if (symlink(dep->abspath, dep_path) != 0)
      ABORT("error creating symblink %s -> %s\n", dep->abspath, dep_path);
  }
}

Flags parse_argv(int argc, char** argv) {
  Flags flags = {0};
  for (int i = 1; i < argc; i++) {
    if (str_is(argv[i], "--print") || str_is(argv[i], "-p")) {
      flags.print = true;
    }
  }
  return flags;
}
