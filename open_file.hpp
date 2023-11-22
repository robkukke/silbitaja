#ifndef OPEN_FILE_HPP
#define OPEN_FILE_HPP

#include <cstdio>
#include <cstdlib>
#include <cstring>

const char * const kLocVar="EST_MORPHO_DATA";

inline FILE* openAtLoc(const char *fileName, const char *mode, 
		       bool mustExist = true)
{
  const char *szLoc = getenv(kLocVar);
  if(!szLoc) {
    fprintf(stderr, "WARNING: %s is not set, defaulting to current directory\n", 
	    kLocVar);
    szLoc = ".";
  }

  char szFullName[512];
  strcpy(szFullName, szLoc);
  if(szFullName[strlen(szFullName) - 1] != '/')
    strcat(szFullName, "/");
  strcat(szFullName, fileName);

  FILE *ff = fopen(szFullName, mode);
  if(!ff && mustExist) {
    fprintf(stderr, "ERROR: Cannot open %s.\n", szFullName);
    exit(-1);
  }

  return ff;
}

#endif
