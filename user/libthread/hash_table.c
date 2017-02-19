/** @file linked_list.c
 *
 *  @brief This file contains the definitions for functions which can be used
 *   to manipulate the generic hash table with elements of type void*
 *
 *  @author akanjani, lramire1
 */

#include <data_structures.h>
#include <linked_list.h>
#include <stdlib.h>

int hash_table_init(generic_hash_table_t *hash_table, unsigned int nb_buckets) {

  // Check validity of arguments
  if (hash_table == NULL) {
    return -1;
  }

  hash_table->nb_buckets = nb_buckets;

  // Allocate the buckets
  hash_table->buckets =
      calloc(hash_table->nb_buckets, sizeof(generic_linked_list_t));
  if (hash_table->buckets == NULL) {
    return -1;
  }

  // Initialize eack linked list
  int i;
  for (i = 0; i < hash_table->nb_buckets; ++i) {
    if (linked_list_init(&hash_table->buckets[i]) < 0) {
      free(hash_table->buckets);
      return -1;
    }
  }

  return 0;
}

int hash_table_add_element(generic_hash_table_t *hash_table, void *elem) {

  // Check validity of arguments
  if (hash_table == NULL || hash_table->hash_function == NULL || elem == NULL) {
    return -1;
  }

  // Compute in which bucket the element will go
  unsigned int bucket = hash_table->hash_function(elem, hash_table->nb_buckets);
  if (bucket < 0 || bucket >= hash_table->nb_buckets) {
    return -1;
  }

  // Add the element to the appropriate linked list
  if (linked_list_insert_node(&hash_table->buckets[bucket], elem) < 0) {
    return -1;
  }

  return 0;
}

void* hash_table_remove_element(generic_hash_table_t* hash_table, void* elem) {

  // Check validity of arguments
  if (hash_table == NULL || hash_table->hash_function == NULL || elem == NULL ||
      hash_table->compare == NULL) {
    return NULL;
  }

  // Compute in which bucket the element could be
  unsigned int bucket = hash_table->hash_function(elem, hash_table->nb_buckets);
  if (bucket < 0 || bucket >= hash_table->nb_buckets) {
    return NULL;
  }

  return linked_list_delete_node(&hash_table->buckets[bucket], elem);
}

void *hash_table_get_element(generic_hash_table_t *hash_table, void *elem) {

  // Check validity of arguments
  if (hash_table == NULL || hash_table->hash_function == NULL || elem == NULL ||
      hash_table->compare == NULL) {
    return NULL;
  }

  // Compute in which bucket the element could be
  unsigned int bucket = hash_table->hash_function(elem, hash_table->nb_buckets);
  if (bucket < 0 || bucket >= hash_table->nb_buckets) {
    return NULL;
  }

  return linked_list_get_node(&hash_table->buckets[bucket], elem);
}
