#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/*
* @data: The address to an unknown data type
* @next: The node after this one (next in queue)
* @prev: The node before this one
*/
typedef struct node
{
        void *data;
        struct node* next;
        struct node* prev;
}node;

/*
* @size: The number of nodes in queue
* @first: the first node in the queue (to be dequeued next)
* @last: the last node in queue (the most recently queued)
*/
struct queue {

        int size;

        node* first;
        node* last;
};

node* newNode(void* data)
{
        node* tmp = (node*)malloc(sizeof(node));

        if (tmp == NULL) // malloc failed
                return NULL; 

        tmp->data = data;
        tmp->next = NULL;
        tmp->prev = NULL;
        return tmp;
}

queue_t queue_create(void)
{
        queue_t queue = (queue_t)malloc(sizeof(struct queue));

        if (queue == NULL) // malloc failed
                return NULL;

        // nothing in queue yet
        queue->size = 0;
        queue->first = queue->last = NULL;

        return queue;
}

int queue_destroy(queue_t queue)
{
        if (queue == NULL) return -1;
        if (queue->size != 0) return -1;

        // destroy queue
        free(queue);

        return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
        if (queue == NULL || data == NULL) return -1;

        node* tmp = newNode(data);

        // Only element in queue
        if (queue->size == 0)
        {
                queue->first = queue->last = tmp;
                queue->size++;
                return 0;
        }

        // Assign appropriate pointers
        tmp->prev = queue->last;
        queue->last->next = tmp; 

        // enqueue
        queue->last = tmp; 
        queue->size++;

        return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
        if (queue == NULL || data == NULL) return -1;
        if (queue->size == 0) return -1;

        node* tmp = queue->first;

        // Save data
        *data = queue->first->data;

        // Replace oldest in queue
        queue->first = queue->first->next;
        queue->size--;

        // Reset queue->first pointers
        if (queue->size != 0)
                queue->first->prev = NULL;

        // Empty queue if no more elements
        if (queue->first == NULL)
                queue->last = NULL;

        free(tmp);

        return 0;
}

int queue_delete(queue_t queue, void *data)
{
        if (queue == NULL || data == NULL) return -1;
        
        node* current = queue->first;
        node* prev = NULL;

        if (current == NULL) return -1;

        // First element
        if (current->data == data) {
                queue_dequeue(queue, &data);
                return 0;
        }

        // Iterate through queue until match found
        while (current->data != data && current != NULL) {
                prev = current;
                current = current->next;
        }
        if (current == NULL) return -1;
        
        // Data found
        if (current == queue->last)
        {
                queue->last = current->prev;
                queue->last->next = NULL;

                free(current);
                queue->size--;
                return 0;
        }

        // Reassign both adjacent node's pointers 
        current->next->prev = prev;
        prev->next = current->next;

        free(current);
        queue->size--;
        return 0;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
        if (queue == NULL || func == NULL) return -1;

        node* element = queue->first;

        //Apply function to every node in queue
        while (element != NULL) {
                node* tmp = element->next;
                func(queue, element->data);
                element = tmp;
        }

        return 0;
}

int queue_length(queue_t queue)
{
        return queue->size;
}

