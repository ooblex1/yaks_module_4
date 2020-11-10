#pragma once
/*
 * hash.h -- A generic hash table implementation, allowing arbitrary
 * key structures.
 *
 */
#include <stdint.h>
#include <stdbool.h>

typedef void hashtable_t;	/* representation of a hashtable hidden */

/* hopen -- opens a hash table with initial size hsize */
hashtable_t *hopen(uint32_t hsize);

/* hclose -- closes a hash table */
void hclose(hashtable_t *htp);

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen);

/* happly -- applies a function to every entry in hash table */
void happly(hashtable_t *htp, void (*fn)(void* ep));

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *hsearch(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *hremove(hashtable_t *htp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

