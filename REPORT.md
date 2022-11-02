# LIBUTHREAD: a user-level threading library

## Summary
The goal of Project 2 is to understand the basics of a multi-threading library  
by developing a user-level one named `libuthread`. Modeled after `pthread`,  
libuthread sports a few basic features:

- Cooperative thread creation
- Semaphore support
- Preemption

## FIFO Queue
The first task of this project is to create a basic FIFO queue class that can  
create and destroy new queues; enqueue and dequeue elements in `O(1)` time;  
delete specified objects; and apply a user-defined function to each contained  
element in the queue. This FIFO queue is used throughout the rest of the  
project. In order to accomplish `O(1)` enqueuing and dequeuing, linked lists  
were chosen to construct the foundation of our queue. This would avoid having to  
shift over every element 'leftwards' each time one is dequeued, as one would   
need to do with a traditional array. Additionally, linked lists allow for  
*potentially limitless* size, as opposed to traditional arrays which require  
a set maximum size for initialization.

Doubly linked lists were specifically chosen since it would allow elements to  
see the node behind *and* in front of them. This was essential for ensuring   
that queue_delete() functions properly; that is, if a node is deleted,  
it must know its adjacent nodes in order to reassign their pointers to one  
another. The sources used for creating the FIFO queue are as follows:

[Doubly Linked Lists](https://www.tutorialspoint.com/data_structures_algorithms/doubly_linked_list_algorithm.htm)  
[Deletion in a Linked List](https://www.geeksforgeeks.org/deletion-in-linked-list/)

## libuthread.a
Creating `libuthread` itself can be decomposed into three phases. First, a  
cooperative threading structure is implemented, where each thread must  
voluntarily yield their run. Second, a semaphore library is implemented in order  
to better handle multiple threads using a 'resource system.' There exist a  
finite number of resources that threads pull from and return constantly.  
Finally, preemption must be implemented such that no one thread hogs the CPU for  
itself.

### Cooperative Threading
This step by far took the most planning and effort to implement. As with most  
phases in this project, we began by looking at the given testing files and the  
project document. The first thing we did was define what was needed inside the  
thread TCB structure:

```
struct uthread_tcb {
        uthread_ctx_t* thread_ctx;
        void* stack_ptr;
        int state;
};
```

Going by the project document and `uthread_ctx_init()` in `context.c`, we  
decided on these three components.

We realized it would be greatly benificial to create a queue that would contain  
**all** existing threads, dequeuing them 'in line' for execution and enqueuing  
new ones to the back. If a thread that's executing yields, it would simply be  
deleted from its current spot and requeued to the back of `thread_queue`. If a  
thread exited due to completion, it would simply be deleted from queue and  
freed. This queue would be a global variable to ensure every `uthread` function  
would have access to it.

The test cases indicated that `uthread_run()` is where the threading library  
execution began, so that's where development began as well. The `idle` thread  
was decided to be a global variable such that any function in the threading  
library can reliably refer to it in order to switch contexts. This `idle` thread  
throughout `libuthread.a`'s use always refers to the infinite loop within  
`uthread_run()`, as its primary job is to assign new TCBs to the `initial`  
thread. The `initial` was decided to be a local variable within `uthread_run()`,  
constantly updating its 'held' thread throughout the while loop's entire  
runtime.



