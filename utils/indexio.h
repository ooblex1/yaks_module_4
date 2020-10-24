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

typedef struct wordDoc_t{
	char* word;
	queue_t* doc_q;
} wordDoc_t;

/*
typedef struct doc_t{
	int document;
	int count;
} doc_t;
*/



int32_t indexsave(hashtable_t *indexp, char *indexnm);


hashtable_t *indexload(char *indexnm);
