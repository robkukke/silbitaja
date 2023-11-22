#ifndef __TRIE_HPP__
#define __TRIE_HPP__

#include <string.h>

const int kMaxSyllables=32;

class TrieNode
{
public:
   TrieNode();
   ~TrieNode();
   int valueOf(char *pWord, char *pRuleToo);
   void add(char *pWord, char val, char ruleToo);
   static char mapChar(unsigned char c);
private:
   TrieNode **fpArray;
   char fValue;
   char fRuleToo;
};

class Trie
{
public:
   Trie();
   ~Trie();
   void init(const char *fileName);
   int valueOf(char *pWord, char *pRuleToo);
   int shortestWordLength() const;
private:
   TrieNode *fpRoot;
   int fShortestWordLength;
};


#endif