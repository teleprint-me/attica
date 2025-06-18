/**
 * @file tests/sort/test_heap.c
 */

#include "core/logger.h"
#include "test/unit.h"
#include "sort/heap.h"

#include <assert.h>
#include <stdio.h>

static void test_heap_sort_basic() {
    int data[] = {5, 2, 9, 1, 5, 6};
    heap_sort(data, 6);
    int expected[] = {1, 2, 5, 5, 6, 9};
    (void) expected; // compiler claims this is unused.
    for (size_t i = 0; i < 6; ++i) {
        assert(data[i] == expected[i]); // it obviously is used here.
    } // stupid compiler. :p
}

int main(void) {
    test_heap_sort_basic();
    printf("heap_sort: all tests passed\n");
    return 0;
}
