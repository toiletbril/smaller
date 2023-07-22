#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern bool flag_overwrite;

// Print out error and exit.
void put_error(const char *m, const char *filename);
// Contatenate argv into a single buffer.
bool concat_args(int argc, char **argv, size_t size, char *buf);
// Set global state if flag matches.
bool set_flag(const char *str);

#endif // CLI_H
