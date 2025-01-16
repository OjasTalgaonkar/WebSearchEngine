#include "../include/indexer.h"

#define BUFFER_SIZE 300000 // max buffer size for page content
#define ALPHABET_SIZE 26

struct TrieNode *indexPage(const char *url, int *totalTerms) {

  char buffer[BUFFER_SIZE];
  int bytesRead = getText(url, buffer, BUFFER_SIZE);

  struct TrieNode *root = createNode();

  char word[100];
  int wordLength = 0;

  for (int i = 0; i < bytesRead; i++) {
    char c = buffer[i];

    // Check if character is alphabetic
    if (isalpha(c)) {
      // Convert uppercase letters to lowercase
      if (isupper(c)) {
        c = tolower(c);
      }

      // Add character to word if within bounds
      if (wordLength < sizeof(word) - 1) {
        word[wordLength++] = c;
      }
    } else if (wordLength > 0) {
      // Null-terminate the word and add it to the Trie
      (*totalTerms)++;
      word[wordLength] = '\0';
      addWordOccurrence(word, wordLength, root);
      printf("\t%s\n", word);
      // Reset word storage for the next word
      wordLength = 0;
    }
  }
  // Add any remaining word at the end of the buffer
  if (wordLength > 0) {
    word[wordLength] = '\0';
    addWordOccurrence(word, wordLength, root);
    printf("\t%s\n", word);
  }

  return root;
}

int addWordOccurrence(const char *word, const int wordLength,
                      struct TrieNode *root) {
  if (wordLength > 0) {
    insert(root, word);
  }
  return 0;
}

struct TrieNode *createNode() {
  struct TrieNode *newNode = malloc(sizeof(struct TrieNode));
  newNode->count = 0;

  // initialize pointers in array to NULL aka no children
  for (int i = 0; i < 26; i++) {
    newNode->children[i] = NULL;
  }

  return newNode;
}

void freeTrieMemory(struct TrieNode *node) {
  if (node == NULL)
    return;
  // free children
  for (int i = 0; i < ALPHABET_SIZE; i++) {
    freeTrieMemory(node->children[i]);
  }
  // free root
  free(node);
}

void insert(struct TrieNode *root, const char *key) {
  struct TrieNode *currentNode = root;
  while (*key) {
    int index = *key - 'a';

    if (!currentNode->children[index]) {
      currentNode->children[index] = createNode();
    }

    currentNode = currentNode->children[index];

    key++; // next letter
  }

  currentNode->count += 1;
}

int getText(const char *srcAddr, char *buffer, const int bufSize) {
  FILE *pipe;
  int bytesRead;

  snprintf(buffer, bufSize, "curl -s \"%s\" | python3 getText.py", srcAddr);

  pipe = popen(buffer, "r");
  if (pipe == NULL) {
    fprintf(stderr, "ERROR: could not open the pipe for command %s\n", buffer);
    return 0;
  }

  bytesRead = fread(buffer, sizeof(char), bufSize - 1, pipe);
  buffer[bytesRead] = '\0';

  pclose(pipe);

  return bytesRead;
}

//-----------part 2 addition-----//
int searchKey(struct TrieNode *root, const char *key) {
  struct TrieNode *currentNode = root;
  while (*key) {
    int index = *key - 'a';

    if (!currentNode->children[index]) {
      return 0;
    }

    currentNode = currentNode->children[index];
    key++;
  }

  if (currentNode->count > 0) {
    return currentNode->count;
  }
  return 0;
}