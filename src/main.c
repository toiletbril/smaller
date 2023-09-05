#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "smaller.h"
#include "cli.h"

#ifndef NO_DIALOG
    #include "gui.h"
#endif

bool flag_overwrite = false;
bool flag_verbose = false;

size_t files_created = 0;
size_t files_skipped = 0;

int main(int argc, char **argv)
{
    bool has_args = false;
    for (int i = 1; i < argc; ++i) {
        has_args |= !set_flag(argv[i]);
    }

    if (!has_args) {
#ifndef NO_DIALOG
        return dialog_main();
#else
        put_and_die("Not enough arguments. Try '--help' for more information.");
#endif
    }

    char dir_path[MAX_PATH];
    if (!concat_args(argc, argv, MAX_PATH, dir_path)) {
        put_and_die("Directory path is too long.");
    }

    if (!is_skin_folder(dir_path)) {
        put_item_and_die("Is not a skin folder", dir_path);
    }

    put_message("Traversing directory '%s'...\n", dir_path);

    smaller_dir(dir_path);

    put_message("Successfully traversed '%s'. Files created: %zu, skipped: %zu.\n",
                dir_path, files_created, files_skipped);
    return 0;
}
