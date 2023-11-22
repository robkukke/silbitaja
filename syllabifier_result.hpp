#ifndef SYLLABIFIER_RESULT_HPP
#define SYLLABIFIER_RESULT_HPP

const int kResultLength = 128;

struct SyllabificationResult {
    char word[kResultLength];
    int numSyllables;
    SyllabificationResult *next;
    int lastPlace;

    SyllabificationResult();

    explicit SyllabificationResult(SyllabificationResult *pRes);
};

extern "C" {
SyllabificationResult *syllabify(char *pWord);
void deleteResults(SyllabificationResult *pFirst);
void initSyllabification();
}

#endif
