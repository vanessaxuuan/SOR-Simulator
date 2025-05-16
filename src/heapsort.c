#include "../include/heapsort.h"
#include "../include/sor.h"
#include <stdio.h>

void heapify(Exchange arr[], int n, int i) {
    int largest = i;          // Initialize largest as root
    int left = 2 * i + 1;     // Left child index
    int right = 2 * i + 2;    // Right child index

    // If left child is larger
    if (left < n && arr[left].score > arr[largest].score)
        largest = left;

    // If right child is larger
    if (right < n && arr[right].score > arr[largest].score)
        largest = right;

    // If largest is not root
    if (largest != i) {
        Exchange temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest);
    }
}

void heapSort(Exchange arr[], int n) {
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // One by one extract elements
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
        Exchange temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        // Call heapify on the reduced heap
        heapify(arr, i, 0);
    }

    // Reverse to get descending order
    for (int i = 0; i < n / 2; i++) {
        Exchange temp = arr[i];
        arr[i] = arr[n - i - 1];
        arr[n - i - 1] = temp;
    }
}
