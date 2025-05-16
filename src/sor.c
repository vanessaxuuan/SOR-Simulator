#include "../include/config.h"
#include <../include/heapsort.h>
#include "../include/sor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Scale values into a common range between 0 and 1 
void normalize(Exchange exchanges[], float norm_price[], float norm_latency[], float norm_volume[]) {
    float min_price = exchanges[0].price, max_price = exchanges[0].price;
    float min_latency = exchanges[0].latency, max_latency = exchanges[0].latency;
    int min_vol = exchanges[0].volume, max_vol = exchanges[0].volume;

    for (int i = 1; i < NUM_EXCHANGES; i++) {
        if (exchanges[i].price < min_price) min_price = exchanges[i].price;
        if (exchanges[i].price > max_price) max_price = exchanges[i].price;

        if (exchanges[i].latency < min_latency) min_latency = exchanges[i].latency;
        if (exchanges[i].latency > max_latency) max_latency = exchanges[i].latency;

        if (exchanges[i].volume < min_vol) min_vol = exchanges[i].volume;
        if (exchanges[i].volume > max_vol) max_vol = exchanges[i].volume;
    }

    for (int i = 0; i < NUM_EXCHANGES; i++) {
        norm_price[i] = 1.0f - (exchanges[i].price - min_price) / (max_price - min_price + 1e-6f);
        norm_latency[i] = 1.0f - (exchanges[i].latency - min_latency) / (max_latency - min_latency + 1e-6f);
        norm_volume[i] = (float)(exchanges[i].volume - min_vol) / (max_vol - min_vol + 1e-6f);
    }
}

// Descending order
int compare(const void *a, const void *b) {
    Exchange *ex1 = (Exchange *)a;
    Exchange *ex2 = (Exchange *)b;
    return (ex2->score > ex1->score) - (ex2->score < ex1->score);  // Descending
}

// Sort exchanges by score in descending order (Quick Sort)
void quickSort_calculateScores(Exchange exchanges[]) {
    // Scaled values (0-1)
    float norm_price[NUM_EXCHANGES], norm_latency[NUM_EXCHANGES], norm_volume[NUM_EXCHANGES];
    normalize(exchanges, norm_price, norm_latency, norm_volume);

    // Calculate scores
    for (int i = 0; i < NUM_EXCHANGES; i++) {
        exchanges[i].score = 
            W_PRICE * norm_price[i] + 
            W_LATENCY * norm_latency[i] + 
            W_VOLUME * norm_volume[i];
    }

    // Quick sort [O(n log n)]
    qsort(exchanges, NUM_EXCHANGES, sizeof(Exchange), compare);
}

// Sort exchanges by score in descending order (Max Heap)
void maxHeap_calculateScores(Exchange exchanges[]) {
    float norm_price[NUM_EXCHANGES], norm_latency[NUM_EXCHANGES], norm_volume[NUM_EXCHANGES];
    normalize(exchanges, norm_price, norm_latency, norm_volume);

    for (int i = 0; i < NUM_EXCHANGES; i++) {
        exchanges[i].score = 
            W_PRICE * norm_price[i] + 
            W_LATENCY * norm_latency[i] + 
            W_VOLUME * norm_volume[i];
    }

    // Max-heap sort [O(n log n)] with initial heapify [O(n)] & subsequent extractions (pop max and heapify) at O(log n)
    heapSort(exchanges, NUM_EXCHANGES);
}

// Evaluate each exchange concurrently
void* evaluate_exchange(void* arg) {
    ThreadArg* threadArg = (ThreadArg*)arg;
    Exchange* ex = threadArg->ex;
    float norm_price, norm_latency, norm_volume;
    printf("\nExecuting Thread %s\n", ex->name);

    // Normalize
    norm_price = 1.0f - (ex->price - threadArg->min_price) / (threadArg->max_price - threadArg->min_price + 1e-6f);
    norm_latency = 1.0f - (ex->latency - threadArg->min_latency) / (threadArg->max_latency - threadArg->min_latency + 1e-6f);
    norm_volume = (float)(ex->volume - threadArg->min_volume) / (threadArg->max_volume - threadArg->min_volume + 1e-6f);

    // Calculate score using normalized values
    ex->score = W_PRICE * norm_price +
                W_LATENCY * norm_latency +
                W_VOLUME * norm_volume;

    return NULL;
}

// Multithreaded evaluation of exchange scores
void run_multithreaded_scoring(Exchange exchanges[], int count) {
    // Calculate min/max values first
    float min_price = exchanges[0].price, max_price = exchanges[0].price;
    float min_latency = exchanges[0].latency, max_latency = exchanges[0].latency;
    int min_volume = exchanges[0].volume, max_volume = exchanges[0].volume;

    for (int i = 1; i < count; i++) {
        if (exchanges[i].price < min_price) min_price = exchanges[i].price;
        if (exchanges[i].price > max_price) max_price = exchanges[i].price;
    
        if (exchanges[i].latency < min_latency) min_latency = exchanges[i].latency;
        if (exchanges[i].latency > max_latency) max_latency = exchanges[i].latency;
    
        if (exchanges[i].volume < min_volume) min_volume = exchanges[i].volume;
        if (exchanges[i].volume > max_volume) max_volume = exchanges[i].volume;
    }

    // Allocate thread arguments on heap to ensure they live throughout thread execution
    ThreadArg* threadArgs = (ThreadArg*)malloc(count * sizeof(ThreadArg));
    if (!threadArgs) {
        fprintf(stderr, "Failed to allocate memory for thread arguments\n");
        return;
    }

    pthread_t* threads = (pthread_t*)malloc(count * sizeof(pthread_t));
    if (!threads) {
        fprintf(stderr, "Failed to allocate memory for threads\n");
        free(threadArgs);
        return;
    }

    // Create threads
    for (int i = 0; i < count; i++) {
        threadArgs[i].ex = &exchanges[i];
        threadArgs[i].min_price = min_price;
        threadArgs[i].max_price = max_price;
        threadArgs[i].min_latency = min_latency;
        threadArgs[i].max_latency = max_latency;
        threadArgs[i].min_volume = min_volume;
        threadArgs[i].max_volume = max_volume;

        int ret = pthread_create(&threads[i], NULL, evaluate_exchange, &threadArgs[i]);
        if (ret != 0) {
            fprintf(stderr, "Failed to create thread %d: %s\n", i, strerror(ret));
            // Clean up already created threads
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            free(threadArgs);
            free(threads);
            return;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < count; i++) {
        int ret = pthread_join(threads[i], NULL);
        printf("\nThread %d Merging\n", i);
        if (ret != 0) {
            fprintf(stderr, "Failed to join thread %d: %s\n", i, strerror(ret));
        }
    }

    // Clean up
    free(threadArgs);
    free(threads);

    // Sort the exchanges by score
    heapSort(exchanges, count);
}

// Allocate order to the best exchange(s)
void allocateOrder(Exchange exchanges[], int order_quantity) {
    printf("\nAllocating %d shares based on ranking:\n\n", order_quantity);

    for (int i = 0; i < NUM_EXCHANGES && order_quantity > 0; i++) {
        int allocation = exchanges[i].volume < order_quantity ? exchanges[i].volume : order_quantity;
        if (allocation > 0) {
            printf("\n✅ %s: Allocated %d shares (Score: %.2f)\n", exchanges[i].name, allocation, exchanges[i].score);
            order_quantity -= allocation;
        }
    }

    if (order_quantity > 0) {
        printf("\n❌ Unable to fully fulfill the order. %d shares remaining.\n", order_quantity);
    } else {
        printf("\n🎉 Order fully fulfilled.\n");
    }
}