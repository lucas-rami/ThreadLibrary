/** @file data_structures.h
 *  @brief This file declares the data structures used in the program.
 *  @author akanjani, lramire1
 */

#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_

typedef struct generic_node {
  void *value;
  struct generic_node *next;
} generic_node_t;

typedef struct generic_double_node {
  void *value;
  struct generic_double_node *next, *prev;
} generic_double_node_t;

typedef struct queue {
  generic_node_t *head, *tail;
} generic_queue_t;

typedef struct linked_list {
  generic_double_node_t *head, *tail;
  int (*find)(void* elem, void* value);
} generic_linked_list_t;

typedef struct hash_table {
  generic_linked_list_t *buckets;
  unsigned int nb_buckets;
  unsigned int (*hash_function)(void* elem, unsigned int nb_buckets);
  int (*find)(void* elem, void* value);
} generic_hash_table_t;

#endif /* _DATA_STRUCTURES_H_ */
