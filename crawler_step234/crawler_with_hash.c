/* crawler.c --- 
 * 
 * 
 * Author: Ye Zhang, Kevin Larkin, Aadhya Kocha
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

int main(){

	//create new webpage
	webpage_t *new_page = webpage_new("https://thayer.github.io/engs50/", 0, NULL);

	if(! webpage_fetch(new_page)) {
		exit(EXIT_FAILURE);
	}

    int pos = 0;
    char *result;
    char *location;
    webpage_t *current;

    queue_t* internal_q = qopen();
    
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


	qclose(internal_q);

	exit(EXIT_SUCCESS);
	
}