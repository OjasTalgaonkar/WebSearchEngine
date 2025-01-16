#ifndef INDEXER_H
#define INDEXER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



struct TrieNode {
    struct TrieNode *children[26];
    int count;
};

struct TrieNode *indexPage(const char *url, int * totalTerms);

int addWordOccurrence(const char *word, const int wordLength, struct TrieNode *root);

struct TrieNode *createNode();

void insert(struct TrieNode *root, const char *key);

int getText(const char *srcAddr, char *buffer, const int bufSize);

void freeTrieMemory(struct TrieNode *node);

int searchKey(struct TrieNode *root, const char *key);



#endif