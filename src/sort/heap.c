/**
 * @file src/sort/heap.c
 */

#include "sort/heap.h"
#include <unistd.h>

static void swap(int* a, int* b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

// Maintains the max-heap property for subtree rooted at `i`, in an array of length `n`.
static void heapify(int data[], size_t n, size_t i) {
    size_t largest = i;
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;

    if (left < n && data[left] > data[largest]) {
        largest = left;
    }

    if (right < n && data[right] > data[largest]) {
        largest = right;
    }

    if (largest != i) {
        swap(&data[i], &data[largest]);
        heapify(data, n, largest);
    }
}

void heap_sort(int* data, size_t len) {
    if (!data || len < 2) {
        return;
    }

    // Build max heap
    for (ssize_t i = (ssize_t)(len / 2 - 1); i >= 0; --i) {
        heapify(data, len, (size_t) i);
    }

    // Extract elements one by one from the heap
    for (size_t i = len - 1; i > 0; --i) {
        swap(&data[0], &data[i]); // Move current root to end
        heapify(data, i, 0); // Heapify reduced heap
    }
}
