#ifndef OPEN_FILE_HPP
#define OPEN_FILE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

inline FILE *openAtLoc(const char *filename, const char *mode) {
    const char *szLoc = ".";
    char szFullName[512];

    strcpy(szFullName, szLoc);

    if (szFullName[strlen(szFullName) - 1] != '/') {
        strcat(szFullName, "/");
    }

    strcat(szFullName, filename);

    FILE *ff = fopen(szFullName, mode);

    if (!ff) {
        fprintf(stderr, "ERROR: Cannot open %s.\n", szFullName);
        exit(EXIT_FAILURE);
    }

    return ff;
}

#endif
