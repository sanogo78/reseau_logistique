#include <stdbool.h>
#ifndef QUEUE_H
#define QUEUE_H

// Structure pour une file FIFO
typedef struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int *array;
} Queue;

Queue *createQueue(unsigned capacity);
bool isFull(Queue *queue);
bool isEmpty(Queue *queue);
void enqueue(Queue *queue, int item);
int dequeue(Queue *queue);

#endif