#pragma once
/* lhash.h --- a locked hash table
 * 
 * 
 * Author: Ye Zhang
 * Created: Sun Nov  8 23:01:09 2020 (-0500)
 * Version: 
 * 
 * Description: 
 * 
 */


#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include "hash.h"

typedef struct lhash_t{
	hashtable_t* ht;
	pthread_mutex_t *m;
}lhash_t;

typedef void lhashtable_t;	/* representation of a hashtable hidden */

/* hopen -- opens a hash table with initial size hsize */
lhashtable_t *lhopen(uint32_t hsize);

/* hclose -- closes a hash table */
void lhclose(lhashtable_t *lhtp);

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen);

/* happly -- applies a function to every entry in hash table */
void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep));

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *lhremove(lhashtable_t *lhtp, 
	      bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen);


