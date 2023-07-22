#ifndef SMALLER_H
#define SMALLER_H

#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "smaller.h"
#include "cli.h"

// Traverse a folder.
void smaller_dir(const char *dir_path, size_t *files_created, size_t *files_skipped);

// Check if a folder contains 'skin.ini' file.
bool is_skin_folder(const char *dir_path);

bool file_exists(const char *filepath);

#endif // SMALLER_H
