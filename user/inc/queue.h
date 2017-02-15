/** @file queue.h
 *  @brief This file declares the functions to use the generic queue.
 *  @author akanjani, lramire1
 */

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct generic_node {
  void *value;
  struct generic_node *next;
} generic_node_t;

typedef struct queue {
  generic_node_t *head, *tail;
} generic_queue_t;

int insert_node( generic_queue_t *list, void *value );
void *delete_node( generic_queue_t *list );

#endif /* _QUEUE_H_ */
