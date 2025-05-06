#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct TreapNode {
    int key;
	unsigned int priority;
    struct TreapNode *left, *right;
} TreapNode;

unsigned int hash_wang(int key) {
    unsigned int x = (unsigned int)key;
    x = (x ^ 61) ^ (x >> 16);
    x += x << 3;
    x ^= x >> 4;
    x *= 0x27d4eb2d;
    x ^= x >> 15;
    return x;
}

TreapNode* newNode(int key) {
    TreapNode* node = (TreapNode*)malloc(sizeof(TreapNode));
    node->key = key;
    // node->priority = rand();
	node->priority = hash_wang(key);
    node->left = node->right = NULL;
    return node;
}

TreapNode* rightRotate(TreapNode* y) {
    TreapNode* x = y->left;
    TreapNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    return x;
}

TreapNode* leftRotate(TreapNode* x) {
    TreapNode* y = x->right;
    TreapNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    return y;
}

TreapNode* insert(TreapNode* root, int key, int* inserted) {
    if (!root) {
        *inserted = 1;
        return newNode(key);
    }
    if (key < root->key) {
        root->left = insert(root->left, key, inserted);
        if (root->left->priority > root->priority)
            root = rightRotate(root);
    } else if (key > root->key) {
        root->right = insert(root->right, key, inserted);
        if (root->right->priority > root->priority)
            root = leftRotate(root);
    } else {
        *inserted = 0;
    }
    return root;
}

int counter(TreapNode* root, int key, int* count) {
    (*count)++;
    if (!root) return 0;
    if (root->key == key) return 1;
    if (key < root->key)
        return counter(root->left, key, count);
    return counter(root->right, key, count);
}

void inorder(TreapNode* root) {
    if (root) {
        inorder(root->left);
        printf("key: %d | priority: %d\n", root->key, root->priority);
        inorder(root->right);
    }
}

void freeTreap(TreapNode* root) {
    if (!root) return;
    freeTreap(root->left);
    freeTreap(root->right);
    free(root);
}

TreapNode* generate(TreapNode** root_ref, int n, int used_keys[]) {
    TreapNode* root = NULL;
    int inserted, key, i = 0;
    while (i < n) {
        key = rand() % (10 * n);
        inserted = 0;
        root = insert(root, key, &inserted);
        if (inserted) {
            used_keys[i++] = key;
        }
    }
    *root_ref = root;
	// printf("Inorder traversal:\n");
	// inorder(root);
    return root;
}

void generate_unused_keys(int* unused_keys, int n, int* used_keys, int used_n) {
	int max_key = 10 * used_n;
    bool *used_hash_map = malloc(max_key * sizeof(bool));  
    if (!used_hash_map) {
        fprintf(stderr, "Memory allocation failed for used_hash_map\n");
        exit(1);
    }
    for (int i = 0; i < 10 * used_n; i++) {
        used_hash_map[i] = false;  
    }

    for (int i = 0; i < used_n; i++) {
        used_hash_map[used_keys[i]] = true;
    }

    int i = 0;
    while (i < n) {
        int candidate = rand() % (10 * used_n);

        if (!used_hash_map[candidate]) {
            unused_keys[i++] = candidate;
            used_hash_map[candidate] = true;  
        }
    }
}

// int exists(int* arr, int size, int key) {
//     for (int i = 0; i < size; i++) {
//         if (arr[i] == key)
//             return 1;
//     }
//     return 0;
// }

// void generate_unused_keys(int* unused_keys, int n, int* used_keys, int used_n) {
//     int i = 0;
//     while (i < n) {
//         int candidate = rand() % (10 * used_n);
//         if (!exists(used_keys, used_n, candidate) &&
//             !exists(unused_keys, i, candidate)) {
//             unused_keys[i++] = candidate;
//         }
//     }
// }

double average(int* arr, int size) {
    int total = 0;
    for (int i = 0; i < size; i++) {
        total += arr[i];
    }
    return size > 0 ? (double)total / size : 0.0;
}
void printSearchPath(TreapNode* root, int key) {
    printf("Search path  %d: ", key);
    while (root) {
        printf("%d ", root->key);
        if (key == root->key) {
            printf("(found)\n");
            return;
        } else if (key < root->key) {
            root = root->left;
        } else {
            root = root->right;
        }
    }
    printf("(not found)\n");
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
		int* used_keys = malloc(n * sizeof(int));
		int* unused_keys = malloc(n * sizeof(int));
		int* used_comparisons = malloc(num/2 * sizeof(int));
		int* unused_comparisons = malloc(num/2 * sizeof(int));

		if (!used_keys || !unused_keys || !used_comparisons || !unused_comparisons) {
			fprintf(stderr, "Memory allocation failed\n");
			exit(1);
		}

		TreapNode* root = NULL;

		printf("\n--- Sample Size: %d ---\n", n);
		root = generate(&root, n, used_keys);
		printf("Done generating treap with %d elements.\n", n);

		generate_unused_keys(unused_keys, n, used_keys, n);

		printf("\nSearching used keys...\n");
		for (int i = 0; i < num / 2; i++) {
			int count = 0;
			counter(root, used_keys[i], &count);
			used_comparisons[i] = count;
			// printf("Found %d in %d comparisons\n", used_keys[i], count);
			// printSearchPath(root, used_keys[i]);
		}

		printf("\nSearching unused keys...\n");
		for (int i = 0; i < num / 2; i++) {
			int count = 0;
			counter(root, unused_keys[i], &count);
			unused_comparisons[i] = count;
			// printf("Not found %d in %d comparisons\n", unused_keys[i], count);
			// printSearchPath(root, used_keys[i]);
		}

		double avg_used = average(used_comparisons, num / 2);
		double avg_unused = average(unused_comparisons, num / 2);

		printf("Average traversed nodes: %.2f\n", (avg_unused + avg_used)/2);
		printf("Treap time: %.6f seconds\n", (double)(clock() - start) / CLOCKS_PER_SEC);

		freeTreap(root);
		free(used_keys);
		free(unused_keys);
		free(used_comparisons);
		free(unused_comparisons);

	}
	

    return 0;
}
