#ifndef _PAGE_FAULT_HANDLER_H_
#define _PAGE_FAULT_HANDLER_H_

#include <ureg.h>

void singlethread_handler(void* arg, ureg_t *ureg);

#endif /* _PAGE_FAULT_HANDLER_H_ */
