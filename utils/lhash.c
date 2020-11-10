/* lhash.c --- implements a generic locked hash table 
 * 
 * 
 * Author: Ye Zhang
 * Created: Sun Nov  8 23:01:26 2020 (-0500)
 * Version: 1
 * 
 * 
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "lhash.h"
#include "queue.h"
#include "hash.h"


lhashtable_t *lhopen(uint32_t hsize){
	if (hsize == 0){
		printf("hsize can't be 0, hopen failed\n");
		return NULL;
	}

	lhash_t* lht;
	if(!(lht = (lhash_t*)malloc(sizeof(lhash_t)))){
		printf("Malloc Failed\n");
	}
	lht->ht = hopen(hsize);
	lht->m=(pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(lht->m,NULL);
	return (lhashtable_t*)lht;
}

void lhclose(lhashtable_t *lhtp){
	if (lhtp == NULL){
		printf("the lhashtable does not exist\n");
		return;
	}

	
	lhash_t* lht = (lhash_t*)lhtp;
	pthread_mutex_lock(lht->m);
	hclose(lht->ht);
	if (lht->m!=NULL) {
			pthread_mutex_destroy(lht->m);
			free(lht->m);
	}
	free(lht);
}

int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen){
	if (lhtp == NULL){
		printf("the lhashtable does not exist\n");
		return 1;
	}
	if (ep == NULL){
		printf("the element does not exist\n");
		return 1;
	}
	if (key == NULL){
		printf("the key does not exist\n");
		return 1;
	}
	
	lhash_t* lht = (lhash_t*)lhtp;

	if (lht == NULL){
		printf("hashtable does not exist\n");
		return 1;
	}

	int32_t result;
	pthread_mutex_lock(lht->m);
	result = hput(lht->ht,ep,key,keylen);
	pthread_mutex_unlock(lht->m);
	return result;
	
}

void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)){
	if (lhtp == NULL){
		printf("the lhashtable does not exist\n");
		return;
	}

	if (fn == NULL){
		printf("the function does not exist\n");
		return;
	}

	lhash_t* lht = (lhash_t*)lhtp;
	pthread_mutex_lock(lht->m);
	happly(lht->ht,fn);
	pthread_mutex_unlock(lht->m);
}

void *lhsearch(lhashtable_t *lhtp, bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key,
							int32_t keylen){

	if (lhtp == NULL){
		printf("the lhashtable does not exist\n");
		return NULL;
	}

	if (key == NULL){
		printf("the key does not exist\n");
		return NULL;
	}

	if (searchfn == NULL){
		printf("the function does not exist\n");
		return NULL;
	}

	lhash_t* lht = (lhash_t*)lhtp;
	void* result;
	pthread_mutex_lock(lht->m);
	result = hsearch(lht->ht,searchfn,key,keylen);
	pthread_mutex_unlock(lht->m);
	return result;
}

void *lhremove(lhashtable_t *lhtp,
							bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key,
							int32_t keylen){
	if (lhtp == NULL){
		printf("the lhashtable does not exist\n");
		return NULL;
	}

	if (key == NULL){
		printf("the key does not exist\n");
		return NULL;
	}

	if (searchfn == NULL){
		printf("the function does not exist\n");
		return NULL;
	}


	lhash_t* lht = (lhash_t*)lhtp;
	void* result;
	pthread_mutex_lock(lht->m);
	result = hremove(lht->ht,searchfn,key,keylen);
	pthread_mutex_unlock(lht->m);
	return result;
}