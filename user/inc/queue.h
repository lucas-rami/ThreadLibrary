/** @file queue.h
 *  @brief This file declares the queue structure as well as functions to use
 * the generic queue.
 *  @author akanjani, lramire1
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <generic_node.h>

typedef struct queue { generic_node_t *head, *tail; } generic_queue_t;

int queue_init(generic_queue_t *queue);
int queue_insert_node(generic_queue_t *list, void *value);
void *queue_delete_node(generic_queue_t *list);

#endif /* _QUEUE_H_ */
