#include "common.h"
#include "cli.h"

void put_item_and_die(const char *m, const char *item)
{
    fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, item, m);
    exit(1);
}

void put_and_die(const char *m)
{
    fprintf(stderr, "%s: %s\n", PROGRAM_NAME, m);
    exit(1);
}

// Returns `false` if size is exceeded
bool concat_args(int argc, char **argv, size_t size, char *buf)
{
    size_t k = 0;
    bool prev_found = false;

    for (int i = 1; i < argc; ++i) {
        size_t len = strlen(argv[i]);

        if (argv[i][0] == '-')
            continue;
        if (k > size - 2)
            return false;
        if (prev_found) {
            buf[k++] = ' ';
            prev_found = false;
        }

        for (size_t j = 0; j < len; ++j) {
            if (k > size - 2)
                return false;
            prev_found = true;
            buf[k++] = argv[i][j];
        }

    }

    buf[k] = '\0';
    return true;
}

static inline void put_help_and_die(void)
{
    printf(
        "USAGE\n"
        "  %s [-options] <skin directory>\n"
        "  Create @1x osu! skin elements from @2x elements.\n"
        "  Supported formats are `png` and `jpg`, output is always `png`.\n"
        "\n"
        "FLAGS\n"
        "  -o, --overwrite\tOverwrite existing files.\n"
        "  -v, --verbose  \tVerbose output.\n"
        "      --help     \tDisplay this menu.\n"
        "      --version  \tDisplay version.\n"
#ifndef NO_DIALOG
        "\nLaunch without arguments to use a GUI.\n"
#endif
        ,
        PROGRAM_NAME);
    exit(0);
}

static inline void put_version_and_die(void)
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
    // If there is just '-' in argv it will be ignored
    if (str[0] != '-')
        return false;

    int len = strlen(str);

    for (int i = 1; i < len; ++i) {
        switch (str[i]) {
            case 'o': flag_overwrite = true; break;
            case 'v': flag_verbose = true; break;

            // Long arguments go here
            case '-': {
                if (strcmp(str, "--overwrite") == 0) {
                    flag_overwrite = true;
                    return true;
                }
                else if (strcmp(str, "--verbose") == 0) {
                    flag_verbose = true;
                    return true;
                }

                else if (strcmp(str, "--help") == 0) {
                    put_help_and_die();
                }
                else if (strcmp(str, "--version") == 0) {
                    put_version_and_die();
                }
                else {
                    put_message("Unknown option %s.\n", str);
                    exit(1);
                }
            } break;

            default: {
                put_message("Unknown option -%c.\n", str[i]);
                exit(1);
            }
        }
    }

    return true;
}
