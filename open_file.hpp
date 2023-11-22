#ifndef OPEN_FILE_HPP
#define OPEN_FILE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

inline FILE* openAtLoc(const char *fileName, const char *mode)
{
  const char *szLoc = ".";

  char szFullName[512];
  strcpy(szFullName, szLoc);
  if(szFullName[strlen(szFullName) - 1] != '/')
    strcat(szFullName, "/");
  strcat(szFullName, fileName);

  FILE *ff = fopen(szFullName, mode);
  if(!ff) {
    fprintf(stderr, "ERROR: Cannot open %s.\n", szFullName);
    exit(-1);
  }

  return ff;
}

#endif
