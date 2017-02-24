/** @file data_structures.h
 *  @brief This file declares the generic_node_t structure.
 *  @author akanjani, lramire1
 */

#ifndef _DATA_STRUCTURES_H
#define _DATA_STRUCTURES_H

typedef struct generic_node {

  /** @brief A void pointer type member which stores the data to be stored in
   *   every node
   */
  void *value;

  /** @brief A struct generic_node* type member which stores the address of
   *   the next node in a lnked list
   */
  struct generic_node *next;
} generic_node_t;

#endif /* _DATA_STRUCTURES_H */
