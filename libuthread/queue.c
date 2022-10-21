#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct node
{
        void *data;     // holds an address to an unknown data type
        struct node* next;
        struct node* prev;
}node;

struct queue {

        int size;

        node* first;
        node* last;
};

node* newNode(void* data)
{
        node* tmp = (node*)malloc(sizeof(node));
        tmp->data = data;
        tmp->next = NULL;
        tmp->prev = NULL;
        return tmp;
}

queue_t queue_create(void)
{
        queue_t queue = (queue_t)malloc(sizeof(queue_t));

        queue->size = 0;
        queue->first = queue->last = NULL;

        return queue;
}

int queue_destroy(queue_t queue)
{
        if (queue == NULL) return -1;
        if (queue->size != 0) return -1;

        //destroy queue
        free(queue);

        return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
        if (queue == NULL || data == NULL) return -1;

        node* tmp = newNode(data);

        //Only element in queue
        if (queue->size == 0)
        {
                queue->first = queue->last = tmp;
                queue->size++;
                return 0;
        }

        //Set up pointers
        tmp->prev = queue->last;
        queue->last->next = tmp; 

        //enqueue
        queue->last = tmp; 
        queue->size++;

        return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
        if (queue == NULL || data == NULL) return -1;
        if (queue->size == 0) return -1;

        node* tmp = queue->first;

        //Save data
        *data = queue->first->data;

        //Replace oldest in queue
        queue->first = queue->first->next;
        queue->first->prev = NULL;
        queue->size--;

        //Empty queue if no more elements
        if (queue->first == NULL)
                queue->last = NULL;

        free(tmp);

        return 0;
}

int queue_delete(queue_t queue, void *data)
{
        if (queue == NULL || data == NULL) return -1;
        
        node* element = queue->first;

        while (element != NULL)
        {
                if (element->data != data)
                {
                        element = element->next;
                        continue;
                }

                //Reassign elements
                element->prev->next = element->next;
                
                free(element);
                return 0;
        }

        return -1;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
        /* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
        return queue->size;
}

