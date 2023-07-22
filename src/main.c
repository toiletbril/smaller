#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "smaller.h"
#include "gui.h"
#include "cli.h"

bool flag_overwrite = false;

int main(int argc, char **argv)
{
    if (argc < 2) {
        return use_dialog();
    }

    for (int i = 0; i < argc; ++i) {
        set_flag(argv[i]);
    }

    char dir_path[MAX_PATH];
    if (!concat_args(argc, argv, MAX_PATH, dir_path)) {
        fprintf(stderr, "%s: ERROR: Directory path is too long.\n",
                PROGRAM_NAME);

        exit(1);
    };

    // NOTE: This shows up even when folder does not exist or is a file
    if (!is_skin_folder(dir_path)) {
        put_error("Is not a skin folder", dir_path);
    }

    size_t files_created = 0;
    size_t files_skipped = 0;

    smaller_dir(dir_path, &files_created, &files_skipped);

    printf("%s: Successfully traversed '%s'. Files created: %zu, skipped: %zu.\n",
           PROGRAM_NAME, dir_path, files_created, files_skipped);
    return 0;
}
