/* crawler.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Thu Oct 15 16:40:16 2020 (-0400)
 * Version: 1
 * 
 * Description: craws the web form a given seed to a given maxDepth and 
 * catches the content of the pages it finds, one page per file, 
 * in a given directory
 * 
 * code for step 3: THIS CODE IS NOT TESTED, COMPILES, BUT QUEUE IS NOT PRINTED
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

//print the URL for a page
void printURL(void *p) {
	webpage_t *wpp = (webpage_t *)p;
	char *url = webpage_getURL(wpp);
	
	if (url != NULL) {
		printf("%s\n", url);
	} else{
		printf("url is empty");
	}

}

// helper function used to delete queue content
void deletePages(void* qp){
    queue_t* queue = (queue_t*)qp;
    webpage_t *current_page;
    while ( ( current_page = (webpage_t*)qget(queue) )!=NULL ){
        webpage_delete(current_page);
    }
}

//int main(int argc,char *argv[]) {
int main(){

	//create new webpage
	webpage_t *new_page = webpage_new("https://thayer.github.io/engs50/", 0, NULL);

	if(! webpage_fetch(new_page)) {
		exit(EXIT_FAILURE);
	}


    queue_t* internal_q = qopen();

    int pos = 0;
    char *result;
    char *location;

    webpage_t *current;
    while ((pos = webpage_getNextURL(new_page, pos, &result)) > 0) {
    	location = "external";
    	if (IsInternalURL(result)) {
    		location = "internal";
    		current = webpage_new(result, 1, NULL);
    		qput(internal_q,(void*)current);
    	}
      	printf("Found %s url: %s\n", location, result);
      	free(result);
      	//webpage_delete(new_page);
    }


    //printing
    qapply(internal_q, printURL);


	// free all the data structure
	webpage_delete(new_page);

    deletePages(internal_q);

	qclose(internal_q);

	exit(EXIT_SUCCESS);
	
}