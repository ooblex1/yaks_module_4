/* crawler.c --- 
 * 
 * 
 * Author: Ye Zhang, (add your names here)
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
#include <unistd.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"


//Notes from Ye: I took what's probablt useufl from step 234 and pasted here. 
//The main function currently doesn't include any page save and maxstep at all, and I have never compiled this

//print url given pointer to a webpage, delete if not needed
void printURL(void *p) {
    webpage_t *wpp = (webpage_t *)p;
    char *url = webpage_getURL(wpp);
    
    if (url != NULL) {
        printf("%s\n", url);
    } else{
        printf("url is empty");
    }

}

//print urls in the hash table, delete if not needed
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


int main(int argc,char *argv[]) {

	//check arguments
    if(argc != 4){
        printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
        printf("wrong argument number\n");
        exit(EXIT_FAILURE);
    }

    //no function to check if input is valid yet
		if (!NormalizeURL(argc[2]) || !isInternalURL(argc[2])) {                                           
			printf("usage: <seedurl> should be normalised and internal\n");                                  
			return EXIT_FAILURE;                                                                             
		}                                                                                                  
                                                                                                     
		if (arc[4]<0) {                                                                                    
			printf("usage: <maxdepth> should be >= 0");                                                      
		}
    
    char* seedurl = argv[1];
    char* pagedir = argv[2];
    int maxdepth = atoi(argv[3]);

    //new seed url webpage
    webpage_t *new_page = webpage_new(seedurl, maxdepth, NULL);

    //new queue and hash
    queue_t* internal_q = qopen();
    hashtable_t* visited_ht = hopen(100);

    // put seedurl in queue and hashtable
    if (webpage_fetch(new_page)){
        hput(visited_ht,(void*)seedurl,seedurl,strlen(seedurl));
        qput(internal_q,(void*)new_page);

    } else {
        webpage_delete(new_page);
        qclose(internal_q);
        hclose(visited_ht);
        exit(EXIT_FAILURE);
    }


    //loop through all pages til maxdepth and add to queue+hash only when url different from hash
    //below is the code that doesn not consider maxdepth, but hopefully does the rest of job
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
