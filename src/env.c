#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shared.h"
#include "string.h"

char* get_flroot(void) {
  char bashrc_path[MAX_PATH];
  sprintf(bashrc_path, "/Users/%s/.bashrc", getenv("USER"));

  FILE* bashrc = fopen(bashrc_path, "r");
  if (!bashrc)
    ABORT("Error loading ~/.bashrc");

  char line[MAX_LINE];
  char* flroot = NULL;
  for (;;) {
    if (fgets(line, MAX_LINE, bashrc) == NULL)
      break;

    flroot = line_extract(line, "FLROOT=\"", '"');
    if (flroot)
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
