#include <stdlib.h>
#include "env.h"
#include "package.h"
#include "shared.h"

int main(void) {
  Packages* packages = load_packages(get_flroot());
  print_packages(packages);
  exit(EXIT_SUCCESS);
}
