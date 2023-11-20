#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char * const kLocVar="EST_MORPHO_DATA";

//#ifdef LINUX_PORT

inline FILE* openAtLoc(const char *fileName, const char *mode, 
		       bool mustExist = true)
{
  char *szLoc = getenv(kLocVar);
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

  //  printf("Opening %s\n", szFullName);

  FILE *ff = fopen(szFullName, mode);
  if(!ff && mustExist) {
    fprintf(stderr, "ERROR: Cannot open %s.\n", szFullName);
    exit(-1);
  }

  return ff;
}

//#else

//inline FILE* openAtLoc(const char *fileName, const char *mode, 
//		       bool mustExist = true)
//{
//  return fopen(fileName, mode);
//}

//#endif
