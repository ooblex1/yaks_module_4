/* index_test.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha
 * Created: Tue Oct 27 12:32:54 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pageio.h"
#include "webpage.h"
#include "indexio.h"

void freeHash(void *p){
	if (p != NULL){
		wordDoc_t *current_word = (wordDoc_t*) p;
		free(current_word->word);
		qclose(current_word->doc_q);
	}
}
int main(void) {
    hashtable_t *result;
	result = indexload("../test/oldindex");
    if (result != NULL){
        indexsave(result,"./newindex");
        happly(result,freeHash);
        hclose(result);
    }   
	exit(EXIT_SUCCESS);
}