/* piotest1.c --- Tests implementation of pageio module for Module 5 Step 1
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Oct 23 17:51:35 2020 (-0400)
 * Version: 1
 * 
 * Description: Tests the pagesave() and pageload() functions
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "hash.h"
#include "pageio.h"
#include "queue.h"
#include "webpage.h"

void deletePagesQ(void *qp) {
	queue_t *queue = (queue_t *)qp;
	webpage_t *current_page;
	while ((current_page = (webpage_t *)qget(queue)) != NULL) {
		webpage_delete(current_page);
	}
}

bool inHash(void *elp, const void *keyp) {
	if (elp == NULL) {
		return NULL;
	}
	char *el = (char *)elp;
	char *key = (char *)keyp;
	return strcmp(el,key) == 0;
}

int main(void) {
	
	webpage_t *new_page = webpage_new("https://thayer.github.io/engs50/", 0, NULL);
	
	if (!webpage_fetch(new_page)) {
		exit(EXIT_FAILURE);
	}
	
	int pos = 0;
	int32_t savestat = 1;
	char *result, old_url[100], new_url[100];
	queue_t *internal_q = qopen();
	hashtable_t *visited_ht = hopen(30);
	webpage_t *current, *original, *retrieved;
	
	while ((pos = webpage_getNextURL(new_page, pos, &result)) > 0) {
		if (IsInternalURL(result) && (hsearch(visited_ht, inHash, result, strlen(result)) == NULL)) {
			hput(visited_ht, result, result, strlen(result));
			current = webpage_new(result, 1, NULL);
			if (pos == 771) {
				original = current;
			}
			if (!webpage_fetch(current)) {
				printf("ERROR: Some error fetching page at position %d\n", pos);
				exit(EXIT_FAILURE);
			}
			savestat = pagesave(current, pos, "pages");
			if (savestat == 1) {
				printf("Failed to save web page at position %d\n", pos);
				exit(EXIT_FAILURE);
			} else {
				printf("Successfully saved web page at position %d\n", pos);
				//if (pos == 771) {
				//original = webpage_new(result, 1, NULL);
				//original = current;
				//}
			}
			qput(internal_q, current);
		} else {
			//retrieved = webpage_new(result, 1, NULL);
			free(result);
		}
	}
	
	retrieved = pageload(771, "pages");

	/*
	if (retrieved != original) {
		printf("FAILURE: They don't match\n");
		exit(EXIT_FAILURE);
	}
	*/
	strcpy(new_url, webpage_getURL(retrieved));
	strcpy(old_url, webpage_getURL(original));			 
	if (strcmp(new_url, old_url) != 0) {
		printf("FAILURE: Retrieved webpage's URL does not match original webpage's URL\n");
		printf("original URL: %s; retrieved URL: %s\n", old_url, new_url);
 		exit(EXIT_FAILURE);
 	}
 	if (webpage_getDepth(retrieved) != webpage_getDepth(original)) {
		printf("FAILURE: Retrieved webpage's depth does not match original webpage's depth\n");
		exit(EXIT_FAILURE);
	}
	if (webpage_getHTMLlen(retrieved) != webpage_getHTMLlen(original)) {
		printf("FAILURE: Retrieved webpage's html length does not match original webpage's html length\n");
		exit(EXIT_FAILURE);
	}
	if (webpage_getHTML(retrieved) != webpage_getHTML(original)) {
		printf("FAILURE: Retrieved webpage's html does not match original webpage's html\n");
		exit(EXIT_FAILURE);
	}
	
	printf("SUCCESS: Retrieved webpage matches original webpage\n");
	
	webpage_delete(new_page);
	webpage_delete(retrieved);
	deletePagesQ(internal_q);

	qclose(internal_q);
	hclose(visited_ht);

	exit(EXIT_SUCCESS);
}
