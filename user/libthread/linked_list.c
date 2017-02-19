/** @file linked_list.c
 *
 *  @brief This file contains the definitions for functions which can be used
 *   to insert or delete elements from the generic linked kist with elements
 *   of type void*
 *
 *  @author akanjani, lramire1
 */

#include <data_structures.h>
#include <linked_list.h>
#include <stdlib.h>

int linked_list_init(generic_linked_list_t *list) {

  // Check validity of arguments
  if (list == NULL) {
    return -1;
  }

  // Initialize the linked list
  list->head = NULL;
  list->tail = NULL;
  return 0;
}

int linked_list_insert_node(generic_linked_list_t *list, void *value) {

  // Check validity of arguments
  if (list == NULL || value == NULL) {
    return -1;
  }

  // Allocate a new node
  generic_double_node_t *new_node = malloc(sizeof(generic_double_node_t));
  if (new_node == NULL) {
    return -1;
  }
  new_node->value = value;
  new_node->next = NULL;

  if (list->head == NULL && list->tail == NULL) {
    // Linked list is empty
    new_node->prev = NULL;
    list->head = new_node;
    list->tail = new_node;
  } else {
    // Linked list is non-empty
    new_node->prev = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
  }

  return 0;
}

void *linked_list_delete_node(generic_linked_list_t *list, void *value) {

  // Check validity of arguments
  if (list == NULL || value == NULL || list->compare == NULL) {
    return NULL;
  }

  // Iterator on the list's elements
  generic_double_node_t *node = list->head;

  // Loop over the list
  while (node != NULL) {
    if (!list->compare(node->value, value)) {
      if (node->prev == NULL && node->next == NULL) {
        // Node is only element in linked list
        list->head = NULL;
        list->tail = NULL;
      } else if (node->prev == NULL) {
        // Node is list's head
        list->head = node->next;
        list->head->prev = NULL;
      } else if (node->next == NULL) {
        // Node is list's tail
        list->tail = node->prev;
        list->tail->next = NULL;
      } else {
        // Node is between other nodes
        node->prev->next = node->next;
        node->next->prev = node->prev;
      }
      return node;
    }
    node = node->next;
  }
  return NULL;
}

void *linked_list_get_node(const generic_linked_list_t *list, void *value) {

  // Check validity of arguments
  if (list == NULL || value == NULL || list->compare == NULL) {
    return NULL;
  }

  // Iterator on the list's elements
  generic_double_node_t *iterator = list->head;

  // Loop over the list
  while (iterator != NULL) {
    if (!list->compare(iterator->value, value)) {
      return iterator;
    }
    iterator = iterator->next;
  }

  return NULL;
}
