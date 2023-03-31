#include "libhashtable.h"
#include <check.h>
#include <stdio.h>

uint64_t _hash(const void *data, const size_t len) {
  uint64_t ret = 5381;
  const void *p = data;
  for (; p != (data + len); p++) {
    ret = ((ret << 5) + ret) + *(char *)data;
  }
  return ret;
}
#ifdef TESTING
START_TEST(test_hash) {
  const char *data_foo = "foo";
  uint64_t hash_foo = _hash(data_foo, strlen(data_foo));
  const char *data_bar = "bar";
  uint64_t hash_bar = _hash(data_bar, strlen(data_bar));
  ck_assert_msg(hash_foo != hash_bar,
                "hash(foo): %lu should not equal hash(bar): %lu", hash_foo,
                hash_bar);
}
END_TEST
#endif

void hash_table_init(hash_table_t *hash_table) {
  // Initializie array of buckets using calloc to null out all data.
  hash_table->buckets = (hash_table_entry_t *)calloc(
      hash_table->bucket_size * hash_table->n_buckets,
      sizeof(hash_table_entry_t));
}
void hash_table_deinit(hash_table_t *hash_table) { free(hash_table->buckets); }

hash_table_entry_t *hash_table_lookup(const void *key, const size_t len,
                                      hash_table_t *hash_table) {
  const uint64_t buckets_idx = _hash(key, len) % hash_table->n_buckets;
  hash_table_entry_t *p = &hash_table->buckets[buckets_idx];
  const hash_table_entry_t *end =
      &hash_table->buckets[buckets_idx + hash_table->bucket_size];
  for (; p != end; p++) {
    if (p == NULL)
      return NULL;
    if (hash_table->cmp(key, p->key))
      return p;
  }
  return NULL;
}
/* TODO: test lookup <27-03-23, Max Schulte> */


int hash_table_insert(hash_table_entry_t *hash_table_entry,
                      hash_table_t *hash_table) {
  const uint64_t buckets_idx = _hash((const void *)hash_table_entry->key,
                                     (const size_t)hash_table_entry->key_len) %
                               hash_table->n_buckets;
  hash_table_entry_t *p = &hash_table->buckets[buckets_idx];
  const hash_table_entry_t *end =
      &hash_table->buckets[buckets_idx + hash_table->bucket_size];
  for (; p != end; p++) {
    fprintf(stderr, "%ld\n", p - (&hash_table->buckets[buckets_idx]));
    if (hash_table->cmp(p->key, hash_table_entry->key))
      continue;
    *p = *hash_table_entry;
    return 0;
  }
  return 1;
}
#ifdef TESTING
bool _test_string_cmp(const char *b, const char *a) {
  if (a == NULL || b == NULL)
    return a == b;
  return strcmp(b, a) == 0;
}
START_TEST(test_hash_table_insert) {
  // Create hashtable.
  hash_table_t hash_table = {
      .bucket_size = 8,
      .n_buckets = 8,
      .cmp = (bool (*)(const void *, const void *)) & _test_string_cmp,
  };
  hash_table_init(&hash_table);
  // Insert entry.
  hash_table_entry_t hash_table_entry = {
      .key = (void *)"foo",
      .key_len = 3,
      .val = (void *)"bar",
  };
  ck_assert_msg(hash_table_insert(&hash_table_entry, &hash_table) == 0,
                "got nonzero error for hash_table_insert");
  // Manually look up the first entry in the bucket.
  const uint64_t buckets_idx = _hash((const void *)hash_table_entry.key,
                                     (const size_t)hash_table_entry.key_len) %
                               hash_table.n_buckets;
  hash_table_entry_t expected_entry = hash_table.buckets[buckets_idx];
  // Make sure what was manually looked up was what we inserted.
  ck_assert_msg(expected_entry.key != NULL,
                "expected entry key should not be NULL");
  ck_assert_msg(expected_entry.val != NULL,
                "expected entry val should not be NULL");
  ck_assert_msg(strcmp(expected_entry.key, hash_table_entry.key) == 0,
                "expected key: %s got: %s", (char *)expected_entry.key,
                (char *)hash_table_entry.key);
  ck_assert_msg(strcmp(expected_entry.val, hash_table_entry.val) == 0,
                "expected val: %s got: %s", (char *)expected_entry.val,
                (char *)hash_table_entry.val);

  hash_table_deinit(&hash_table);
}
END_TEST
#endif

#ifdef TESTING
// Create and run test suite.
Suite *test_suite(void) {
  Suite *s = suite_create("all");
  TCase *tc_core = tcase_create("core");
  tcase_add_test(tc_core, test_hash);
  tcase_add_test(tc_core, test_hash_table_insert);
  suite_add_tcase(s, tc_core);
  return s;
}
int main(void) {
  int number_failed;
  Suite *s = test_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return number_failed;
}
#endif
