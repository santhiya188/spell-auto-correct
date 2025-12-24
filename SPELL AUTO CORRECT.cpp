#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ALPHABET 26
#define MAX_WORD 50
#define MAX_SUGGESTIONS 10
#define DICT_SIZE 10

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET];
    int isEndOfWord;
    int frequency;
} TrieNode;

TrieNode* createNode() {
    TrieNode *node = (TrieNode*)malloc(sizeof(TrieNode));

    node->isEndOfWord = 0;
    node->frequency = 0;

    for (int i = 0; i < ALPHABET; i++)
        node->children[i] = NULL;

    return node;
}

void insertWord(TrieNode *root, const char *word, int frequency) {
    TrieNode *crawler = root;

    for (int i = 0; word[i] != '\0'; i++) {
        int index = word[i] - 'a';
        if (index < 0 || index >= ALPHABET) continue;

        if (!crawler->children[index])
            crawler->children[index] = createNode();

        crawler = crawler->children[index];
    }

    crawler->isEndOfWord = 1;
    crawler->frequency = frequency;
}

int editDistance(const char *a, const char *b) {
    int n = strlen(a), m = strlen(b);
    int dp[n + 1][m + 1];

    for (int i = 0; i <= n; i++) dp[i][0] = i;
    for (int j = 0; j <= m; j++) dp[0][j] = j;

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (a[i - 1] == b[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else {
                int ins = dp[i][j - 1] + 1;
                int del = dp[i - 1][j] + 1;
                int rep = dp[i - 1][j - 1] + 1;

                int min = ins < del ? ins : del;
                dp[i][j] = min < rep ? min : rep;
            }
        }
    }
    return dp[n][m];
}

typedef struct {
    char word[MAX_WORD];
    int frequency;
} Suggestion;

Suggestion suggestions[MAX_SUGGESTIONS];
int suggestionCount = 0;

void searchCandidates(TrieNode *root, char *current, int depth, const char *input) {
    if (!root) return;

    if (root->isEndOfWord) {
        current[depth] = '\0';
        int dist = editDistance(input, current);

        if (dist <= 2 && suggestionCount < MAX_SUGGESTIONS) {
            strcpy(suggestions[suggestionCount].word, current);
            suggestions[suggestionCount].frequency = root->frequency;
            suggestionCount++;
        }
    }

    for (int i = 0; i < ALPHABET; i++) {
        if (root->children[i]) {
            current[depth] = 'a' + i;
            searchCandidates(root->children[i], current, depth + 1, input);
        }
    }
}

void sortSuggestions() {
    for (int i = 0; i < suggestionCount - 1; i++) {
        for (int j = i + 1; j < suggestionCount; j++) {
            if (suggestions[j].frequency > suggestions[i].frequency) {
                Suggestion temp = suggestions[i];
                suggestions[i] = suggestions[j];
                suggestions[j] = temp;
            }
        }
    }
}

int main() {
    TrieNode *root = createNode();

    const char *dictionary[DICT_SIZE] = {
        "hello", "help", "helmet", "hero", "cat",
        "cater", "dog", "door", "doll", "data"
    };

    int frequencies[DICT_SIZE] = {
        100, 80, 40, 60, 200,
        30, 150, 90, 50, 120
    };

    for (int i = 0; i < DICT_SIZE; i++)
        insertWord(root, dictionary[i], frequencies[i]);

    char input[MAX_WORD];
    printf("Enter a misspelled word: ");
    scanf("%s", input);

    for (int i = 0; input[i]; i++)
        input[i] = tolower(input[i]);

    char current[MAX_WORD];
    suggestionCount = 0;

    searchCandidates(root, current, 0, input);
    sortSuggestions();

    printf("\nCorrected Suggestions:\n");

    if (suggestionCount == 0)
        printf("No suggestions found!\n");
    else
        for (int i = 0; i < suggestionCount; i++)
            printf("%d. %s (freq: %d)\n",
                   i + 1,
                   suggestions[i].word,
                   suggestions[i].frequency);

    return 0;
}

