#ifndef WEBCRAWLER_H
#define WEBCRAWLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "indexer.h"

#define MAX_ADDR_LENGTH 1000




struct listNode{
  char addr[MAX_ADDR_LENGTH];
  struct listNode *next;
  struct TrieNode *root;
  int totalTerms;  
};


int crawl(FILE * fp, int MAX,struct listNode *start);

int contains(const struct listNode *pNode, const char *addr);

int getLink(const char* srcAddr, char* link, const int maxLinkLength);

void insertBack(struct listNode *pNode, const char *addr);

void destroyList(struct listNode *pNode);

int listLength(const struct listNode *head);

int countNodesWithTerm(const struct listNode *head, const char *term);

double TF(const char *term, struct TrieNode *root, int totalTerms);

double IDF(const char *term, struct listNode *start );

double S(struct listNode *start,struct listNode node, const char *query);

int validate_query(const char *query);


#endif

