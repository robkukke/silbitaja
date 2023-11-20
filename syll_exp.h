#ifndef __SYLL_EXP_H__
#define __SYLL_EXP_H__

#ifdef LINUX_PORT
#if !defined(_stdcall)
#define _stdcall
#endif
#endif

const int kResultLength=128;

struct SyllabificationResult
{
   char word[kResultLength];
   int numSyllables;
   SyllabificationResult *next;
   int lastPlace;
#ifdef __cplusplus
   SyllabificationResult();
   SyllabificationResult(SyllabificationResult *pRes);
#endif
};


#ifdef __cplusplus
extern "C" {
#endif

SyllabificationResult* _stdcall syllabify(char *pWord);

void _stdcall deleteResults(SyllabificationResult *pFirst);

#ifdef LINUX_PORT
void initSyllabification();
#endif

#ifdef __cplusplus
}
#endif

#endif
