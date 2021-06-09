#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shared.h"

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
