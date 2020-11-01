/* query.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha
 * Created: Thu Oct 29 22:14:18 2020 (-0400)
 * Version: 
 * 
 * Description: Query in search engine
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "webpage.h"
#include "queue.h"
#include "hash.h"
#include "pageio.h"
#include "indexio.h"

int min_count;

void normalizeWord(char *word){
	//check if more than 3 characters
	if(strlen(word)>=3){
        for(int i=0; word[i]!= '\0'; i++){
            word[i] = tolower(word[i]);
        }
    }
}

bool validate(char* line) {
    for(int i=0;i<strlen(line);i++){
        if (isalpha(line[i]) == 0 && isspace(line[i] == 0)) {
            return false;
        }
    }
    return true;
}

int parse(char* line, char** word) {
    char* token;
    token = strtok(line," ");
    int i = 0;
    while (token != NULL) {
        normalizeWord(token); //cant recognize from indexer.c
        word[i] = token;
        token = strtok(NULL," ");
        i++;
    }
    return i;
}

bool search(void *elmt, const void *key) {
	if (elmt == NULL){
		return NULL;
	}
	wordDoc_t* el = (wordDoc_t *)elmt;
	char* key_new = (char *)key;
	return strcmp(el->word,key_new)==0;
}

void printcount(void *elmt) {
    if (elmt != NULL) {
        doc_t *doc_exist = (doc_t*)elmt;
        printf("%d ", doc_exist->count);
        if (doc_exist->count < min_count) {
            min_count = doc_exist->count;
        }
    }
}

void printquery(char** words,hashtable_t *ht,int max) {
    min_count = INT_MAX;

    for(int i=0; i<max;i++) {
        char* current_word = words[i];
        if(strcmp(current_word,"and")!=0) {
            wordDoc_t *word_struct = hsearch(ht,search,current_word,strlen(current_word));
            if (word_struct != NULL) {
                printf("%s: ",word_struct->word);
                qapply(word_struct->doc_q,printcount);
            }
        }
    }
    if (min_count != INT_MAX){
        printf("--%d\n",min_count);
    }
    else{
        printf("no result\n");
    }
}

void delete(void *p){
	if (p != NULL){
		wordDoc_t *current_word = (wordDoc_t*) p;
		free(current_word->word);
		qclose(current_word->doc_q);
	}
}

int main() {
    char input[500];
    char** words;

    printf("> ");
    hashtable_t *ht;
    ht = indexload("../queryfiles/query1"); //temporary name
    while((fgets(input,500,stdin)) != NULL) {
        if (!validate(input)) {
            printf("invalid query!\n");
            continue;
        }
        if (strlen(input)<=1) {
            printf("> ");
            continue;
        }
        input[strlen(input)-1] = '\0';
        int maxwords = strlen(input)/2;
        words = (char**)calloc(maxwords,sizeof(char*));
        int limit = parse(input,words);
        printquery(words,ht,limit);

        free(words);
        printf("> ");
    }

    happly(ht,delete);
    hclose(ht);

    return 0;
}