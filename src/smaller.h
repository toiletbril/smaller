#ifndef SMALLER_H
#define SMALLER_H

#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "smaller.h"
#include "cli.h"

extern bool flag_overwrite;
extern bool flag_verbose;

extern size_t files_created;
extern size_t files_skipped;

void smaller_dir(const char *dir_path);
bool is_skin_folder(const char *dir_path);
bool file_exists(const char *filepath);

#endif // SMALLER_H
