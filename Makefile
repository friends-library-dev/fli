# TODO `-g` for debug?
FLAGS = -Wall -O -W -pedantic

.SILENT: fli

fli:
	clang -o dist/fli src/main.c $(FLAGS)
