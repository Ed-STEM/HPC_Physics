#ifndef __queue_h__
#define __queue_h__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <pthread.h>

typedef struct Node {
    double data;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
    pthread_mutex_t lock;
} Queue;

Queue* createQueue(void);

void enqueue(Queue* queue, double value);

int dequeue(Queue* queue);

int isEmpty(Queue* queue);

#endif