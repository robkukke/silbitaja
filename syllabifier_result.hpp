#ifndef SYLLABIFIER_RESULT_HPP
#define SYLLABIFIER_RESULT_HPP

const int kResultLength = 128;

struct SyllabificationResult {
    char word[kResultLength]{};
    int numSyllables;
    SyllabificationResult *next;
    size_t lastPlace;

    SyllabificationResult();

    explicit SyllabificationResult(SyllabificationResult *pRes);
};

SyllabificationResult *syllabify(char *pWord);

void deleteResults(SyllabificationResult *pFirst);

void initSyllabification();

#endif
