/** @file data_structures.h
 *  @brief This file declares the generic_node_t structure.
 *  @author akanjani, lramire1
 */

#ifndef _DATA_STRUCTURES_H_
#define _DATA_STRUCTURES_H_

typedef struct generic_node {
  void *value;
  struct generic_node *next;
} generic_node_t;

#endif /* _DATA_STRUCTURES_H_ */
