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

	/* check arguments
	if(argc != 4){
		printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
		exit(EXIT_FAILURE);
	}
	
	char* seedurl = argv[1];
	char* pagedir = argv[2];
	int maxdepth = atoi(argv[3]);
	*/

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
    }

	free(result);
    free(location);
    free(key_word);

	// free all the data structure
	webpage_delete(new_page);

	exit(EXIT_SUCCESS);
	
}