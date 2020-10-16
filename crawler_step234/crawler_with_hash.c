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


//desice whether the given url is internal
bool isInternal((char* url char*seedurl){
	if (strstr(url, seedurl) != NULL) {
    	return true;
    }
    return false;
}


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
	char *seedurl = "https://thayer.github.io/engs50/";
	int maxdepth = 0;
	webpage_t *new_page = webpage_new(seedurl, maxdepth, NULL);

	
    queue_t* internal_q = qopen();
	hashtable_t* visited_url_ht = hopen(100);

	// put seedurl in queue and hashtable
	if (webpage_fetch(new_page)){
		hput(visited_url_ht,(void*)seedurl,seedurl,strlen(seedurl));
		//qput(internal_q,(void*)new_page);
		//saving to dir here?
	} else {
		webpage_delete(new_page);
		//qclose(internal_q);
		hclose(visited_url_ht);
		exit(EXIT_FAILURE);
	}

	//loop through all URL's to put them into hash and queue
	//only when webpage is not in queue or hash will it be put into both
	int pos = 0;
	char* result;

	while (webpage_getDepth(next)<maxdepth && (pos = webpage_getNextURL(new_page, pos, &result)) > 0) {

	}

	webpage_t *next;
	while ( ( next = (webpage_t*)qget(qp) )!=NULL ){

		while (webpage_getDepth(next)<maxdepth && (pos = webpage_getNextURL(next, pos, &result)) > 0) {

			printf("Found url: %s  \n", result);
			if (IsInternalURL(result)){
				
				printf("Internal URL.\n");
				
				if(hsearch(visited_ht,searchurl,result,strlen(result))==NULL){
					webpage_t* inter_web = webpage_new(result, webpage_getDepth(next)+1, NULL);
					hput(visited_ht,(void*)result,result,strlen(result));
					qput(qp,(void*)inter_web);
				} else
					free(result);
			} else {
				printf("External URL.\n");
				free(result);
			}
		}
		webpage_delete((void*)next);
	}


	free(result);
    free(location);
    free(key_word);

	// free all the data structure
	webpage_delete(new_page);

	//hremove(visited_ht,searchurl,seedurl,strlen(seedurl));
	hclose(visited_ht);
	//qclose(internal_q);

	exit(EXIT_SUCCESS);
	
}