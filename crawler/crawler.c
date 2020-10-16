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
//The main function currently doesn't include any page save at all, and I have never compiled this

int main(int argc,char *argv[]) {

	//check arguments
    if(argc != 4){
        printf("usage: crawler <seedurl> <pagedir> <maxdepth>\n");
        printf("wrong argument number\n");
        exit(EXIT_FAILURE);
    }

    //no function to check if input is valid yet

    
    char* seedurl = argv[1];
    char* pagedir = argv[2];
    int maxdepth = atoi(argv[3]);

    //new seed url webpage
    webpage_t *new_page = webpage_new(seedurl, maxdepth, NULL);

    //new queue and hash
    queue_t* internal_q = qopen();
    hashtable_t* visited_url_ht = hopen(100);

    // put seedurl in queue and hashtable
    if (webpage_fetch(new_page)){
        hput(visited_url_ht,(void*)seedurl,seedurl,strlen(seedurl));
        qput(internal_q,(void*)new_page);

    } else {
        webpage_delete(new_page);
        qclose(internal_q);
        hclose(visited_url_ht);
        exit(EXIT_FAILURE);
    }


    //loop through all pages til maxdepth and a only when url different from hash
    //below is the code that doesn not consider maxdepth, but hopefully does the rest of job




	exit(EXIT_SUCCESS);
	
}