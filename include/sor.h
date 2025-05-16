#ifndef SOR_H
#define SOR_H

typedef struct {
    char name[10];
    float price;     // Ask price
    float latency;   // in milliseconds
    int volume;      // Available volume
    float score;     // Calculated score
} Exchange;

void normalize(Exchange exchanges[], float norm_price[], float norm_latency[], float norm_volume[]);
int compare(const void *a, const void *b);
void quickSort_calculateScores(Exchange exchanges[]);
void maxHeap_calculateScores(Exchange exchanges[]);
void allocateOrder(Exchange exchanges[], int order_quantity);

#endif
