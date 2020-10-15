/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 * Author: Ye Zhang
 * Created: Oct9 2020
 * 
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hash.h"
#include "queue.h"

/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *) (d)))

static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

//array of queues
typedef struct hash_t{
	int size;     //size of table
	queue_t** array;   //pointer to an array that contains pointers to queues
}hash_t;

hashtable_t *hopen(uint32_t hsize){
	hash_t* ht;

	if (hsize == 0){
		printf("hsize can't be 0, hopen failed\n");
		return NULL;
	}

	ht = (hash_t*)malloc(sizeof(hash_t));
	if ( ht == NULL ){
		printf("memory allocation failed or hsize is zeron\n");
	}

	ht->array = malloc(hsize * sizeof(queue_t*));
	if ( ht->array == NULL ){
		printf("memory allocation failed for queue\n");
	}
	
	ht->size = hsize;
	for (int i = 0; i<hsize; i++){
		ht->array[i] = qopen();
	}
	
	return (hashtable_t*)ht;
}

void hclose(hashtable_t *htp){
	hash_t* ht = (hash_t*)htp;
	if (ht == NULL){
		printf("hashtable does not exist\n");
	}else{
		for (int i = 0; i < ht->size; i++){
			qclose(ht->array[i]);
		}
		free(ht->array);
		free(ht);	
	}
}

int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen){
	hash_t* ht = (hash_t*)htp;

	if (ht == NULL){
		printf("hashtable does not exist\n");
		return 1;
	}

	uint32_t index = SuperFastHash(key,keylen,ht->size);

	if (qput(ht->array[index],ep)== 0){
			return 0;
	}else{
			return 1;
	}
	
}

void happly(hashtable_t *htp, void (*fn)(void* ep)){
	hash_t* ht = (hash_t*)htp;

	if (ht == NULL){
		printf("hashtable does not exist\n");
	}else{
		for (int i=0; i<ht->size; i++){
			qapply(ht->array[i],fn);
		}
	}
}

void *hsearch(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key,
							int32_t keylen){
	hash_t* ht = (hash_t*)htp;
	if (ht == NULL){
		printf("hashtable does not exist\n");
		return NULL;
	}

	uint32_t index = SuperFastHash(key,keylen,ht->size);

	if(ht->array[index]==NULL){
		printf("Element not found\n");
		return NULL;
	}
	
	void* result = qsearch(ht->array[index],searchfn,(void*)key);
	return result;
}

void *hremove(hashtable_t *htp,
							bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key,
							int32_t keylen){
	hash_t* ht = (hash_t*)htp;
	
	if (ht == NULL){
		printf("hashtable does not exist\n");
		return NULL;
	}

	uint32_t index = SuperFastHash(key,keylen,ht->size);

	if(ht->array[index] == NULL){
		printf("Element not found\n");
		return NULL;
	}

	//printf("%p\n", (void *) ht->array[index]);

	void* result = qremove(ht->array[index],searchfn,key);
	return result;
	
}
