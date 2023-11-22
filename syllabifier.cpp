#include <cstdio>
#include <exception>
#include <iostream>
#include <sstream>

#include "open_file.hpp"
#include "syllabifier_result.hpp"
#include "syllabifier_trie.hpp"

static char *vowels = new char[11];
static char *lowerEstChars = new char[7];
static char *upperEstChars = new char[7];

char otilde = 0;
char aumlaut = 0;
char oumlaut = 0;
char uumlaut = 0;
char scaron = 0;
char zcaron = 0;

Trie gExcTrie;

int init_syllabification();

inline SyllabificationResult::SyllabificationResult() : numSyllables(0), next(nullptr), lastPlace(0) {
    memset(word, '\0', 40);
}

inline SyllabificationResult::SyllabificationResult(SyllabificationResult *pRes) : numSyllables(pRes->numSyllables),
                                                                                   next(nullptr),
                                                                                   lastPlace(pRes->lastPlace) {
    strcpy(word, pRes->word);
}

extern "C" void initSyllabification() {
    init_syllabification();
    gExcTrie.init("syllabifier.exceptions");
}

int isVowel(char c) {
    return strchr(vowels, c) != nullptr;
}

int init_syllabification() {
    strcpy(vowels, "aeiouy");

    size_t numVowels = strlen(vowels);
    FILE *specialCharsFile = openAtLoc("syllabifier.specialchars", "r");

    if (!specialCharsFile) {
        return 1;
    }

    char buf[40];
    int numItems = 0;

    while (fgets(buf, 40, specialCharsFile)) {
        char *ident = (strtok(buf, "= \n"));
        char *values = (strtok(nullptr, "= \n"));

        if (ident && values) {
            upperEstChars[numItems] = values[0];
            lowerEstChars[numItems] = values[1];

            numItems++;

            if (!strcmp(ident, "otilde")) {
                vowels[numVowels++] = otilde = values[1];
            } else if (!strcmp(ident, "aumlaut")) {
                vowels[numVowels++] = aumlaut = values[1];
            } else if (!strcmp(ident, "oumlaut")) {
                vowels[numVowels++] = oumlaut = values[1];
            } else if (!strcmp(ident, "uumlaut")) {
                vowels[numVowels++] = uumlaut = values[1];
            } else if (!strcmp(ident, "scaron")) {
                scaron = values[1];
            } else if (!strcmp(ident, "zcaron")) {
                zcaron = values[1];
            } else {
                return 3;
            }
        } else {
            return 2;
        }

        vowels[numVowels] = '\0';
        upperEstChars[numItems] = '\0';
        lowerEstChars[numItems] = '\0';
    }

    fclose(specialCharsFile);

    return 0;
}

void strToEstLower(unsigned char *pWord) {
    size_t wordLength = strlen(reinterpret_cast<const char *>(pWord));

    for (int i = 0; i < wordLength; i++) {
        if (*(pWord + i) >= 'A' && *(pWord + i) <= 'Z') {
            *(pWord + i) += 32;
        } else if (*(pWord + i) > 'z') {
            char *pUpperChar = strchr(upperEstChars, *(pWord + i));

            if (pUpperChar) {
                *(pWord + i) = *(lowerEstChars + (pUpperChar - upperEstChars));
            }
        }
    }
}

int syllabifySimple(const char *input, char *output, int outputLength) {
    char toSyllabify[kResultLength];
    int syllableLimits[kMaxSyllables];
    int numSyllables = 0;
    size_t inputLength = strlen(input);

    if (inputLength > kResultLength - 1 || inputLength > outputLength - 1) {
        throw std::runtime_error("ERROR: Input too long.\n");
    }

    strcpy(toSyllabify, input);
    strcat(toSyllabify, "#");

    int numPrevVowels = 0;

    for (int i = 0; i < inputLength + 1; i++) {
        if (isVowel(toSyllabify[i])) {
            if (numPrevVowels == 0) {
                if (numSyllables > 0) {
                    syllableLimits[numSyllables - 1] = i - 1;
                }

                numSyllables++;
            }

            numPrevVowels++;
        } else {
            if (numPrevVowels == 3) {
                if (toSyllabify[i - 1] == toSyllabify[i - 2]) {
                    syllableLimits[numSyllables - 1] = i - 2;
                } else {
                    syllableLimits[numSyllables - 1] = i - 1;
                }

                numSyllables++;
            } else if (numPrevVowels == 2 && numSyllables > 1 && (toSyllabify[i - 2] == 'i' &&
                                                                  (toSyllabify[i - 1] == 'a' ||
                                                                   toSyllabify[i - 1] == 'e' ||
                                                                   toSyllabify[i - 1] == 'o' ||
                                                                   toSyllabify[i - 1] == 'u') ||
                                                                  toSyllabify[i - 2] == 'e' &&
                                                                  (toSyllabify[i - 1] == 'a' ||
                                                                   toSyllabify[i - 1] == 'o') ||
                                                                  toSyllabify[i - 2] == 'u' &&
                                                                  (toSyllabify[i - 1] == 'a' ||
                                                                   toSyllabify[i - 1] == 'e' ||
                                                                   toSyllabify[i - 1] == 'o') ||
                                                                  toSyllabify[i - 2] == uumlaut &&
                                                                  (toSyllabify[i - 1] == 'a' ||
                                                                   toSyllabify[i - 1] == 'o'))) {
                syllableLimits[numSyllables - 1] = i - 1;

                numSyllables++;
            }

            numPrevVowels = 0;
        }

        if (numSyllables == kMaxSyllables) {
            throw std::runtime_error("ERROR: Too many syllables.\n");
        }
    }

    int copied = 0;
    int curLimitIndex = 0;

    for (int i = 0; i < inputLength && copied < outputLength - 1; i++) {
        if (curLimitIndex < numSyllables - 1 && i == syllableLimits[curLimitIndex]) {
            output[copied++] = '-';

            curLimitIndex++;
        }

        output[copied++] = toSyllabify[i];
    }

    output[copied] = '\0';

    return numSyllables;
}

extern "C" SyllabificationResult *syllabify(char *pWord) {
    size_t exceptions[kMaxSyllables][2], wordLimits[kMaxSyllables];
    size_t numExceptions = 0, numWordLimits = 0;
    size_t wordLength = strlen(pWord);

    if (wordLength > kResultLength - 1) {
        return nullptr;
    }

    char wordBuf[kResultLength];

    // Find limits of @-separated simple words in the compound word
    for (int i = 0; i <= wordLength; i++)
        if (*(pWord + i) == '@' || *(pWord + i) == '+' || *(pWord + i) == '-') {
            wordLimits[numWordLimits] = i - numWordLimits;

            numWordLimits++;

            if (numWordLimits == kMaxSyllables) {
                return nullptr;
            }
        } else {
            wordBuf[i - numWordLimits] = *(pWord + i);
        }

    wordLimits[numWordLimits] = strlen(wordBuf);

    numWordLimits++;

    // Set to lowercase
    strToEstLower(reinterpret_cast<unsigned char *>(wordBuf));

    // Find exceptional syllable limits in simple words
    // Consider limits of simple words as exceptional syllable limits
    size_t prevWordLimit = 0;

    for (int k = 0; k < numWordLimits; k++) {
        char simpleWord[kResultLength];

        strcpy(simpleWord, "#");
        strncat(simpleWord, wordBuf + prevWordLimit, wordLimits[k] - prevWordLimit);

        simpleWord[wordLimits[k] - prevWordLimit + 1] = '\0';

        strcat(simpleWord, "#");

        size_t simpleWordLength = strlen(simpleWord);

        for (int i = 0; i + gExcTrie.shortestWordLength() <= simpleWordLength; i++) {
            char ruleToo = 0;
            int place = gExcTrie.valueOf(simpleWord + i, &ruleToo);

            if (place == -2) { // Something was wrong in exceptions (bad character in simpleWord)
                fprintf(stderr, "ERROR: Unknown character in the input string \"%s\".\n", pWord);

                return nullptr;
            }

            // An exceptional syllable limit found
            if (place >= 0 && place + i > 0) {
                size_t pos = prevWordLimit + place + i - 1;

                // Test the correct order of an exceptional syllable limit
                if (!numExceptions || exceptions[numExceptions - 1][0] < pos - 1) {
                    exceptions[numExceptions][0] = pos;
                    exceptions[numExceptions][1] = reinterpret_cast<size_t>(reinterpret_cast<unsigned char *>(ruleToo));

                    numExceptions++;

                    if (numExceptions == kMaxSyllables) {
                        return nullptr;
                    }
                }
            }
        }

        exceptions[numExceptions][0] = wordLimits[k];
        exceptions[numExceptions][1] = 0;

        numExceptions++;

        if (numExceptions == kMaxSyllables) {
            return nullptr;
        }

        prevWordLimit = wordLimits[k];
    }

    // Detect standard syllable limits
    auto *pFirstResult = new SyllabificationResult;
    SyllabificationResult *pLastResult = pFirstResult;

    for (int i = 0; i < numExceptions; i++) {
        SyllabificationResult *pNewLast = pLastResult;
        SyllabificationResult *pCur = pFirstResult;

        while (true) {
            if (exceptions[i][0] > pCur->lastPlace) {
                if (exceptions[i][1]) {
                    pNewLast = pNewLast->next = new SyllabificationResult(pCur);
                }

                char elementaryPart[kResultLength];

                strncpy(elementaryPart, wordBuf + pCur->lastPlace, exceptions[i][0] - pCur->lastPlace);

                elementaryPart[exceptions[i][0] - pCur->lastPlace] = '\0';

                try {
                    pCur->numSyllables += syllabifySimple(elementaryPart, pCur->word + strlen(pCur->word),
                                                          static_cast<int>(kResultLength - strlen(pCur->word)));
                }

                catch (char *s) {
                    return nullptr;
                }

                pCur->lastPlace = exceptions[i][0];

                if (i != numExceptions - 1) {
                    strcat(pCur->word, "-");
                }
            }

            if (pCur == pLastResult) {
                break;
            }

            pCur = pCur->next;
        }

        pLastResult = pNewLast;
    }

    // Restore original character cases
    for (SyllabificationResult *pCur = pFirstResult; pCur; pCur = pCur->next) {
        size_t resultLength = strlen(pCur->word);

        for (int i = 0, j = 0; i < resultLength; i++, j++) {
            if (pCur->word[j] == '-' && *(pWord + i) != '@' && *(pWord + i) != '+' && *(pWord + i) != '-') {
                j++;
            }

            if (pCur->word[j] != '-') {
                pCur->word[j] = *(pWord + i);
            }
        }
    }

    return pFirstResult;
}

extern "C" void deleteResults(SyllabificationResult *pFirst) {
    SyllabificationResult *pCur, *pTmp;
    pCur = (SyllabificationResult *) pFirst;

    while (pCur) {
        pTmp = pCur;
        pCur = pCur->next;

        delete pTmp;
    }
}

void syllabifyIndividualWords(const std::string &input) {
    std::istringstream iss(input);
    std::string word;

    while (iss >> word) {
        char *buf = &word[0];

        if (*buf != '-') {
            SyllabificationResult *pFirst = syllabify(buf);

            if (pFirst != nullptr) {
                printf("%s\n", pFirst->word);
                deleteResults(pFirst);
            }
        }
    }
}

int main() {
    initSyllabification();

    std::string input;
    std::getline(std::cin, input);

    syllabifyIndividualWords(input);

    return 0;
}
