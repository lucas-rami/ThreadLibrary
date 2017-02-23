# ThreadLibrary - Operating Systems Design and Implementation - Project 2

## 1.0 Description of data structures

### 1.1 Task state (task_t)



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
