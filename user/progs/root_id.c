
#include <thread.h>
#include <global_state.h>
#include <simics.h>
#include <stdlib.h>

int main(void) {

  thr_init(PAGE_SIZE);

  lprintf("----- get TCB");

  tcb_t* tcb = get_tcb();

  if (tcb == NULL) {
    lprintf("----- tcb is null");
    return -1;
  }

  lprintf("----- get TID");

  int tid = thr_getid();

  lprintf("----- TID is %d", tid);

  lprintf("----- get kernel TID");

  int k_tid = thr_get_kernel_id(tid);

  lprintf("----- kernel TID is %d", k_tid);

  return 0;
}
