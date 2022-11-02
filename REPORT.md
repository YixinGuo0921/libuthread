# LIBUTHREAD: a user-level threading library

## Summary
The goal of Project 2 is to understand the basics of a multi-threading library  
by developing a user-level one named **libuthread**. Modeled after *pthread*,  
libuthread sports a few basic features:

- Cooperative thread creation
- Semaphore support
- Preemption

## FIFO Queue
The first task of this project is to create a basic FIFO queue class that can  
create and destroy new queues; enqueue and dequeue elements in *O(1)* time;  
delete specified objects; and apply a user-defined function to each contained  
element in the queue. This FIFO queue is used throughout the rest of the  
project. In order to accomplish *O(1)* enqueuing and dequeuing, linked lists  
were chosen to construct the foundation of our queue. This would avoid having to  
shift over every element 'leftwards' each time one is dequeued, as one would   
need to do with a traditional array. Additionally, linked lists allow for  
**potentially limitless** size, as opposed to traditional arrays which require  
a set maximum size for initialization.

Doubly linked lists were specifically chosen since it would allow elements to  
see the node behind *and* in front of them. This was essential for ensuring   
that queue_delete() functions properly; that is, if a node is deleted,  
it must know its adjacent nodes in order to reassign their pointers to one  
another. The sources used for this step are as follows:

[Doubly Linked Lists](https://www.tutorialspoint.com/data_structures_algorithms/doubly_linked_list_algorithm.htm)  
[Deletion in a Linked List](https://www.geeksforgeeks.org/deletion-in-linked-list/)

## libuthread.a