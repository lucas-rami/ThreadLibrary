# ThreadLibrary - Operating Systems Design and Implementation - Project 2

## 1 Description of Data Structures

### 1.1 Task State (task_t)

The task_t data structure defined in global_state.h holds information about the
task currently running.

The stack_lowest and stack_highest fields denote the boundaries of the stack
space for all threads in the task (for example, stack_lowest is the lowest
address of the thread's stack with the lowest address space). stack_lowest may
be updated in the thr_create() function when a new stack is allocated at lower
addresses than the one with the lowest address space.

Each thread created in the task get assigned a "library issued TID" in addition
to its kernel TID. The library TIDs are assigned in increasing order, starting
from 0. More can be found on this design decision in section 2.2.               

Since the stack_lowest and library_tid fields may be accessed at the same time
by multiple threads running thr_create(), they are protected with a mutex.

The stack_highest_childs field contains the highest address for the "child"
threads (i.e. all threads that are not the root thread). This value is used to
computer the memory location of the TCB for each thread (see 1.2 and 2.3).      

The task's global state stores the set of TCBs for all threads in the task in a
thread-safe hash table. The hash_table API gives a way for any thread in the
task to access the TCBs of other threads, given their library issued TID. The
only exception is for the root thread of a task, which for reasons described
later in this document, has its TCB stored directly in a field of the task_t
data structure (root_tcb).

The task's global state also contains a queue (stack_queue), which is thread-
safe by implementation, storing the list of free stack "slots". When a child
thread (i.e. non-root thread) exits, then its stack gets "deallocated" and the
lowest address of its stack is put in the queue. Later, when a new child
thread is created, the parent first looks in this queue. If it is empty,
then the parent thread allocated a new stack towards lower addresses. But if it
is not, then the first value is popped out of it, and the child thread is given
the stack space corresponding to the address which was in the queue.
The goal of this mechanism is to prevent segmentation in the stack region of a
task. We make sure that the stack space is densely populated with stacks of
active threads.


### 1.2 Thread Control Block (tcb_t)

The tcb_t data structure defined in global_state.h holds information about one
particular thread in the currently running task.

Each thread's TCB is created in the thr_create() function by the parent thread
before the child thread is created. The only exception to this rule is for the
root thread, which creates its own TCB in the thr_init() function. After a TCB
is created, it is put in the hash table containing all the task's TCBs by the
creator thread (see 1.1).

When a TCB is created, the fields called library_tid, stack_low and
stack_high are set to their final value (they are never modified during the
thread execution, hence they do not need to be protected with a lock). The
return_status field may only be written to by the thr_exit() function of the
thread owning the TCB, hence we do not protect it with a lock neither.

The kernel_tid field, storing the kernel issued ID for the thread, is protected
with a mutex for reasons described in 2.2.                           

The thread_state field indicates whether the thread is RUNNING, EXITED, or if it
is WAITED_ON, which means that some other thread has called thr_join() on it and
is waiting for it to finish execution. We use a condition variable to wake up
any thread waiting on the TCB's owner to exit. Since the thread_state field may
be accessed by different threads at the same time, we protect it using a mutex.

At the bottom (highest address) of every thread stack, there is a pointer to the
thread's TCB. How the highest address of a thread stack can be determined
without knwoledge of the TCB is explained in section 2.1.2. The root thread of a
task is again an exception to this rule, its TCB being stored directly in the
task_t data structure of a task.


## 2 Important Design Decisions

### 2.1 Stack design

#### 2.1.1 Task stack design

The diagram presented below shows the general stack architecture of a multi-
threaded task running with out thread library. The values for some fields of the
task_t data structure are displayed on the right. What is referred to as
"stack space" for each of the non root-thread is detailed in 2.1.2.

0xff...ff
            |                      |     
            |----------------------|  <-- task.stack_highest   
            |     Root-thread      |
            |        stack         |
            |----------------------|  <-- task.stack_highest_childs + PAGE_SIZE
            |      Guard page      |
            |----------------------|  <-- task.stack_highest_childs
            |      Thread 1        |
            |    "stack space"     |
            |----------------------|  <-- task.stack_highest_childs
            |      Thread 2        |        - task.stack_size - 2 * PAGE_SIZE
            |    "stack space"     |
            |----------------------|  <-- task.stack_highest_childs
            |          .           |        - 2(task.stack_size - 2 * PAGE_SIZE)
            |          .           |
            |          .           |
            |----------------------|
            |      Thread n        |
            |    "stack space"     |
            |----------------------| <-- task.stack_lowest
            |                      |
0x00...00

#### 2.1.2 Thread stack design

The diagram belows shows the design we chose for threads stacks. Each thread has
a one page long exception stack used by the exception handler. Also, between
two threads there is always a guard page which is not allocated. It allows us
to detect when a thread overflows its stack because a SIGSEGV signal will be
raised. Without these guard pages, a thread could potentially corrupt the stacks
located below him while remaining undetected, hence causing the task's behavior
to become unpredictable.

0xff...ff
            |                      |     
            |----------------------|  <-- task.stack_highest_childs =
            |      Thread 1        |          tcb1.stack_high
            |   exception stack    |
            |----------------------|  <-- tcb1.stack_high - PAGE_SIZE
            |      Thread 1        |
            |   execution stack    |
            |----------------------|  <-- tcb1.stack_high - PAGE_SIZE  
            |      Thread 1        |       - task.stack_size = tcb1.stack_low
            |     guard page       |
            |----------------------|  <-- tcb1.stack_high - 2 * PAGE_SIZE
            |      Thread 2        |       - task.stack_size = tcb2.stack_high
            |   exception stack    |
            |----------------------|  <-- tcb2.stack_high - PAGE_SIZE
            |      Thread 2        |
            |   execution stack    |
            |----------------------|  <-- tcb2.stack_high - PAGE_SIZE  
            |      Thread 2        |          - task.stack_size = tcb2.stack_low
            |     guard page       |
            |----------------------|
            |          .           |
            |          .           |            
            |          .           |        
0x00...00

Since each thread stack has the same size than the others (except for the root
thread), each thread can determine dynamically the highest address of its stack
without even knowing its TCB by using the task.stack_highest_childs value and
simple arithmetic (see get_tcb() function).

### 2.2 Library issued TID VS Kernel issue TID

We make use of library issued TIDS in our thread library. Having library issued
TIDs allow us to avoid making a gettid() system call sometimes, and consequently
it speeds up our library. The TIDs accepted and returned by our library
functions are these library issued TIDs, which in some cases need to be
converted to their kernel equivalent to perform some system calls such as
deschedule() or make_runnable(). We explain below how library TIDs can speed up
our system.

It is the responsibility of the parent thread to update the kernel TID field of
the TCB for the newly created child thread, and it does it just after
thread_fork() returns in thr_create(). However, the new child thread might
return from thread_fork() before its parent, and its kernel TID won't be there
yet. However, the child thread TCB already contains the library TID so the child
thread might be able to perform some work with its library TID even without
knowing its kernel TID. Eventually, when a thread wants to know its own kernel
TID it will first check its TCB to see if it is there, and in the case where
it is not, will make the gettid() call itself.
In the case that a thread A wants to know the kernel TID of another thread B,
it will get if from the TCB if it exists, otherwise it will wait for some other
thread (namely thread B or its parent) to update it. That's why we use a
condition variable and a mutex to for the kernel_tid field of each TCB.


### 2.3 Stack Initialization in thr_create()

When thr_create() is called, the parent thread initializes the stack for the
new child thread before calling thread_fork(). We decided to build the child
stack so that it starts by calling a function called stub(), which at its turn
calls the function func(arg) that the child thread is supposed to execute. The
purpose of this stub() function is dual. Its first goal is to register an
exception handler for the newly created thread. The second one is to ensure
that even if the func() function returns instead of calling thr_exit(), stub()
can make the call to thr_exit() itself.

### 2.4 Malloc library

Our implementation makes it unnecessary to call thr_init() to initialize the
malloc library, which initializes itself the first time one of its function is
called. To achieve this we use a mutex and an atomic operation using the XCHG
instruction.

### 2.5 Mutexes

Our implementation uses Lamport's bakery algorithm. which ensures mutual exclusion,
progress and bounded waiting. If a thread get a "ticket" to enter the critical section
but has to wait for its turn, it calls yield(-1) so another thread (and eventually the
one holding the mutex) can run while it is waiting. Other alternatives were considered
while implementing mutexes: a simple spinlock which forced each thread to busy wait
until they got the mutex or an implementation using a queue to keep track of the order
in which threads were asking for the mutex (and hence ensure bounded waiting). These
two alternatives were not as fast as Lamport's bakery algorithm, and that's why this
last was chosen.

### 2.6 Conditional Variables

Our implementation of conditional variables makes use of a queue to keep track of all
threads waiting on a conditional variable. This queue is thread-safe by default.
If is fine if someone makes a call to cond_signal() or cond_broadcast() while the
queue is empty. The funtion will simply return without waking up any thread.

### thr_join() and thr_exit()
