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

static const int kAlphabetLength='z'-'a'+11;
static const int kWowelPos=kAlphabetLength-4;
static const int kConsPos=kAlphabetLength-3;
static const int kGenPos=kAlphabetLength-2;


TrieNode::TrieNode():fpArray(nullptr),fValue(-1), fRuleToo(0)
{
}

TrieNode::~TrieNode(){
   if(fpArray){
      for(int i=0; i<kAlphabetLength; i++)
         if(fpArray[i]){
            delete fpArray[i];
            fpArray[i]=nullptr;
         }
      delete fpArray;
      fpArray=nullptr;
   }
}

int TrieNode::valueOf(char *pWord, char *pRuleToo)
{
   if(*pWord==0 || *pWord==10 || *pWord==13 || *pWord==32 || !fpArray){ //end of the given word or trie branch
      *pRuleToo=fRuleToo;
      return fValue;
   }
   else{
      char val=-1;
      char indx=mapChar(*pWord);
      if(indx==(char)-1) //unknown character in the word
        return -2;
      *pRuleToo=0;
      if(fpArray[indx]) //current character itself supported
         val=static_cast<char>(fpArray[indx]->valueOf(pWord+1, pRuleToo)); //test next character
      int bWowel=0;
      if(val==-1 && (bWowel=isVowel(*pWord)) && fpArray[kWowelPos]) //test general wowel's case
         val=static_cast<char>(fpArray[kWowelPos]->valueOf(pWord+1, pRuleToo));
      if(val==-1 && !bWowel && *pWord!='#' && fpArray[kConsPos]) //test general consonant's case
         val=static_cast<char>(fpArray[kConsPos]->valueOf(pWord+1, pRuleToo));
      if(val==-1 && *pWord!='#' && fpArray[kGenPos])  //test general character (but not #)
         val=static_cast<char>(fpArray[kGenPos]->valueOf(pWord+1, pRuleToo));
      return val;
   }
}

void TrieNode::add(char *pWord, char val, char ruleToo)
{
   if(*pWord=='@'){
      this->add(pWord+1, val, ruleToo);
   }
   else if(*pWord==0){
      fValue=val;
      fRuleToo=ruleToo;
   }
   else{
      if(!fpArray){
         fpArray=new TrieNode*[kAlphabetLength];
         memset(fpArray, 0, kAlphabetLength*sizeof(TrieNode*));
      }
      char indx=mapChar(*pWord);
      if(indx<0){
        fprintf(stderr, "ERROR: "
                        "Unknown character: '%c' in the exceptions file!\n", *pWord);
        return;
      }
      if(!fpArray[indx])
         fpArray[indx]=new TrieNode;
      fpArray[indx]->add(pWord+1, val, ruleToo);
   }
}

char TrieNode::mapChar(unsigned char c)
{
   if (c >= 48 && c <= 57) {
       return static_cast<char>(c);
   }
   if(c>'z' || c<'a'){
      if(c==(unsigned char)otilde)
         return 'z'-'a'+1;
      else if(c==(unsigned char)aumlaut)
         return 'z'-'a'+2;
      else if(c==(unsigned char)oumlaut)
         return 'z'-'a'+3;
      else if(c==(unsigned char)uumlaut)
         return 'z'-'a'+4;
      else if(c==(unsigned char)scaron)
         return 'z'-'a'+5;
      else if(c==(unsigned char)zcaron)
         return 'z'-'a'+6;
      else if(c==(unsigned char)'W') //general wowel
         return 'z'-'a'+7;
      else if(c==(unsigned char)'C') //general consonant
         return 'z'-'a'+8;
      else if(c==(unsigned char)'*') //general character
         return 'z'-'a'+9;
      else if(c==(unsigned char)'#') //'fake' beginning/ending of a word
         return 'z'-'a'+10;
      else
         return -1;
   }
   else
      return static_cast<char>(c-'a');
}

Trie::Trie() : fpRoot(nullptr), fShortestWordLength(MAXINT)
{
}

void Trie::init(const char *fileName)
{
   FILE *inFile;
   if(!(inFile=openAtLoc(fileName, "r")))
      return;
   else{
      fpRoot=new TrieNode();
      char buf[80];
      while(fgets(buf, 80, inFile)){
         char *pWord=strtok(buf, " \r\n");
         char *pRuleToo=strtok(nullptr, " \r\n");
         if(pWord){
            size_t wlength=strlen(pWord)-1;
            if(wlength<fShortestWordLength)
               fShortestWordLength=static_cast<int>(wlength);
            char val=(char)(strchr(pWord, '@')-pWord);
            char ruleToo=pRuleToo? 1: 0;
            fpRoot->add(pWord, val, ruleToo);
         }
      }
      fclose(inFile);
   }
}

Trie::~Trie()
{
   if(fpRoot)
      delete fpRoot;
}

int Trie::valueOf(char *pWord, char *pRuleToo)
{
   if(fpRoot){
      return fpRoot->valueOf(pWord, pRuleToo);
   }
   else{
      *pRuleToo=0;
      return -1;
   }
}

int Trie::shortestWordLength()
{
   return fShortestWordLength;
}

