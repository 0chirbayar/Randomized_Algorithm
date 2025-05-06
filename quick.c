#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define GROUP_SIZE 5

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

int median_of_medians(int *arr, int left, int right) {
    if (right - left + 1 <= GROUP_SIZE) {
        qsort(arr + left, right - left + 1, sizeof(int), compare);
        return arr[left + (right - left) / 2];
    }

    int num_groups = (right - left + 1 + GROUP_SIZE - 1) / GROUP_SIZE;
    int *medians = malloc(num_groups * sizeof(int));
    if (!medians) {
        fprintf(stderr, "Memory allocation failed for medians.\n");
        exit(EXIT_FAILURE);
    }

    int median_index = 0;
    for (int i = left; i <= right; i += GROUP_SIZE) {
        int group_right = i + GROUP_SIZE - 1;
        if (group_right > right) group_right = right;
        qsort(arr + i, group_right - i + 1, sizeof(int), compare);
        medians[median_index++] = arr[i + (group_right - i) / 2];
    }

    int result = median_of_medians(medians, 0, median_index - 1);
    free(medians);
    return result;
}

int partition(int *arr, int left, int right, int depth, int max_depth) {
    int pivot, index;
    if (depth >= max_depth) {
        pivot = median_of_medians(arr, left, right);
        for (index = left; index <= right; index++) {
            if (arr[index] == pivot) break;
        }
    } else {
        index = left + rand() % (right - left + 1);
    }

    int temp = arr[index];
    arr[index] = arr[right];
    arr[right] = temp;

    pivot = arr[right];
    int i = left;
    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            int tmp = arr[i];
            arr[i++] = arr[j];
            arr[j] = tmp;
        }
    }
    arr[right] = arr[i];
    arr[i] = pivot;

    return i;
}

void quicksort(int *arr, int left, int right, int max_depth, int depth) {
    while (left < right) {
        int index = partition(arr, left, right, depth, max_depth);
        if (index - left < right - index) {
            quicksort(arr, left, index - 1, max_depth, depth + 1);
            left = index + 1;
        } else {
            quicksort(arr, index + 1, right, max_depth, depth + 1);
            right = index - 1;
        }
    }
}

int quickselect(int *arr, int left, int right, int k, int max_depth, int depth) {
    while (left <= right) {
        int index = partition(arr, left, right, depth, max_depth);
        if (index - left == k) {
            return arr[index];
        } else if (index - left < k) {
            k -= (index - left + 1);
            left = index + 1;
        } else {
            right = index - 1;
        }
    }
    return -1; 
}

double test_quickselect(int *arr, int n, int max_depth, int trials) {
    double total = 0.0;
    for (int t = 0; t < trials; ++t) {
        int k = rand() % n;

        int *copy = malloc(n * sizeof(int));
        if (!copy) {
            fprintf(stderr, "Memory allocation failed for quickselect copy.\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < n; ++i) copy[i] = arr[i];

        clock_t start = clock();
        quickselect(copy, 0, n - 1, k, max_depth, 0);
        total += (double)(clock() - start) / CLOCKS_PER_SEC;
        free(copy);
    }
    return total / trials;
}

double test_quicksort(int *arr, int n, int max_depth) {
    clock_t start = clock();
    quicksort(arr, 0, n - 1, max_depth, 0);
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

int *generate_array(int n) {
    int *arr = malloc(n * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Memory allocation failed for array.\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; ++i)
        arr[i] = rand() % 1000000000;
    return arr;
}

int main() {
    srand((unsigned)time(NULL));
    int samples[] = {10000000, 20000000, 40000000, 80000000, 160000000}; 
    int num_samples = sizeof(samples) / sizeof(samples[0]);
    double qselect[5];
    double qsort[5];
    for (int i = 0; i < num_samples; ++i) {
        int n = samples[i];
        int *arr = generate_array(n);
        int max_depth = 30;

        printf("\n--- Sample Size: %d ---\n", n);
        printf("Testing quickselect)...\n");
        double avg_qs = test_quickselect(arr, n, max_depth, 1000);
        qselect[i] = avg_qs;
        printf("Avg. quickselect time: %.6f seconds\n", avg_qs);

        int *arr_copy = malloc(n * sizeof(int));
        if (!arr_copy) {
            fprintf(stderr, "Memory allocation failed for quicksort.\n");
            free(arr);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < n; ++j) arr_copy[j] = arr[j];
        free(arr); 

        printf("Testing quicksort...\n");
        double sort_time = test_quicksort(arr_copy, n, max_depth);
        printf("Quicksort time: %.6f seconds\n", sort_time);
        qsort[i] = sort_time;

        free(arr_copy);
    }

    printf("%-15s %-10s %-10s %-10s %-10s %-10s\n", "", "10M", "20M", "40M", "80M", "160M");
    printf("%-15f %-10f %-10f %-10f %-10f %-10f\n", "quicksort", qsort[0], qsort[1], qsort[2], qsort[3], qsort[4]);
    printf("%-15f %-10f %-10f %-10f %-10f %-10f\n", "quickselect", qselect[0], qselect[1], qselect[2], qselect[3], qselect[4]);

    return 0;
}
