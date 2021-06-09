# TODO `-g` for debug?
FLAGS = -Wall -O -W -pedantic

.SILENT: fli

fli:
	clang -o dist/fli src/main.c src/string.c src/env.c src/package.c $(FLAGS)
