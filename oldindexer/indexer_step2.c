/* indexer.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Sat Oct 24 13:49:20 2020 (-0400)
 * Version: 
 * 
 * Description: reads the html associated with every webpage fetched by the crawler
 * constructs in memory an index data structure that can be used to look up a word and find out 1) which document contains the work and 2) how many times the word occur in that document
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <ctype.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"
#include "indexio.h"

/* normalize word： at least 3 characters and lower case. 
*/ 
char *normalizeWord(char *word){

	//check if more than 3 characters
	if(strlen(word)<3){
		return NULL;
	}

	for(int i=0; word[i]!= '\0'; i++){
		//if not a character then return null
		if(!isalpha(word[1])){
			return NULL;
		}

		word[i] = tolower(word[i]);
	}
	return word;
}

int main(){

	char* pagedir = "../crawler/pages";
	webpage_t *current;
	current = pageload(1,pagedir);

	char* result;
	int pos = 0;

	//STEP1 Code
	while ( (pos = webpage_getNextWord(current, pos, &result)) > 0) {

		//normalize all words and then print
		if(normalizeWord(result)!=NULL){
			printf("normalized: %s\n", result);
		}

		free(result);
	}

	webpage_delete(current);



}