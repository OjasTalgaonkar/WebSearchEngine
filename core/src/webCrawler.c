#include "../include/webCrawler.h"

int crawl(FILE *fp, int MAX, struct listNode *start) {
  int n = 0;

  if (start == NULL) {
    fprintf(stderr, "ERROR: could not allocate memory\n");
    return -2;
  }

  printf("Indexing...\n");

  char url[MAX_ADDR_LENGTH];
  char destUrl[MAX_ADDR_LENGTH];
  int maxHops;
  while (n < MAX) {
    if (fscanf(fp, "%s %d", url, &maxHops) != 2) {
      break;
    }

    url[MAX_ADDR_LENGTH - 1] = '\0';

    int hopNum = 0;
    while (1) {
      strncpy(destUrl, url, MAX_ADDR_LENGTH);
      if (!contains(start, destUrl)) {
        printf("%s\n", destUrl);
        insertBack(start, destUrl);
        n++;
      }
      hopNum++;
      if (hopNum <= maxHops && n < MAX) {
        int res = getLink(destUrl, url, MAX_ADDR_LENGTH);
        if (!res) {
          break;
        }
      } else {
        break;
      }
    }
  }
  return 1;
}

void insertBack(struct listNode *pNode, const char *addr) {
  // Ensure we're at the end of the list
  if ((pNode->next) == NULL) {
    struct listNode *newNode =
        (struct listNode *)malloc(sizeof(struct listNode));
    if (newNode == NULL) {
      fprintf(stderr, "ERROR: Memory allocation failed\n");
      return;
    }

    newNode->addr[0] = '\0';
    strncpy(newNode->addr, addr, MAX_ADDR_LENGTH - 1);
    newNode->addr[MAX_ADDR_LENGTH - 1] = '\0';

    newNode->root = NULL;
    newNode->totalTerms = 0;
    newNode->next = NULL;

    newNode->root = indexPage(addr, &(newNode->totalTerms));

    pNode->next = newNode;
    return;
  }

  insertBack(pNode->next, addr);
}

int contains(const struct listNode *pNode, const char *addr) {
  if (pNode == NULL) {
    return 0;
  }

  if (strcmp(pNode->addr, addr) == 0) {
    return 1;
  }

  return contains(pNode->next, addr);
}

void destroyList(struct listNode *pNode) {

  if (pNode == NULL) {
    return;
  }

  destroyList(pNode->next);

  freeTrieMemory(pNode->root);
  free(pNode);
}

int getLink(const char *srcAddr, char *link, const int maxLinkLength) {
  const int bufSize = 1000;
  char buffer[bufSize];

  int numLinks = 0;

  FILE *pipe;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getLinks.py", srcAddr);

  pipe = popen(buffer, "r");
  if (pipe == NULL) {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
    return 0;
  }

  fscanf(pipe, "%d\n", &numLinks);

  if (numLinks > 0) {
    int linkNum;
    double r = (double)rand() / ((double)RAND_MAX + 1.0);

    for (linkNum = 0; linkNum < numLinks; linkNum++) {
      fgets(buffer, bufSize, pipe);

      if (r < (linkNum + 1.0) / numLinks) {
        break;
      }
    }

    strncpy(link, buffer, maxLinkLength);
    link[maxLinkLength - 1] = '\0';

    {
      char *pNewline = strchr(link, '\n');
      if (pNewline != NULL) {
        *pNewline = '\0';
      }
    }
  }

  pclose(pipe);

  if (numLinks > 0) {
    return 1;
  } else {
    return 0;
  }
}

//-----------part 2 changes --------------//

int listLength(const struct listNode *head) {
  int count = 0;

  head = head->next;

  while (head != NULL) {
    count++;
    head = head->next;
  }

  return count;
}

int countNodesWithTerm(const struct listNode *head, const char *term) {
  int count = 0;

  while (head != NULL) {
    if (head->root != NULL) {

      if (searchKey(head->root, term) > 0) {
        count++;
      }
    }
    head = head->next;
  }

  return count;
}

double TF(const char *term, struct TrieNode *root, int totalTerms) {
  if (totalTerms == 0) {
    return 0.0; // Avoid div by zero
  }
  int termCount = searchKey(root, term);
  return (double)termCount / totalTerms;
}

double IDF(const char *term, struct listNode *start) {
  return log((1.0 + listLength(start)) /
             (1.0 + countNodesWithTerm(start, term)));
}

double S(struct listNode *start, struct listNode node, const char *query) {
  double sum = 0.0;
  char *queryCopy = strdup(query);
  if (queryCopy == NULL) {
    fprintf(stderr, "ERROR: Memory allocation failed\n");
    return 0.0;
  }

  char *token = strtok(queryCopy, " ");
  while (token != NULL) {
    double tf = TF(token, node.root, node.totalTerms);
    double idf = IDF(token, start);
    sum += tf * idf;
    token = strtok(NULL, " ");
  }

  free(queryCopy);
  return sum;
}

int validate_query(const char *query) {
  for (int i = 0; query[i] != '\0'; i++) {
    if (!islower(query[i]) && !isspace(query[i])) {
      return 0;
    }
  }
  return 1;
}
