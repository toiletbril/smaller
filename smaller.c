#include <stdio.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #define _CRT_SECURE_NO_WARNINGS
    #include <Windows.h>
#else
    #include <dirent.h>
    #include <sys/stat.h>
#endif

#define VERSION "0.1"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void put_error(const char* m, const char* filename)
{
    printf("smaller: %s: %s\n", filename, m);
    exit(1);
}

int get_file_ext(const char* filename, size_t size, char* buf)
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
            return 1;
        }
    }

    return 0;
}

int is_twox_image(const char* filename)
{
    int len = strlen(filename);
    if (len - 7 <= 0) return 0;

    if (filename[len - 7] == '@') {
        return 1;
    }

    return 0;
}

int resized_filename(const char* filename, size_t size, char* buf)
{
    // button-left@2x.png
    int len = strlen(filename);

    if (len - 7 <= 0) return 0;

    if (filename[len - 7] == '@') {
        int k = 0;

        for (int i = 0; i < len; ++i) {
            if ((size_t)k >= size) return 0;
            if (i == len - 7) i += 3;
            buf[k++] = filename[i];
        }

        buf[k] = '\0';
        return 1;
    }

    return 0;
}

int make_it_smaller(const char* filename)
{
    int width, height, channels, ok;
    ok = stbi_info(filename, &width, &height, &channels);

    printf("smaller: resizing %s...\n", filename);

    if (!ok) {
        put_error(stbi_failure_reason(), filename);
    }

    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);

    int new_width = width / 2;
    int new_height = height / 2;

    unsigned char* resized_image = (unsigned char*)malloc(new_width * new_height * channels);

    stbir_resize_uint8(image, width, height, 0, resized_image, new_width, new_height, 0, channels);

    char new_filename[128];
    ok = resized_filename(filename, 128, new_filename);

    if (!ok) {
        put_error("resized_filename failed", filename);
    }

    char extension[16];
    get_file_ext(filename, 16, extension);

    ok = stbi_write_png(new_filename, new_width, new_height, channels, resized_image, 0);

    if (!ok) {
        put_error("Failed to write output image", new_filename);
    }

    free(resized_image);
    free(image);

    return 1;
}

int smaller_dir(const char* dir_path)
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
        if (strcmp(file.cFileName, ".") != 0 && strcmp(file.cFileName, "..") != 0) {
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
        }
    } while (FindNextFile(hfind, &file));

    FindClose(hfind);

    return 1;
}
#else
    printf("Linux is TODO :3\n");
#endif
    return 0;
}

void usage(void)
{
    printf("ERROR: Not enough arguments.\n"
           "USAGE: smaller <skin directory>\n"
           "Create 1x skin elements from 2x elements.\n"
           "%s (c) toiletbril <https://github.com/toiletbril>\n", VERSION);
    exit(1);
}

int main(int argc, char** argv)
{
    if (argc != 2) usage();

    const char* dir_path = argv[1];
    int ok = smaller_dir(dir_path);

    if (!ok) {
        put_error("Something went wrong", argv[1]);
    }

    printf("Successfully converted %s.", argv[1]);

    return 0;
}
