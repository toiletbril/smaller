#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

extern bool flag_overwrite;
extern bool flag_verbose;

#define put_message(...) printf(PROGRAM_NAME ": " __VA_ARGS__)

void put_item_and_die(const char *m, const char *item);
void put_and_die(const char *m);

bool concat_args(int argc, char **argv, size_t size, char *buf);
bool set_flag(const char *str);

#endif // CLI_H
