#include "queue.h"
#include "dbg.h"

#include <stdio.h>
#include <stdlib.h>

Queue* createQueue(void){
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    check_mem(queue, "createQueue failed to allocate queue. ");
    pthread_mutex_init(&queue->lock, NULL);
    queue->front = queue->rear = NULL;
    return queue;
}

void enqueue(Queue* queue, double value){
    pthread_mutex_lock(&queue->lock);
    Node* newNode = (Node*)malloc(sizeof(Node));
    check_mem(newNode, "createQueue failed to allocate queue. ");

    newNode->data = value;
    newNode->next = NULL;

    if (queue->rear == NULL){
        queue->front = queue->rear = newNode;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
    pthread_mutex_unlock(&queue->lock);
}

int dequeue(Queue* queue){
    pthread_mutex_lock(&queue->lock);
    if(queue->front == NULL){
        log_warn("Queue is empty.") ;
        return -1;
    }

    Node* temp = queue->front;
    double data = temp->data;
    queue->front = queue->front->next;

    if(queue->front == NULL){
        queue->rear = NULL;
    }
    
    free(temp);
    pthread_mutex_unlock(&queue->lock);
    return data;
}

int isEmpty(Queue* queue){
    return queue->front == NULL;
}