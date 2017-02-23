/** @file hash_table.h
 *  @brief This file declares the the hash table structure as well as functions
 * to use the generic hash table.
 *  @author akanjani, lramire1
 */

#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include <linked_list.h>
#include <mutex_type.h>

typedef struct hash_table {
  generic_linked_list_t *buckets;
  mutex_t *mutexes;
  unsigned int nb_buckets;
  unsigned int (*hash_function)(void *elem, unsigned int nb_buckets);
  int (*find)(void *elem, void *value);
} generic_hash_table_t;

int hash_table_init(generic_hash_table_t *hash_table, unsigned int nb_buckets,
                    int (*find)(void *, void *),
                    unsigned int (*hash_function)(void *, unsigned int));
int hash_table_add_element(generic_hash_table_t *hash_table, void *elem);
void *hash_table_remove_element(generic_hash_table_t *hash_table, void *elem);
void *hash_table_get_element(generic_hash_table_t *hash_table, void *elem);

#endif /* _HASH_TABLE_H_ */
