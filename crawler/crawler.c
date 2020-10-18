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
	char outn[5], *purl, *html;
	FILE *outf;
	int nlen = strlen(dirname) + 16, stat, pdep, plen;
	int32_t result = 1;
	char mdir[nlen], cmod[nlen], remv[nlen], path[nlen];
	
	purl = webpage_getURL(pagep);
	pdep = webpage_getDepth(pagep);
	plen = webpage_getHTMLlen(pagep);
	html = webpage_getHTML(pagep);
	
	sprintf(outn, "%d", id);
	strcpy(mdir, "mkdir ");
	strcat(mdir, dirname);
	strcpy(path, "./");
	strcat(path, dirname);
	strcat(path, "/");
	strcat(path, outn);
	strcpy(cmod, "chmod 700 ");
	strcat(cmod, path);
	strcpy(remv, "rm ");
	strcat(remv, path);
	
	if (access(dirname, F_OK) != 0) {
		stat = (int)WEXITSTATUS(system(mdir));
	} else {
		stat = 0;
	}
	
	if (stat == 0) {
		if ((outf = fopen(path, "w")) != NULL) {
			system(cmod);
			
			if (access(path, W_OK) == 0 && purl != NULL && html != NULL) {
				fprintf(outf, "%s\n", purl);
				fprintf(outf, "%d\n", pdep);
				fprintf(outf, "%d\n", plen);
				fprintf(outf, "%s", html);
				
	 			result = 0;
			} else {
				system(remv);
			}
			
			if (fclose(outf) == EOF) {
				printf("ERROR: Error closing file %s\n", path);
				result = 1;
			}
		} else {
			printf("ERROR: Failed to make file %s\n", path);
		}
	} else {
		printf("ERROR: Failed to make directory \"%s\"\n", dirname);
	}
	
	return result;
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
		if (!NormalizeURL(seedurl) || !isInternalURL(seedurl)) {
			printf("usage: <seedurl> should be normalised and internal\n");
			return EXIT_FAILURE;
		}
		
		if (maxdepth<0) {
			printf("usage: <maxdepth> should be >= 0");
      return EXIT_FAILURE;
		}

		struct stat stats;
    stat(pagedir, &stats);
		if (!S_ISDIR(stats.st_mode)) {
			printf("usage: <pagedir> should exist\n");  
      return EXIT_FAILURE;
		}

		File f = new File(pagedir);
		if(!f.canWrite()) {
			printf("Ensure page writable\n");
			return EXIT_FAILURE;
		}

		int32_t savestat = 1;
		char dirname[10];
		strcpy(dirname, "pages");
				
    //new seed url webpage
    webpage_t *new_page = webpage_new(seedurl, maxdepth, NULL);

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
				if (!webpage_fetch(current)) {
					printf("ERROR: Some error fetching page at position %d\n", pos);
					exit(EXIT_FAILURE);
				}
				savestat = pagesave(current, pos, dirname);
				if (savestat == 1) {
					printf("Failed to save web page at position %d\n", pos);
				} else {
					printf("Successfully saved web page at position %d\n", pos);
				}
				//free(webpage_getHTML(current));
				qput(internal_q,current);
			} else{
				free(result);
			}
		}

		//savestat = pagesave(current, 1, dirname);
	//if (savestat == 1) {
	//	exit(EXIT_FAILURE);
	//}

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
