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
that `queue_delete()` properly functions; that is, if a node is deleted,  
it must know both of its adjacent nodes in order to reassign their pointers to  
one another. The sources used for creating the FIFO queue are as follows:

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
freed. Using a queue for `libuthread` would also allow us to use the many  
useful functions that `queue.c` has to offer. `thread_queue` was decided to be a  
global variable to ensure that every `uthread` function would have proper access  
to it.

The test cases indicated that `uthread_run()` is where the threading library  
execution began, so that's where development began as well. The components are  
as follows:  
- The `idle` thread throughout `libuthread.a`'s runtime *always* refers to the  
infinite loop within `uthread_run()`. Its primary job is to assign the next TCB  
in queue to the `initial` thread and switch to it. `idle` was decided to be a  
global variable so that any function in the threading library can reliably refer  
to it in order to switch contexts.  
- The `initial` thread is initially assigned the function that the caller  
program requests (using `uthread_run()`'s parameters), and if this function  
creates a new thread using `uthread_create()`, that new thread would be queued  
to the back of `thread_queue`. It will then eventually be assigned to `initial`  
by `idle` once all others before it either yield or execute until completion.  
The `initial` thread was decided to be a local variable within `uthread_run()`,  
since no other `uthread` function requires access to it.

In short, the `idle` thread initially assigns the user-requested function to  
`initial`, and once that function yields or exits, control is returned to `idle`  
for it to choose the *next* thread, if there exists one. Control is **always**  
returned to idle before choosing the next thread; `uthread_yield()` and  
`uthread_exit()` have no power to switch to the next thread by themselves. They  
can only *modify* `thread_queue` such that `idle` can then correctly choose the  
next thread. This functionality was tested with the given programs as well as  
`uthread_yield2.c`, which would output a different order of statements if  
`uthread_yield()` did not yield the current thread correctly.

The **RUNNING** state informs the rest of the program which thread is running.  
It is assigned in the `idle` thread *only*, which works out since threads will  
*always* switch to the idle thread to pick the next thread. The **READY** state  
simply 

### Semaphore library
Similar to cooperative threading, the test cases and project document were  
referred to heavily in developing this phase. Initially, the semaphore struct  
was composed of these two members:

```
struct semaphore {
        queue_t waiting_room;
        unsigned int resource;
};
```

`sem_up()` and `sem_down()` both have two courses of action, both of which  
depend on one of the above members. 

- `sem_down()` subtracts a resource and returns to the thread if there are  
enough resources (e.g. if `resource` is greater than zero); otherwise, it will  
queue the thread into `waiting_room` and block it using `uthread_block()`.  
- `sem_up()` will *always* increase the number of resources by one, and if  
there is *any* thread in waiting room, it will dequeue the oldest requesting  
thread and pass it to `uthread_unblock()`.

`uthread_block()` and `uthread_unblock()` simply change the passed thread to a  
**BLOCKED** and **UNBLOCKED** state respectively. We considered also removing  
them from `thread_queue` within `uthread.c`, but decided that it would be best  
to keep the notion that `thread_queue` contains all existing incomplete threads,  
regardless of if they're blocked or not. In order to account for this, the  
`idle` thread only switches contexts to the new thread if its state is NOT  
**BLOCKED** (i.e. if its state is **UNBLOCKED** or **READY**), otherwise it will  
choose the next thread in queue.

#### Corner Case

To deal with the semaphore library's corner case, we used the **UNBLOCKED**  
thread state and implemented one more member to the semaphore struct:  

```
queue_t released_threads;
```

This queue keeps track of every thread released by `sem_up()`, making use of the     
distinction between **UNBLOCKED** and **READY**. If a resource is released and  
there exists **UNBLOCKED** threads within `released_threads`, that means that  
those threads were *supposed* to take the resource but had it stolen from them,  
thus creating the corner case. If the threads in `released_threads` are of any  
other state, it means that they were ran by `idle` and thus legally took their  
resources. Our implementation of semaphores therefore iterates through  
`released_threads` every time a resource is released normally, changing any  
**UNBLOCKED** threads back to being **BLOCKED**, thus preventing them from  
running.

### Preemption
The premise of this phase is fairly detached from the rest of the project.  
Whereas other phases added further functionality to `uthread.c`, this phase  
simply adds a 'different mode' for the rest of `libuthread` to run in. This  
means modification to anything outside of `preempt.c` for this phase was scarce,  
apart from disabling preemption in critical areas.

If the `preempt` boolean is set to false when calling `uthread_run()`,  
`preempt_start()` will execute but return after initializing sigset `ss` (this  
is so `preempt_{enable, disable}()` does not refer to an uninitialized sigset).  
Otherwise, it will perform three actions:  

- Add `SIGALRM` and `SIGVTALRM` to `ss`; this will be used for enabling and  
disabling preemption.
- Set up the alarm handler such that any `SIGVTALRM` signal will execute the  
handler, instead of simply exiting.  
- set up the alarm itself with `setitimer()`, which utilizes the `itimerval`  
structure for its microsecond precision.  

Preemption disabling and enabling make use of `sigprocmask()`, blocking and  
unblocking `ss` signals (`SIGALRM` and `SIGVTALRM`) respectively
