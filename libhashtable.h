#ifndef LIBHASHTABLE_H
#define LIBHASHTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  void *key;
  size_t key_len;
  void *val;
} hash_table_entry_t;

typedef struct {
  size_t bucket_size;
  size_t n_buckets;
  void *entries;
  hash_table_entry_t *buckets;
  bool (*cmp)(const void *b, const void *a);
} hash_table_t;

/******************************************************************************
 * Function:         void hash_table_init
 * Description:      initialize a hashtable
 * Where:            hash_table_t * hash_table - hash table to initialize
 * Return:           void
 *****************************************************************************/
void hash_table_init(hash_table_t *hash_table);

/******************************************************************************
 * Function:         void hash_table_deinit
 * Description:      deinitialize a hashtable
 * Where:            hash_table_t * hash_table - hash table to deinitialize
 * Return:           void
 *****************************************************************************/
void hash_table_deinit(hash_table_t *hash_table);

/******************************************************************************
 * Function:         hash_table_lookup
 * Description:      look up a key in the hash table returning the address to
 * the corresponding key value pair if it exists else null.
 * Where:
 *                   const void * key - key to look up in the hash table.
 *                   const size_t len - length of the key
 *                   hash_table - hash_table to look up the key in.
 * Return:           hash_table entry if one exists else NULL
 *****************************************************************************/
hash_table_entry_t *hash_table_lookup(const void *key, const size_t len, hash_table_t *hash_table);

/******************************************************************************
 * Function:         hash_table_insert
 * Description:      insert an entry in the hash table overwriting if the same
 * key exists
 * Where:
 *                   hash_table_entry_t *hash_table_entry - entry to insert
 *                   hash_table - hash_table to insert into
 * Return:           non zero error code
 *****************************************************************************/
int hash_table_insert(hash_table_entry_t *hash_table_entry,
                       hash_table_t *hash_table);

uint64_t _hash(const void *data, const size_t len);
#endif
