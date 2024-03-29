#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
    #define PROGRAM_NAME "smaller.exe"
    #define MAX_PATH     (256 - 9)
    #define DIR_SEP      "\\/"
    #include <windows.h>
#else
    #define PROGRAM_NAME "smaller"
    #define MAX_PATH     (1024 - 9)
    #define DIR_SEP      "/"
    #include <dirent.h>
    #include <errno.h>
    #include <sys/stat.h>
#endif

#define VERSION "1.2.2"
#define GITHUB  "<https://github.com/toiletbril>"

#endif // COMMON_H
