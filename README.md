# ThreadLibrary - Operating Systems Design and Implementation - Project 2

## 1.0 Description of Data Structures

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
from 0. More can be found on this design decision in section x.x.               // Update section

Since the stack_lowest and library_tid fields may be accessed at the same time
by multiple threads running thr_create(), they are protected with a mutex.

The stack_highest_childs field contains the highest address for the "child"
threads (i.e. all threads that are not the root thread). This value is used to
computer the memory location of the TCB for each thread (see 1.2 and x.x).      // Update section

The task's global state stores the set of TCBs for all threads in the task in a
thread-safe hash table. The hash_table API gives a way for any thread in the
task to access the TCBs of other threads, given their library issued TID. The
only exception is for the root thread of a task, which for reasons described
later in this document, has its TCB stored directly in a field of the task_t
data structure (root_tcb).

The task's global state also contains a queue (stack_queue), which is thread-
safe by implementation, storing the list of free stack "slots". When a child
thread (i.e. non-root thread) exits, then its stack gets deallocated and the
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
with a mutex for reasons described in section x.x.                              // Update section

The thread_state field indicates whether the thread is RUNNING, EXITED, or if it
is WAITED_ON, which means that some other thread has called thr_join() on it and
is waiting for it to finish execution. We use a condition variable to wake up
any thread waiting on the TCB's owner to exit. Since the thread_state field may
be accessed by different threads at the same time, we protect it using a mutex.

At the bottom (highest address) of every thread stack, there is a pointer to the
thread's TCB. How the highest address of a thread stack can be determined
without knwoledge of the TCB is explained in section x.x. The root thread of a  // Update section
task is again an exception to this rule, its TCB being stored directly in the
task_t data structure of a task.
