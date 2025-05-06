#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define MAX_LEVEL 25
#define P 0.5

typedef struct SkipListNode {
    int key;
    struct SkipListNode **forward;
} SkipListNode;

typedef struct SkipList {
    int level;
    SkipListNode *header;
} SkipList;

int traversed_nodes = 0;

SkipListNode* createNode(int level, int key) {
    SkipListNode *node = (SkipListNode*)malloc(sizeof(SkipListNode));
    node->key = key;
    node->forward = (SkipListNode**)malloc(sizeof(SkipListNode*) * (level + 1));
    for (int i = 0; i <= level; i++) {
        node->forward[i] = NULL;
    }
    return node;
}

int randomLevel() {
    int level = 0;
    while ((rand() / (double)RAND_MAX) < P && level < MAX_LEVEL)
        level++;
    return level;
}

SkipList* createSkipList() {
    SkipList *list = (SkipList*)malloc(sizeof(SkipList));
    list->level = 0;
    list->header = createNode(MAX_LEVEL, INT_MIN);
    return list;
}

void insert(SkipList *list, int key) {
    SkipListNode *update[MAX_LEVEL + 1];
    SkipListNode *x = list->header;

    for (int i = list->level; i >= 0; i--) {
        while (x->forward[i] && x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }

    x = x->forward[0];

    if (!x || x->key != key) {
        int lvl = randomLevel();
        if (lvl > list->level) {
            for (int i = list->level + 1; i <= lvl; i++)
                update[i] = list->header;
            list->level = lvl;
        }

        x = createNode(lvl, key);
        for (int i = 0; i <= lvl; i++) {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }
    }
}

int search(SkipList *list, int key) {
    SkipListNode *x = list->header;
    for (int i = list->level; i >= 0; i--) {
        while (x->forward[i] && x->forward[i]->key < key) {
            x = x->forward[i];
            traversed_nodes++;
        }
    }
    x = x->forward[0];
    if (x) traversed_nodes++;
    return (x && x->key == key);
}

void generate_unique_random(int *arr, int n, int range_start) {
    for (int i = 0; i < n; i++) {
        arr[i] = range_start + i * 2;
    }
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

int main() {
    srand(time(NULL));
    int samples[] = {5000000, 10000000, 20000000}; 
    int num_samples = sizeof(samples) / sizeof(samples[0]);
    int n;
    int num = 1000000;
    for (int i = 0; i < num_samples; i++)
    {
        clock_t start = clock();
        n = samples[i];
        int *keys = (int*)malloc(sizeof(int) * n);
        printf("\n--- Sample Size: %d ---\n", n);
        generate_unique_random(keys, n, 1);  

        SkipList *list = createSkipList();

        for (int i = 0; i < n; i++) {
            insert(list, keys[i]);
        }
        printf("Done generating skiplist with %d elements.\n", n);
        int success_total = 0;
        int fail_total = 0;
		printf("Searching used keys...\n");
        for (int i = 0; i < num / 2; i++) {
            traversed_nodes = 0;
            search(list, keys[rand() % n]);
            success_total += traversed_nodes;
        }
		printf("Searching unused keys...\n");
        for (int i = 0; i < num / 2; i++) {
            traversed_nodes = 0;
            search(list, 2 * (rand() % (n * 2)));
            fail_total += traversed_nodes;
        }

        double fail, success;
        fail = fail_total / (float)(num / 2);
        success = success_total / (float)(num / 2);
        printf("%.2f\n", fail);
        printf("%.2f\n", success);

        // printf("Average traversed nodes (successful): %.2f\n", success_total / (float)(n / 2));
        // printf("Average traversed nodes (unsuccessful): %.2f\n", fail_total / (float)(n / 2));
        printf("Average traversed nodes: %.2f\n", (fail_total + success_total) / (float)num );

        free(keys);
        printf("Skiplist time: %.6f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC);
    }
    
    
    return 0;
}
