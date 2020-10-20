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
#include "webpage.h"
#include "queue.h"
#include "hash.h"

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
		printf("usage: directory not found");
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

int main(int argc,char *argv[]) {

	//check arguments
    if(argc != 4){
        printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
        printf("wrong argument number\n");
        exit(EXIT_FAILURE);
    }

		char* seedurl = argv[1];
		char* pagedir = argv[2];
    int maxdepth = atoi(argv[3]);
		
    //no function to check if input is valid yet
		if (!NormalizeURL(seedurl) || !IsInternalURL(seedurl)) {
			printf("usage: <seedurl> should be normalised and internal\n");
			exit(EXIT_FAILURE);
		}
		
		if (maxdepth<0) {
			printf("usage: <maxdepth> should be >= 0");
      		exit(EXIT_FAILURE);
		}

		char* path = (char*)malloc(sizeof(strlen(pagedir))+20);
		strcpy(path,pagedir);
		strcpy(path,".crawler");
		FILE *fp = fopen(path,"w");
		if (fp == NULL) {
			printf("usage: directory not found");
      		exit(EXIT_FAILURE);
		}
		fclose(fp);
		free(path);
				
    //new seed url webpage
    webpage_t *new_page = webpage_new(seedurl, maxdepth, NULL);
    //loop through all pages til maxdepth and add to queue+hash only when url different from hash
    int pos = 0;
    char *result;

    queue_t* internal_q = qopen();
    hashtable_t* visited_ht = hopen(100);

	//use strcpy with mem alloc to copy seedurl
	char* str_seedurl = calloc(strlen(seedurl) + 1, sizeof(char));
	strcpy(str_seedurl,seedurl);
    hput(visited_ht,str_seedurl,str_seedurl,strlen(seedurl));
    qput(internal_q,new_page);
	
	//pop it get webpage <- current and read through it
    webpage_t *current = (webpage_t*)qget(internal_q);
	int32_t depth = webpage_getDepth(current);
	while (depth < maxdepth) {
		while ((pos = webpage_getNextURL(new_page, pos, &result)) > 0) {
			if(IsInternalURL(result) && (hsearch(visited_ht,inHash,result,strlen(result))==NULL)){
				hput(visited_ht,result,result,strlen(result));
				current = webpage_new(result, 1, NULL);
				pagesave(current,pos,pagedir);
				qput(internal_q,current);
				new_page = (webpage_t*)qget(internal_q);
				depth = webpage_getDepth(current);
			} else {
				free(result);
			}
		}
		pos = 0;
	}
//write a void fnc for close hashtable (input pointer) and free that element the pointer is pointing to
    //printing
    printf("printing queue\n");
    qapply(internal_q,printURL);
    printf("printing hash\n");
    happly(visited_ht, printHash);

    // free all the data structure
	webpage_delete(new_page);
	deletePagesQ(internal_q);
	hclose(visited_ht);
	qclose(internal_q);

    exit(EXIT_SUCCESS);
}
