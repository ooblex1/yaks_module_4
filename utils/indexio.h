/* 
 * indexio.h
 * Description: 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <hash.h>
#include <queue.h>




int32_t indexsave(hashtable_t *indexp, char *indexnm);


hashtable_t *indexload(char *indexnm);
