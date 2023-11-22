#include <cstdio>
#include <values.h>

#include "open_file.hpp"
#include "syllabifier_trie.hpp"

extern char otilde;
extern char aumlaut;
extern char oumlaut;
extern char uumlaut;
extern char scaron;
extern char zcaron;

int isVowel(char c);

static const int kAlphabetLength = 'z' - 'a' + 11;
static const int kVowelPos = kAlphabetLength - 4;
static const int kConsPos = kAlphabetLength - 3;
static const int kGenPos = kAlphabetLength - 2;

TrieNode::TrieNode() : fpArray(nullptr), fValue(-1), fRuleToo(0) {}

TrieNode::~TrieNode() {
    if (fpArray) {
        for (int i = 0; i < kAlphabetLength; i++) {
            if (fpArray[i]) {
                delete fpArray[i];

                fpArray[i] = nullptr;
            }
        }

        delete fpArray;

        fpArray = nullptr;
    }
}

int TrieNode::valueOf(char *pWord, char *pRuleToo) {
    if (*pWord == 0 || *pWord == 10 || *pWord == 13 || *pWord == 32 ||
        !fpArray) { // End of the given word or trie branch
        *pRuleToo = fRuleToo;

        return fValue;
    } else {
        char val = -1;
        char index = mapChar(*pWord);

        if (index == (char) -1) { // Unknown character in the word
            return -2;
        }

        *pRuleToo = 0;

        if (fpArray[index]) { // Current character itself is supported
            val = static_cast<char>(fpArray[index]->valueOf(pWord + 1, pRuleToo)); // Test next character
        }

        int bVowel;

        if (val == -1 && (bVowel = isVowel(*pWord)) && fpArray[kVowelPos]) { // Test general vowel's case
            val = static_cast<char>(fpArray[kVowelPos]->valueOf(pWord + 1, pRuleToo));
        }

        if (val == -1 && !bVowel && *pWord != '#' && fpArray[kConsPos]) { // Test general consonant's case
            val = static_cast<char>(fpArray[kConsPos]->valueOf(pWord + 1, pRuleToo));
        }

        if (val == -1 && *pWord != '#' && fpArray[kGenPos]) { // Test general character (but not #)
            val = static_cast<char>(fpArray[kGenPos]->valueOf(pWord + 1, pRuleToo));
        }

        return val;
    }
}

void TrieNode::add(char *pWord, char val, char ruleToo) {
    if (*pWord == '@') {
        this->add(pWord + 1, val, ruleToo);
    } else if (*pWord == 0) {
        fValue = val;
        fRuleToo = ruleToo;
    } else {
        if (!fpArray) {
            fpArray = new TrieNode *[kAlphabetLength];

            memset(fpArray, 0, kAlphabetLength * sizeof(TrieNode *));
        }

        char index = mapChar(*pWord);

        if (index < 0) {
            fprintf(stderr, "ERROR: Unknown character \"%c\" in the exceptions file.\n", *pWord);

            return;
        }

        if (!fpArray[index]) {
            fpArray[index] = new TrieNode;
        }

        fpArray[index]->add(pWord + 1, val, ruleToo);
    }
}

char TrieNode::mapChar(unsigned char c) {
    if (c >= 48 && c <= 57) {
        return static_cast<char>(c);
    }

    if (c > 'z' || c < 'a') {
        if (c == (unsigned char) otilde) {
            return 'z' - 'a' + 1;
        } else if (c == (unsigned char) aumlaut) {
            return 'z' - 'a' + 2;
        } else if (c == (unsigned char) oumlaut) {
            return 'z' - 'a' + 3;
        } else if (c == (unsigned char) uumlaut) {
            return 'z' - 'a' + 4;
        } else if (c == (unsigned char) scaron) {
            return 'z' - 'a' + 5;
        } else if (c == (unsigned char) zcaron) {
            return 'z' - 'a' + 6;
        } else if (c == (unsigned char) 'W') { // General vowel
            return 'z' - 'a' + 7;
        } else if (c == (unsigned char) 'C') { // General consonant
            return 'z' - 'a' + 8;
        } else if (c == (unsigned char) '*') { // General character
            return 'z' - 'a' + 9;
        } else if (c == (unsigned char) '#') { // 'Fake' beginning/ending of a word
            return 'z' - 'a' + 10;
        } else {
            return -1;
        }
    } else {
        return static_cast<char>(c - 'a');
    }
}

Trie::Trie() : fpRoot(nullptr), fShortestWordLength(MAXINT) {}

void Trie::init(const char *filename) {
    FILE *exceptionsFile;

    if (!(exceptionsFile = openAtLoc(filename, "r"))) {
        return;
    } else {
        fpRoot = new TrieNode();
        char buf[80];

        while (fgets(buf, 80, exceptionsFile)) {
            char *pWord = strtok(buf, " \r\n");
            char *pRuleToo = strtok(nullptr, " \r\n");

            if (pWord) {
                size_t wLength = strlen(pWord) - 1;

                if (wLength < fShortestWordLength) {
                    fShortestWordLength = static_cast<int>(wLength);
                }

                char val = (char) (strchr(pWord, '@') - pWord);
                char ruleToo = pRuleToo ? 1 : 0;

                fpRoot->add(pWord, val, ruleToo);
            }
        }

        fclose(exceptionsFile);
    }
}

Trie::~Trie() {
    delete fpRoot;
}

int Trie::valueOf(char *pWord, char *pRuleToo) {
    if (fpRoot) {
        return fpRoot->valueOf(pWord, pRuleToo);
    } else {
        *pRuleToo = 0;

        return -1;
    }
}

int Trie::shortestWordLength() const {
    return fShortestWordLength;
}
