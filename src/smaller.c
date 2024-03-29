#include "smaller.h"
#include "common.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
static bool get_resized_filename(const char *filename, size_t size, char *buf)
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

bool file_exists(const char *filepath)
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

bool is_skin_folder(const char *dir_path)
{
    char ini_path[MAX_PATH];
    strcpy(ini_path, dir_path);
    strcat(ini_path, "/skin.ini");

    return file_exists(ini_path);
}

static void smaller_file(const char *file_path)
{
    char new_file_path[MAX_PATH];
    get_resized_filename(file_path, MAX_PATH, new_file_path);

    if (!flag_overwrite) {
        if (file_exists(new_file_path)) {
            if (flag_verbose) {
                put_message("'%s' already exists, skipping.\n", new_file_path);
            }
            files_skipped++;
            return;
        }
    }

    if (flag_verbose) {
        put_message("Resizing '%s'...\n", file_path);
    }

    int width, height, channels, ok;
    ok = stbi_info(file_path, &width, &height, &channels);

    if (!ok) {
        put_item_and_die(stbi_failure_reason(), file_path);
    }

    unsigned char *image = stbi_load(file_path, &width, &height, &channels, 0);

    int new_width  = width / 2;
    int new_height = height / 2;
    if (new_width  <= 0) new_width  = 1;
    if (new_height <= 0) new_height = 1;

    unsigned char *resized_image = (unsigned char *)malloc(new_width * new_height * channels);

    if (resized_image == NULL) {
        put_item_and_die("Could not allocate memory for resized image", file_path);
    }

    ok = stbir_resize_uint8(image, width, height, 0, resized_image, new_width,
                            new_height, 0, channels);

    if (!ok) {
        put_item_and_die("Could not resize image", file_path);
    }

    ok = stbi_write_png(new_file_path, new_width, new_height, channels,
                        resized_image, 0);

    if (!ok) {
        put_item_and_die("Failed to write resized image", new_file_path);
    }

    files_created++;

    free(resized_image);
    free(image);
}

// Calls `smaller_file` on each @2x skin element in a directory
void smaller_dir(const char *dir_path)
{
#ifdef _WIN32
    char dir_wildcard[MAX_PATH];
    char *extensions[2] = { "\\*.png", "\\*.jpg" };

    for (size_t i = 0; i < 2; ++i) {
        strcpy(dir_wildcard, dir_path);
        strcat(dir_wildcard, extensions[i]);

        HANDLE hfind;
        WIN32_FIND_DATA file;
        char file_path[MAX_PATH];

        if ((hfind = FindFirstFile(dir_wildcard, &file)) != INVALID_HANDLE_VALUE) {
            do {
                int count = snprintf(file_path, MAX_PATH, "%s\\%s", dir_path, file.cFileName);
                if (count < 0) {
                    put_item_and_die("Invalid characters in file path", file_path);
                }

                if (has_twox(file_path)) {
                    smaller_file(file_path);
                }
            } while (FindNextFile(hfind, &file));

            FindClose(hfind);
        }
    }
#else
    DIR *dir;
    dir = opendir(dir_path);

    if (dir == NULL) {
        put_item_and_die(strerror(errno), dir_path);
        exit(1);
    }

    struct dirent *entry;
    char file_path[MAX_PATH];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            int count = snprintf(file_path, MAX_PATH, "%s/%s", dir_path, entry->d_name);
            if (count >= MAX_PATH) {
                put_item_and_die("File path is too long", entry->d_name);
            }

            if (count < 0) {
                put_item_and_die("Invalid characters in file path", file_path);
            }

            if (is_twox_image(file_path)) {
                smaller_file(file_path);
            }
        }
    }

    closedir(dir);
#endif
}
