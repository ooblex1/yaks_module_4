/* crawler.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Thu Oct 15 16:40:16 2020 (-0400)
 * Version: 2
 * 
 * Description: craws the web form a given seed to a given maxDepth and 
 * catches the content of the pages it finds, one page per file, 
 * in a given directory
 * code for step 3
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"

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

int main(){

	//create new webpage
	webpage_t *new_page = webpage_new("https://thayer.github.io/engs50/", 0, NULL);

	if(! webpage_fetch(new_page)) {
		exit(EXIT_FAILURE);
	}

    int pos = 0;
    char *result;

    queue_t* internal_q = qopen();
    hashtable_t* visited_ht = hopen(30);

    webpage_t *current;
    while ((pos = webpage_getNextURL(new_page, pos, &result)) > 0) {
    	if(IsInternalURL(result) && (hsearch(visited_ht,inHash,result,strlen(result))==NULL)){
    		hput(visited_ht,result,result,strlen(result));
    		current = webpage_new(result, 1, NULL);
    		qput(internal_q,current);
    	} else{
    		free(result);
    	}
    }

    //printing
    printf("printing queue\n");
    qapply(internal_q,printURL);

    printf("printing hash\n");
    happly(visited_ht, printHash);


	// free all the data structure
	webpage_delete(new_page);
	deletePagesQ(internal_q);

	qclose(internal_q);
	hclose(visited_ht);

	exit(EXIT_SUCCESS);
	
}