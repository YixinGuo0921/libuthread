#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

typedef struct node
{
        void *data;     // holds an address to an unknown data type
        struct node* next;
}node;

struct queue {

        int size;

        node* first;
        node* last;
};

queue_t queue_create(void)
{
        queue_t queue = (queue_t)malloc(sizeof(queue_t));

        queue->size = 0;
        queue->first = (node*)malloc(sizeof(node));
        queue->last = (node*)malloc(sizeof(node));
        queue->first->next = queue->last;

        return queue;
}

int queue_destroy(queue_t queue)
{
        /* TODO Phase 1 */
}

int queue_enqueue(queue_t queue, void *data)
{
        /* TODO Phase 1 */
}

int queue_dequeue(queue_t queue, void **data)
{
        /* TODO Phase 1 */
}

int queue_delete(queue_t queue, void *data)
{
        /* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
        /* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
        /* TODO Phase 1 */
}

