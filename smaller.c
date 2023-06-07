#ifdef _WIN32
    #define PROGRAM_NAME "smaller.exe"
    #define _CRT_SECURE_NO_WARNINGS
    #include <Windows.h>
#else
    #define PROGRAM_NAME "smaller"
    #include <dirent.h>
#endif

#include <stdio.h>
#include <string.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#define VERSION "0.3"

void put_error(const char* m, const char* filename)
{
    printf("smaller: %s: %s\n", filename, m);
    exit(1);
}

int get_file_ext(const char *filename, size_t size, char *buf)
{
    size_t len = strlen(filename);
    for (size_t i = len - 1; i != 0; --i) {
        if (filename[i] == '.') {
            ++i;
            size_t k = 0;
            for (; i <= len - 1; ++i) {
                if (k >= size) return 0;
                buf[k++] = filename[i];
            }

            buf[k] = '\0';
            return 1;
        }
    }

    return 0;
}

int is_twox_image(const char *filename)
{
    const char* ext = strrchr(filename, '.');
    if (ext != NULL && strlen(ext) > 3) {
        const char* twox = ext - 3;
        if (strncmp(twox, "@2x", 3) == 0) {
            return 1;
        }
    }
    return 0;
}

int resized_filename(const char *filename, size_t size, char *buf)
{
    int len = strlen(filename);
    char *twox_index = strstr(filename, "@2x");

    if (twox_index != NULL) {
        size_t k = 0;
        size_t pos = twox_index - filename;

        for (size_t i = 0; i < len; ++i) {
            if (k >= size) return 0;
            if (i == pos) i += 3;
            buf[k++] = filename[i];
        }

        buf[k] = '\0';
        return 1;
    }

    return 0;
}

int is_skin_folder(const char* dir_path) {
#ifdef _WIN32
    HANDLE hfind;
    WIN32_FIND_DATA file;

    char dir_wildcard[512];
    strcpy(dir_wildcard, dir_path);
    strcat(dir_wildcard, "/*");

    if((hfind = FindFirstFile(dir_wildcard, &file)) != INVALID_HANDLE_VALUE) {
    do {
            if (strcmp(file.cFileName, "skin.ini") == 0) {
                FindClose(hfind);
                return 1;
            }
    } while (FindNextFile(hfind, &file));

    FindClose(hfind);
    }
#else
    DIR* dir;
    dir = opendir(dir_path);

    if (dir == NULL) {
        perror(dir_path);
        exit(1);
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, "skin.ini") == 0) {
            closedir(dir);
            return 1;
        }
    }

    closedir(dir);
#endif
    return 0;
}

int make_it_smaller(const char *filename)
{
    int width, height, channels, ok;
    ok = stbi_info(filename, &width, &height, &channels);

    printf("smaller: resizing %s...\n", filename);

    if (!ok) {
        put_error(stbi_failure_reason(), filename);
    }

    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);

    int new_width  = width / 2;
    if (new_width <= 0) new_width = 1;
    int new_height = height / 2;
    if (new_height <= 0) new_height = 1;

    unsigned char* resized_image = (unsigned char*)malloc(new_width * new_height * channels);

    ok = stbir_resize_uint8(image, width, height, 0, resized_image, new_width, new_height, 0, channels);

    if (!ok) {
        put_error(stbi_failure_reason(), filename);
    }

    char new_filename[128];
    resized_filename(filename, 128, new_filename);

    ok = stbi_write_png(new_filename, new_width, new_height, channels, resized_image, 0);

    if (!ok) {
        put_error("Failed to write output image", new_filename);
    }

    free(resized_image);
    free(image);

    return 1;
}

int smaller_dir(const char *dir_path)
{
#ifdef _WIN32
    HANDLE hfind;
    WIN32_FIND_DATA file;

    char dir_wildcard[512];
    strcpy(dir_wildcard, dir_path);
    strcat(dir_wildcard, "/*");

    char filename[512];

    if((hfind = FindFirstFile(dir_wildcard, &file)) != INVALID_HANDLE_VALUE) {
        do {
            sprintf(filename, "%s/%s", dir_path, file.cFileName);

            char ext[16];
            get_file_ext(filename, 16, ext);

            if (strcmp(ext, "png") == 0 || strcmp(ext, "jpg") == 0) {
                if (!is_twox_image(filename)) continue;
                int ok = make_it_smaller(filename);
                if (!ok) {
                    put_error("Failed to generate resized image", file.cFileName);
                }
            }
        } while (FindNextFile(hfind, &file));
    FindClose(hfind);
    }
#else
    DIR *dir;
    dir = opendir(dir_path);

    if (dir == NULL) {
        perror(dir_path);
        exit(1);
    }

    struct dirent *entry;
    char filename[512];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            sprintf(filename, "%s/%s", dir_path, entry->d_name);

            char ext[16];
            get_file_ext(filename, 16, ext);

            if (strcmp(ext, "png") == 0 || strcmp(ext, "jpg") == 0) {
                if (!is_twox_image(filename)) continue;
                int ok = make_it_smaller(filename);
                if (!ok) {
                    put_error("Failed to generate resized image", entry->d_name);
                }
            }
        }
    }

    closedir(dir);
#endif
    return 1;
}

void usage(void)
{
    printf("ERROR: Not enough arguments.\n"
           "USAGE: %s <skin directory>\n"
           "Create osu! @1x skin elements from @2x elements. Works with `png` and `jpg`.\n"
           "%s (c) toiletbril <https://github.com/toiletbril>\n", PROGRAM_NAME, VERSION);
    exit(1);
}

int concat_args(int argc, char **argv, size_t size, char *buf)
{
    size_t k = 0;
    for (int i = 1; i < argc; ++i) {
        size_t len = strlen(argv[i]);

        if (k >= size) return 0;
        if (i > 1) buf[k++] = ' ';

        for (size_t j = 0; j < len; ++j) {
            if (k >= size) return 0;
            buf[k++] = argv[i][j];
        }
    }

    buf[k] = '\0';
    return 1;
}

int main(int argc, char **argv)
{
    if (argc < 2) usage();

    char dir_path[512];
    concat_args(argc, argv, 512, dir_path);

    if (!is_skin_folder(dir_path)) {
        put_error("Is not a skin folder", dir_path);
    }

    int ok = smaller_dir(dir_path);

    if (!ok) {
        put_error("Something went wrong", argv[1]);
    }

    printf("Successfully created @1x skin elements from %s.\n", argv[1]);
    return 0;
}
