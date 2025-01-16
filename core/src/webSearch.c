#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/webCrawler.h"

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "USAGE: %s startAddr numHops [rand seed]\n", argv[0]);
    return -1;
  }

  const char *inputFile = argv[1];
  FILE *fp = fopen(inputFile, "r");
  if (fp == NULL) {
    fprintf(stderr, "ERROR: Could not open input file.\n");
    return 1;
  }

  int MAX_n = atoi(argv[2]);
  long seed = atol(argv[3]);
  srand(seed);

  struct listNode *start = malloc(sizeof(struct listNode));
  if (!start) {
    fprintf(stderr, "ERROR: Could not allocate memory for start node.\n");
    fclose(fp);
    return -1;
  }
  start->addr[0] = '\0';
  start->root = NULL;
  start->totalTerms = 0;
  start->next = NULL;

  crawl(fp, MAX_n, start);

  while (1) {
    printf("\nEnter a web query: ");

    char query[1000];

    // check for invalid read
    if (fgets(query, sizeof(query), stdin) == NULL) {

      printf("Exiting the program.\n");
      break;
    }

    query[strcspn(query, "\n")] = '\0';

    char *trimmedQuery = query;
    while (isspace((unsigned char)*trimmedQuery)) {
      trimmedQuery++;
    }

    // if entered query is empty, terminates
    if (*trimmedQuery == '\0') {
      printf("Exiting the program.\n");
      break;
    }

    // validates if all letters are lowercase
    if (!validate_query(trimmedQuery)) {
      fprintf(stderr,
              "Please enter a query containing only lower-case letters.\n");
      continue;
    }

    printf("Query is \"%s\".\n", trimmedQuery);

    char *idfQueryCopy = strdup(trimmedQuery);
    if (idfQueryCopy == NULL) {
      fprintf(stderr, "ERROR: Memory allocation failed\n");
      continue;
    }

    char *token = strtok(idfQueryCopy, " ");
    while (token != NULL) {
      double idf_value = IDF(token, start);
      printf("IDF scores are \n");
      printf("IDF(%s): %.7f\n", token, idf_value > 0.0 ? idf_value : 0.0);
      token = strtok(NULL, " ");
    }
    free(idfQueryCopy);

    double topScores[3] = {0.0, 0.0, 0.0};
    char topUrls[3][1000] = {"", "", ""};

    struct listNode *current = start;
    while (current != NULL) {
      double score = S(start, *current, trimmedQuery);
      if (score > 0.0) {
        for (int i = 0; i < 3; i++) {
          if (score > topScores[i]) {
            for (int j = 2; j > i; j--) {
              topScores[j] = topScores[j - 1];
              strncpy(topUrls[j], topUrls[j - 1], sizeof(topUrls[j]));
            }
            topScores[i] = score;
            strncpy(topUrls[i], current->addr, sizeof(topUrls[i]) - 1);
            topUrls[i][sizeof(topUrls[i]) - 1] = '\0';
            break;
          }
        }
      }
      current = current->next;
    }

    printf("Web pages:\n");
    for (int i = 0; i < 3; i++) {
      if (topScores[i] > 0.0) {
        printf("%d. %s (score: %.4f)\n", i + 1, topUrls[i], topScores[i]);
      }
    }
  }

  destroyList(start);
  fclose(fp);

  return 0;
}
