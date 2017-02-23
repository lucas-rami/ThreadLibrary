/** @file linked_list.h
 *  @brief This file declares the linked list structure as well as functions to
 * use the generic linked list.
 *  @author akanjani, lramire1
 */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <generic_node.h>
#include <mutex_type.h>

typedef struct linked_list {
  generic_node_t *head, *tail;
  int (*find)(void *elem, void *value);
} generic_linked_list_t;

int linked_list_init(generic_linked_list_t *list, int (*find)(void *, void *));
int linked_list_insert_node(generic_linked_list_t *list, void *value,
                            mutex_t *mp);
void *linked_list_delete_node(generic_linked_list_t *list, void *value,
                              mutex_t *mp);
void *linked_list_get_node(const generic_linked_list_t *list, void *value,
                           mutex_t *mp);

#endif /* _LINKED_LIST_H_ */
