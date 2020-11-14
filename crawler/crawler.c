/* crawler.c --- 
 * 
 * 
 * Author: Ye Zhang, Kevin Larkin, Aadhya Kocha
 * Created: Thu Oct 15 16:40:16 2020 (-0400)
 * Version: 1
 * 
 * Description: craws the web form a given seed to a given maxDepth and 
 * catches the content of the pages it finds, one page per file, 
 * in a given directory
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "lhash.h"
#include "lqueue.h"
#include "library.c"

//Saves the passed webpage in a file with the passed id as its name, in a directory with the passed name
int32_t pagesave(webpage_t *pagep, int id, char *dirname) {
	if (pagep == NULL || id < 0 || dirname == NULL) {
		printf("pagep, id or dirname is null! \n");
		return 1;
	}
	char* path = (char*)malloc(sizeof(strlen(dirname))+20);
	sprintf(path,"%s/%d",dirname,id); //printing into file

	FILE *fp = fopen(path,"w");
	if (fp == NULL) {
		printf("usage: directory not found\n");
		return 2;
	}
	fprintf(fp,"%s\n%d\n%d\n%s\n",webpage_getURL(pagep),webpage_getDepth(pagep),webpage_getHTMLlen(pagep),webpage_getHTML(pagep));
	fclose(fp);
	free(path);
	return 0;
}

void printURL(void *p) {
	webpage_t *wpp = (webpage_t *)p;
	char *url = webpage_getURL(wpp);
	
	if (url != NULL) {
		printf("%s\n", url);
	} else{
		printf("url is empty");
	}
}

void printHash(void *p){
	char* urlp = (char *)p;
	if (urlp != NULL) {
		printf("%s\n", urlp);
	} 
}

// helper function used to delete queue content
void deletePagesQ(void* qp){
    queue_t* queue = (queue_t*)qp;
    webpage_t *current_page;
    while ( ( current_page = (webpage_t*)qget(queue) )!=NULL ){
        webpage_delete(current_page);
    }
}

//used in h search, whether url is in the hash table
bool inHash(void* elp, const void* keyp){
	if (elp == NULL){
		return NULL;
	}
	char* el = (char *)elp;
	char* key = (char *)keyp;
	return strcmp(el,key)==0;
}

void *threadFunc(void *total) {
	struct TotalCrawled *totalArg = (struct TotalCrawled *)total;

    //pop it get webpage <- current and read through it
    webpage_t *current;
	char *result;
	int id = 1;
	while ((current = lqget(totalArg->lqp)) != NULL) {
		printf("getting a webpage \n");
		int depth = webpage_getDepth(current);
		if (webpage_fetch(current)) {
			pagesave(current,id,totalArg->pagedir);
			id++;

			if (depth < totalArg->maxdepth){

				int pos = 0;
				while ((pos = webpage_getNextURL(current, pos, &result)) > 0) {
					if(IsInternalURL(result) && (lhsearch(totalArg->lht,inHash,result,strlen(result))==NULL)){
						lhput(totalArg->lht,result,result,strlen(result));				
						webpage_t *new_page = webpage_new(result, depth+1, NULL);
						lqput(totalArg->lqp,new_page);
					} else {
						free(result);
					}
				}
			}
		}
		webpage_delete(current);
	}

    pthread_exit(NULL);
	free(total);
    return NULL; 
}

int main(int argc,char *argv[]) {

	//check arguments
    if(argc != 5){
        printf("usage: crawler <seedurl> <pagedir> <maxdepth> <threadnm>\n");
        printf("wrong argument number\n");
        exit(EXIT_FAILURE);
    }
	char* seedurl = argv[1];
	char* pagedir = argv[2];
    int maxdepth = atoi(argv[3]);
	int threadnm = atoi(argv[4]);
		
    //no function to check if input is valid yet
	if (!NormalizeURL(seedurl) || !IsInternalURL(seedurl)) {
		printf("usage: <seedurl> should be normalised and internal\n");
		exit(EXIT_FAILURE);
	}
	
	if (maxdepth<0) {
		printf("usage: <maxdepth> should be >= 0");
		exit(EXIT_FAILURE);
	}

	if (threadnm<0) {
		printf("usage: <threadnum> should be >= 0");
		exit(EXIT_FAILURE);
	}

    //new seed url webpage
    webpage_t *new_page = webpage_new(seedurl, 0, NULL);

    //loop through all pages til maxdepth and add to queue+hash only when url different from hash
	lqueue_t *internal_q = lqopen();
	lhashtable_t *visited_ht = lhopen(100);
    if ( visited_ht == NULL ){
        exit(EXIT_FAILURE);
    }

	//use strcpy with mem alloc to copy seedurl
	char* str_seedurl = calloc(strlen(seedurl) + 1, sizeof(char));
	strcpy(str_seedurl,seedurl);
    lhput(visited_ht,str_seedurl,str_seedurl,strlen(seedurl));
    lqput(internal_q,new_page);

	//initialise structure
	struct TotalCrawled *total = malloc(sizeof(struct TotalCrawled));
	total->lht = visited_ht;
	total->lqp = internal_q;
	total->pagedir = pagedir;
	total->seedurl = seedurl;
	total->maxdepth = maxdepth;
	total->threadnm = threadnm;

	pthread_t thread[threadnm];
    for (int i = 0; i < threadnm; i++) {
        pthread_create(&thread[i], NULL, threadFunc, (void *)total);
    }
	for (int i = 0; i < threadnm; i++) {
		pthread_join(thread[i],NULL);
	}

	// free data structure
	lhclose(visited_ht);
	lqclose(internal_q);
	free(total);
	pthread_exit(NULL);

    exit(EXIT_SUCCESS);
}


/*
void *threadFunc(void *total) {
	struct TotalCrawled *totalArg = (struct TotalCrawled *)total;

    //pop it get webpage <- current and read through it
    webpage_t *current;
	int id = 1;
	while ((current = qget(internal_q)) != NULL) {
		printf("getting a webpage \n");
		int depth = webpage_getDepth(current);
		if (webpage_fetch(current)) {
			pagesave(current,id,pagedir);
			id++;

			if (depth < maxdepth){

				int pos = 0;
				while ((pos = webpage_getNextURL(current, pos, &result)) > 0) {
					if(IsInternalURL(result) && (hsearch(visited_ht,inHash,result,strlen(result))==NULL)){
						hput(visited_ht,result,result,strlen(result));				
						new_page = webpage_new(result, depth+1, NULL);
						qput(internal_q,new_page);
					} else {
						free(result);
					}
				}
			}
		}
		webpage_delete(current);
	}

    pthread_exit(NULL);
    return NULL; 
}
*/