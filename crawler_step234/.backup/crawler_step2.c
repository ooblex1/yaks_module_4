/* crawler.c --- 
 * 
 * 
 * Author: Ye Zhang, (add yoru names here)
 * Created: Thu Oct 15 16:40:16 2020 (-0400)
 * Version: 1
 * 
 * Description: craws the web form a given seed to a given maxDepth and 
 * catches the content of the pages it finds, one page per file, 
 * in a given directory
 * code for step 2 only
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

//int main(int argc,char *argv[]) {
int main(){

	//create new webpage
	webpage_t *new_page = webpage_new("https://thayer.github.io/engs50/", 0, NULL);

	if(! webpage_fetch(new_page)) {
		exit(EXIT_FAILURE);
	}
    //char *html = webpage_getHTML(new_page);
    //printf("Found html: %s\n", html);
    //free(html); 

    //printing URL
    int pos = 0;
    char *result;
    char *location;
    char *key_word = "engs50";
 
    while ((pos = webpage_getNextURL(new_page, pos, &result)) > 0) {
    	location = "external";
    	if (strstr(result, key_word) != NULL) {
    		location = "internal";
    	}
      	printf("Found %s url: %s\n", location, result);
      	free(result);
    }

	webpage_delete(new_page);

	exit(EXIT_SUCCESS);
	
}