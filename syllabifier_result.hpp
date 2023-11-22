#ifndef __SYLL_EXP_H__
#define __SYLL_EXP_H__

const int kResultLength=128;

struct SyllabificationResult
{
   char word[kResultLength];
   int numSyllables;
   SyllabificationResult *next;
   int lastPlace;

   SyllabificationResult();
   SyllabificationResult(SyllabificationResult *pRes);
};


extern "C" {

SyllabificationResult* syllabify(char *pWord);

void deleteResults(SyllabificationResult *pFirst);

void initSyllabification();

}

#endif
