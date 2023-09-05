#ifndef NO_DIALOG

#include "gui.h"

#define NOC_FILE_DIALOG_IMPLEMENTATION

#ifdef _WIN32
    #define NOC_FILE_DIALOG_WIN32
#else
    #define NOC_FILE_DIALOG_GTK
#endif

#include "noc_file_dialog.h"

#define FLAGS (NOC_FILE_DIALOG_OPEN | NOC_FILE_DIALOG_OVERWRITE_CONFIRMATION)

static bool get_dir(const char *file_path, char *buf, size_t size)
{
    for (size_t i = strlen(file_path); i > 0; --i) {
        for (size_t j = 0; j < strlen(DIR_SEP); ++j) {
            if (file_path[i] == DIR_SEP[j] && size >= i + 1) {
                memcpy(buf, file_path, i * sizeof(char));
                buf[i] = '\0';
                return true;
            }
        }
    }

    return false;
}

int dialog_main(void)
{
    const char *skin_ini_path = noc_file_dialog_open(FLAGS, "skin.ini\0*.ini", ".", PROGRAM_NAME);

    if (skin_ini_path == NULL) {
        put_and_die("No file selected.");
    }

    char dir_path[MAX_PATH];
    if (!get_dir(skin_ini_path, dir_path, MAX_PATH)) {
        put_and_die("Directory path is too long.");
    }

    if (is_skin_folder(dir_path)) {
        smaller_dir(dir_path);

        put_message("Successfully traversed '%s'. Files created: %zu, skipped: %zu.\n",
                dir_path, files_created, files_skipped);

        return 0;
    } else {
        put_item_and_die("Is not a skin folder", dir_path);
    }

    return 1;
}

#endif // NO_DIALOG
