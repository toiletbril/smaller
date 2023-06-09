#ifdef _WIN32
    #define PROGRAM_NAME "smaller.exe"
    #define MAX_PATH 256
    #define _CRT_SECURE_NO_WARNINGS
    #include <Windows.h>
#else
    #define PROGRAM_NAME "smaller"
    #define MAX_PATH 1024
    #include <dirent.h>
    #include <errno.h>
    #include <sys/stat.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define VERSION "0.7.2"
#define GITHUB "<https://github.com/toiletbril>"

static bool flag_overwrite = false;

static size_t files_created = 0;
static size_t files_skipped = 0;

static inline void put_error(const char *m, const char *filename)
{
    fprintf(stderr, "%s: %s: %s\n", PROGRAM_NAME, filename, m);
    exit(1);
}

// Returns file extension if there is one or `NULL`
static const char *file_extension(const char *filename)
{
    size_t len = strlen(filename);

    for (size_t i = len - 1; i != 0; --i) {
        if (filename[i] == '.') {
            if (i > len - 2)
                return NULL;
            return &filename[++i];
        }
    }

    return NULL;
}

// Returns `true` if there is @2x before file extension
static bool has_twox(const char *filename)
{
    const char *ext = strrchr(filename, '.');

    if (ext != NULL && strlen(ext) > 3) {
        const char *twox = ext - 3;
        if (strncmp(twox, "@2x", 3) == 0) {
            return true;
        }
    }

    return false;
}

static bool is_twox_image(const char *filename)
{
    const char *extension = file_extension(filename);
    if (extension == NULL)
        return false;

    if (!(strcmp(extension, "png") == 0 || strcmp(extension, "jpg") == 0))
        return false;

    if (!has_twox(filename))
        return false;

    return true;
}

// Puts filename without @2x into `buf`. Returns false if `size` is exceeded or there is no @2x
static bool resized_filename(const char *filename, size_t size, char *buf)
{
    size_t len       = strlen(filename);
    char *twox_index = strstr(filename, "@2x");

    if (twox_index != NULL) {
        size_t k   = 0;
        size_t pos = twox_index - filename;

        for (size_t i = 0; i < len; ++i) {
            if (k > size - 2)
                return false;
            if (i == pos)
                i += 3;
            buf[k++] = filename[i];
        }

        buf[k] = '\0';
        return true;
    }

    return false;
}

static inline bool file_exists(const char *filepath)
{
#ifdef _WIN32
    DWORD file = GetFileAttributes(filepath);
    if (file != INVALID_FILE_ATTRIBUTES && !(file & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    }
#else
    struct stat buf;
    if (stat(filepath, &buf) == 0) {
        return true;
    }
#endif
    return false;
}

static inline bool is_skin_folder(const char *dir_path)
{
    char ini_path[MAX_PATH];
    strcpy(ini_path, dir_path);
    strcat(ini_path, "/skin.ini");

    return file_exists(ini_path);
}

static void smaller_file(const char *file_path)
{
    char new_file_path[MAX_PATH];
    resized_filename(file_path, MAX_PATH, new_file_path);

    if (!flag_overwrite) {
        if (file_exists(new_file_path)) {
            ++files_skipped;
            return;
        }
    }

    printf("Resizing %s...\n", file_path);

    int width, height, channels, ok;
    ok = stbi_info(file_path, &width, &height, &channels);

    if (!ok) {
        put_error(stbi_failure_reason(), file_path);
    }

    unsigned char *image = stbi_load(file_path, &width, &height, &channels, 0);

    int new_width = width / 2;
    if (new_width <= 0)
        new_width = 1;
    int new_height = height / 2;
    if (new_height <= 0)
        new_height = 1;

    unsigned char *resized_image = (unsigned char *)malloc(new_width * new_height * channels);

    if (resized_image == NULL) {
        put_error("Could not allocate memory for resized image", file_path);
    }

    ok = stbir_resize_uint8(image, width, height, 0, resized_image, new_width,
                            new_height, 0, channels);

    if (!ok) {
        put_error("Could not resize image", file_path);
    }

    ok = stbi_write_png(new_file_path, new_width, new_height, channels,
                        resized_image, 0);

    if (!ok) {
        put_error("Failed to write resized image", new_file_path);
    }

    ++files_created;

    free(resized_image);
    free(image);
}

// Calls `smaller_file` on each @2x skin element in a directory
void smaller_dir(const char *dir_path)
{
#ifdef _WIN32
    char dir_wildcard[MAX_PATH];
    strcpy(dir_wildcard, dir_path);
    strcat(dir_wildcard, "/*");

    HANDLE hfind;
    WIN32_FIND_DATA file;
    char file_path[MAX_PATH];

    if ((hfind = FindFirstFile(dir_wildcard, &file)) != INVALID_HANDLE_VALUE) {
        do {
            int count = snprintf(file_path, MAX_PATH, "%s/%s", dir_path, file.cFileName);
            if (count < 0) {
                put_error("Invalid characters in file path", file_path);
            }

            if (is_twox_image(file_path)) {
                smaller_file(file_path);
            }
        } while (FindNextFile(hfind, &file));

        FindClose(hfind);
    }
#else
    DIR *dir;
    dir = opendir(dir_path);

    if (dir == NULL) {
        put_error(strerror(errno), dir_path);
        exit(1);
    }

    struct dirent *entry;
    char file_path[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            int count = snprintf(file_path, MAX_PATH, "%s/%s", dir_path, entry->d_name);
            if (count >= MAX_PATH) {
                put_error("File path is too long", entry->d_name);
            }

            if (count < 0) {
                put_error("Invalid characters in file path", file_path);
            }

            if (is_twox_image(file_path)) {
                smaller_file(file_path);
            }
        }
    }

    closedir(dir);
#endif
}

// Adds space between args and concatenates them to `buf`. Skips ones that start with '-'
// Returns `false` if size is exceeded
static bool concat_args(int argc, char **argv, size_t size, char *buf)
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
    printf("USAGE: %s <skin directory>\n"
           "Create @1x osu! skin elements from @2x elements. Works with `png` and `jpg`, "
           "outputs `png`.\n"
           "\n"
           "FLAGS:\n"
           "  -o, --overwrite\tOverwrite existing files.\n"
           "      --help     \tDisplay this menu.\n"
           "      --version  \tDisplay version.\n",
           PROGRAM_NAME);
    exit(0);
}

static inline void version(void)
{
    printf("%s %s\n"
           "(c) toiletbril %s\n",
           PROGRAM_NAME, VERSION, GITHUB);
    exit(0);
}

// Returns false if `str` is not a flag, otherwise sets global variables
static bool set_flag(const char *str)
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
                    return false;
                }
                else if (strcmp(str, "--version") == 0) {
                    version();
                }
                else {
                    fprintf(stderr,
                            "%s: Unknown option %s\n"
                            "Try '%s --help'.\n",
                            PROGRAM_NAME, str, PROGRAM_NAME);
                    exit(1);
                }
            } break;

            default: {
                fprintf(stderr,
                        "%s: Unknown option -%c\n"
                        "Try '%s --help'.\n",
                        PROGRAM_NAME, str[i], PROGRAM_NAME);
                exit(1);
            }
        }
    }

    return true;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "%s: ERROR: Not enough arguments. Try '--help' for more information.\n",
                PROGRAM_NAME);
        exit(1);
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

    smaller_dir(dir_path);

    printf("%s: Successfully traversed '%s'. Files created: %zu, skipped: %zu.\n",
           PROGRAM_NAME, dir_path, files_created, files_skipped);
    return 0;
}
