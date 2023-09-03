#include "common.h"
#include "cli.h"

void put_error(const char *m, const char *filename)
{
    fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, filename, m);
    exit(1);
}

// Adds space between args and concatenates them to `buf`. Skips ones that start with '-'
// Returns `false` if size is exceeded
bool concat_args(int argc, char **argv, size_t size, char *buf)
{
    size_t k = 0;
    for (int i = 1; i < argc; ++i) {
        size_t len = strlen(argv[i]);

        if (argv[i][0] == '-')
            continue;
        if (k > size - 2)
            return false;
        if (i > 1)
            buf[k++] = ' ';

        for (size_t j = 0; j < len; ++j) {
            if (k > size - 2)
                return false;
            buf[k++] = argv[i][j];
        }
    }

    buf[k] = '\0';
    return true;
}

static inline void help(void)
{
    printf(
        "USAGE: %s [-options] <skin directory>\n"
        "Create @1x osu! skin elements from @2x elements. Works with `png` and `jpg`, "
        "outputs `png`.\n"
        "\n"
        "FLAGS:\n"
        "  -o, --overwrite\tOverwrite existing files.\n"
        "      --help     \tDisplay this menu.\n"
        "      --version  \tDisplay version.\n"
#ifndef NO_DIALOG
        "\nLaunch without arguments to use a GUI.\n"
#endif
        ,
        PROGRAM_NAME);
    exit(0);
}

static inline void version(void)
{
    printf(
        "%s %s"
#ifdef NO_DIALOG
        " no GUI"
#endif
        "\n(c) toiletbril %s\n",
        PROGRAM_NAME, VERSION, GITHUB);
    exit(0);
}

// Returns false if `str` is not a flag, otherwise sets global variables
bool set_flag(const char *str)
{
    // NOTE: If there is just '-' in argv it will be ignored
    if (str[0] != '-')
        return false;

    int len = strlen(str);

    for (int i = 1; i < len; ++i) {
        switch (str[i]) {
            case 'o': {
                flag_overwrite = true;
            } break;

            // Long arguments go here
            case '-': {
                if (strcmp(str, "--help") == 0) {
                    help();
                }
                else if (strcmp(str, "--overwrite") == 0) {
                    flag_overwrite = true;
                    return true;
                }
                else if (strcmp(str, "--version") == 0) {
                    version();
                }
                else {
                    put_error("Unknown option.", str);
                }
            } break;

            default: {
                put_error("Unknown option.", str);
            }
        }
    }

    return true;
}
