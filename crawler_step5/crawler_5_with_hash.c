/* crawler.c --- 
 * 
 * 
 * Author: Ye Zhang, Kevin Larkin
 * Created: Sat Oct 17 12:36:16 2020 (-0400)
 * Version: 3
 * 
 * Description: crawls the web from a given seed to a given maxDepth ***and 
 * caches the content of the pages it finds, one page per file, in a given
 * directory*** (actually does what is in asterisks now)
 * (code for step 5)
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
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

int main(){
	int32_t savestat = 1;
	char dirname[10];
	strcpy(dirname, "pages");
	
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
