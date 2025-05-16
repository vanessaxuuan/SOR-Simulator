#ifndef SOR_H
#define SOR_H

typedef struct {
    char name[10];
    float price;     // Ask price
    float latency;   // in milliseconds
    int volume; 
    float score;     // Calculated score
} Exchange;

typedef struct {
    Exchange* ex;
    float min_price, max_price;
    float min_latency, max_latency;
    int min_volume, max_volume;
} ThreadArg;

void normalize(Exchange exchanges[], float norm_price[], float norm_latency[], float norm_volume[]);
int compare(const void *a, const void *b);
void quickSort_calculateScores(Exchange exchanges[]);
void maxHeap_calculateScores(Exchange exchanges[]);
void allocateOrder(Exchange exchanges[], int order_quantity);
void run_multithreaded_scoring(Exchange exchanges[], int count);
void allocateOrder(Exchange exchanges[], int order_quantity);

#endif
