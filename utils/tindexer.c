// /* tindexer.c --- 
//  * 
//  * 
//  * Author: Aadhya N. Kocha
//  * Created: Thu Nov 12 18:59:08 2020 (-0500)
//  * Version: 
//  * 
//  * Description: a multi-threaded implementation using indexer
//  * 
//  */
// #include <stdint.h>
// #include <stdbool.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <pthread.h>
// #include "lhash.h"
// #include "queue.h"
// #include "hash.h"

// int32_t lindexsave(lhashtable_t *lht, char *indexnm){
//     if (indexnm == NULL) {
//         printf("index is null! \n");
//         return 1;
//     }

//     fp = fopen(indexnm,"w");
//     if (fp == NULL) {
//         printf("usage: directory not found");
//         return 2;
//     }
//     happly(ht,hprintf);
//     fclose(fp);
    
//     return 0;
// }

// void indexBuild(lhashtable_t *words_ht, int id, webpage_t *current) {
// 	int pos = 0;
// 	char* result;
// 	while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

// 		// normalize all words
// 		if(normalizeWord(result)!=NULL && strlen(result) > 2){
// 			wordDoc_t *word_exist;
// 			word_exist=(wordDoc_t*)hsearch(words_ht,inHash,result,strlen(result));

// 			//create new element if the word doens't exist
// 			if(word_exist == NULL){ 
// 				wordDoc_t *new_word = (wordDoc_t*)malloc(sizeof(wordDoc_t));
// 				doc_t* new_doc = (doc_t*)malloc(sizeof(doc_t));

// 				//hash table element initialized
// 				new_word->word = result;
// 				new_word->doc_q = qopen();

// 				//docs (element of queue) initialized
// 				new_doc->document = id; 
// 				new_doc->count = 1;

// 				//put doc in he queue
// 				qput(new_word->doc_q,new_doc);

// 				//put wordDoc in the hashtable
// 				hput(words_ht,new_word,result,strlen(result));

// 			} else{ //otherwise if word already exists

// 				//check if the doc already exists
// 				doc_t *doc_exist;
// 				doc_exist = (doc_t*)qsearch(word_exist->doc_q,docExist,&id);

// 				//if no such document, put doc in the queue with count one
// 				if(doc_exist == NULL){ // if the doc is not in the queue
// 					doc_t* new_doc = (doc_t*)malloc(sizeof(doc_t));
// 					new_doc->document = id; 
// 					new_doc->count = 1;
// 					qput(word_exist->doc_q,new_doc);
// 				} else {// if the doc is in the queue, increase count
// 					doc_exist->count++;
// 				}
// 				free(result);
// 			}
// 		}
// 		else {
// 			free(result);
// 		}
// 	}
// }

// void *threadFunc(void *vargp) {
//     //indexer does its scanning
//     //build to delete webpages
//     while (access(path, R_OK) == 0) {
// 		current = pageload(id,pagedir);
// 		indexBuild(words_ht,id,current);
// 		webpage_delete(current);
// 		id++;
// 		sprintf(path,"%s/%d",pagedir,id);
// 	}
// 	sumwords(words_ht);
// 	indexsave(words_ht,indexnm);
    
//     printf("Total number: ____ \n"); 
//     pthread_exit(0);

//     return NULL; 
// }

// int main(int argc,char *argv[]){

//     if(argc != 4){
//         printf("usage: indexer <pagedir> <indexnm> <threadnum>\n");
//         printf("wrong argument number\n");
// 	    exit(EXIT_FAILURE);
// 	}

//     char* pagedir = argv[1];
// 	char* indexnm = argv[2];
//     int threadnum = argv[3];
// 	char path[50];
// 	webpage_t *current;
// 	int id = 1;
	
// 	if (access(pagedir, F_OK) != 0) {
// 		printf("ERROR: directory not found\n");
// 		exit(EXIT_FAILURE);
// 	}
// 	sprintf(path,"%s/%d",pagedir,id);

//     lhashtable_t *words_ht;
//     lhp = lhopen(100);
//     if ( lhp == NULL ){
//         fail = true;
//         exit(EXIT_FAILURE);
//     }
//     pthread_t thread;
//     for (int i = 0; i < threadnum; i++) {
//         //pass fnc and hash table to pthread_create
//         pthread_create(&thread, NULL, threadFunc, (void *)&thread); 
//     }

// 	//free all the queues in lhashtable
// 	happly(words_ht,freeHash);
//     lhclose(lhp);
//     pthread_exit(NULL); 

//     return 0;
// }