/** @file linked_list.h
 *  @brief This file declares the functions to use the generic linked list.
 *  @author akanjani, lramire1
 */

#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <data_structures.h>

int linked_list_init(generic_linked_list_t *list, int (*find)(void *, void *));
int linked_list_insert_node(generic_linked_list_t *list, void *value);
void *linked_list_delete_node(generic_linked_list_t *list, void *value);
void *linked_list_get_node(const generic_linked_list_t *list, void *value);

#endif /* _LINKED_LIST_H_ */
