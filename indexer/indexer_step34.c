/* indexer.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Sat Oct 24 13:49:20 2020 (-0400)
 * Version: step 3&4
 * 
 * Description: reads the html associated with every webpage fetched by the crawler
 * constructs in memory an index data structure that can be used to look up a word and find out 1) which document contains the work and 2) how many times the word occur in that document
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"
#include "indexio.h"

//element of hash table
typedef struct wordDoc_t{
	char* word;
	queue_t* doc_q;
} wordDoc_t;

//each hash table contains a word and a doc_t
//doc c contains the document and count
typedef struct doc_t{
	int document;
	int count;
} doc_t;


//below are functions called in main, to simplify structure

//used in h search, whether word is in the hash table
bool inHash(void* elp, const void* keyp){
	if (elp == NULL){
		return NULL;
	}
	wordDoc_t* el = (wordDoc_t *)elp;
	char* key = (char *)keyp;
	return strcmp(el->word,key)==0;
}

bool docExist(void *docp, const void *idp){
	doc_t *dp = (doc_t*) docp;
	int *id = (int*) idp;
	return dp->document == *id;
}


// normalize word： at least 3 characters and lower case. 
char *normalizeWord(char *word){

	//check if more than 3 characters
	if(strlen(word)<3){
		return NULL;
	}

	for(int i=0; word[i]!= '\0'; i++){
		//if not a character then return null
		if(!isalpha(word[1])){
			return NULL;
		}

		word[i] = tolower(word[i]);
	}
	return word;
}


//sum words function

int total = 0;

void sumOneWord(void *elp){
	wordDoc_t *el = (wordDoc_t *)elp;
	doc_t *docq = el->doc_q;
	int increment = docq->count;
	total += increment; 
}

void sumwords(void *htp){
	hashtable_t *ht = (hashtable_t *)htp;
	happly(ht, sumOneWord);
	printf("sum of all counts: %d\n",total);
}


int main(){

	char* pagedir = "../crawler/pages";
	webpage_t *current;
	int id = 1;
	current = pageload(id,pagedir);


	//hashtable for indexer
	hashtable_t *words_ht;
	words_ht=hopen(100);

	char* result;
	int pos=0;

	//STEP2 Code
	
	while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

		// normalize all words
		if(normalizeWord(result)!=NULL){
				
			wordDoc_t *word_exist;
			word_exist=(wordDoc_t*)hsearch(words_ht,inHash,result,strlen(result));

			//create new element if the word doens't exist
			if(word_exist == NULL){ 
				wordDoc_t *new_word = (wordDoc_t*)malloc(sizeof(wordDoc_t));
				doc_t* new_doc = (doc_t*)malloc(sizeof(doc_t));

				//hash table element initialized
				new_word->word = result;
				new_word->doc_q = qopen();

				//docs (element of queue) initialized
				new_doc->document = id; 
				new_doc->count = 1;

				//put doc in he queue
				qput(new_word->doc_q,new_doc);

				//put wordDoc in the hashtable
				hput(words_ht,new_word,result,strlen(result));

			} else{ //otherwise if word already exists

				//check if the doc already exists
				doc_t *doc_exist;
				doc_exist = (doc_t*)qsearch(word_exist->doc_q,docExist,&id);

				//if no such document, put doc in the queue with count one
				if(doc_exist == NULL){ // if the doc is not in the queue
					doc_t* new_doc = (doc_t*)malloc(sizeof(doc_t));
					new_doc->document = id; 
					new_doc->count = 1;
					qput(word_exist->doc_q,new_doc);
				} else // if the doc is in the queue, increase count
					doc_exist->count++;
					
				//free(result);

			}

		}else{
			free(result);
		}
	}

				
	sumwords(words_ht);	
	webpage_delete(current);



}