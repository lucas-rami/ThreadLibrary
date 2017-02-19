/** @file hash_table.h
 *  @brief This file declares the functions to use the generic hash table.
 *  @author akanjani, lramire1
 */

#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_

#include <data_structures.h>

int hash_table_init(generic_hash_table_t* hash_table);
int hash_table_add_element(generic_hash_table_t* hash_table, void* elem);
void* hash_table_remove_element(generic_hash_table_t* hash_table, void* elem);
void* hash_table_get_element(generic_hash_table_t* hash_table, void* elem);

#endif /* _HASH_TABLE_H_ */
