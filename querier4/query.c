  
/* query.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha Ye Zhang Kevin Larkin
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

#define MAXTERMS 10

typedef struct intpair {
	int a;
	int b;
} intpair_t;

typedef struct segment {
	char *terms[MAXTERMS];
	int length = 0;
} segment_t;

void normalizeWord(char *word){
    //check if more than 3 characters
	if((strlen(word)>=3) || (strcmp(word,"or")==0)){
        for(int i=0; word[i]!= '\0'; i++){
            word[i] = tolower(word[i]);
        }
    }
}

bool validate(char* line) {
    for(int i=0;i<strlen(line);i++){
			if (isalpha(line[i]) == 0 && isspace(line[i]) == 0) {
            return false;
        }
    }
    return true;
}


bool isFirstLastCorrect(char** word, int limit){
    char* first_word = word[0];
		char* last_word = word[limit-1];
		
    //check first word
    if (strcmp(first_word,"and")==0 || strcmp(first_word,"or")==0){
        return false;
    }

    //check last word
    if (strcmp(last_word,"and")==0 || strcmp(last_word,"or")==0){
        return false;
    }

    return true;
}

bool isOrderCorrect(char **word, int limit){
	int i;
	char *word_a, *word_b;
	
	for (i=0;i<limit;i++) {
		word_a = word[i];
		word_b = word[i+1];
		
		if (strcmp(word_a,"and")==0 || strcmp(word_a,"or")==0){
			if (strcmp(word_b,"and")==0 || strcmp(word_b,"or")==0){
				return false;
			}
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

//processes the words array to create AND-linked segments array, returns length of segments array
int partition(segment_t **segments, char **words, int wlen) {
	int i=0, j=0, k=0;
	
	while(i<wlen) {
		if (strcmp(words[i], "and")!=0) {
			if (strcmp(words[i], "or")==0) {
				j++;
				segments[j].length = k;
				k=0;
			} else {
				strcpy(segments[j].terms[k], words[i]);
				k++;
			}
		}
		i++;
	}
	return j++;
}

bool search(void *elmt, const void *key) {
    if (elmt == NULL){
        return NULL;
    }
    wordDoc_t* el = (wordDoc_t *)elmt;
    char* key_new = (char *)key;
    return strcmp(el->word,key_new)==0;
}

//search queue of doc_t for the document
bool searchQueue(void* elementp, const void* keyp){
    if (elementp == NULL){
        return NULL;
    }
    int* page_key = (int*)keyp;
    doc_t* current_doc = (doc_t*) elementp;
    return (current_doc->document == *page_key);
}

void printcount(void *elmt) {
    if (elmt != NULL) {
        doc_t *doc_exist = (doc_t*)elmt;
        printf("rank:%d:doc:%d\n", doc_exist->count, doc_exist->document);
    }
}

void copyDoc(doc_t *source,doc_t *destination){
    if (source != NULL && destination != NULL){
        destination->document = source->document;
        destination->count = source->count;
    }
}

//copy queue of documents
void copyDocQ(queue_t *source, queue_t *destination){
    queue_t *backup_q;
    backup_q = qopen();
    doc_t *current;
    while ((current = qget(source)) != NULL){
            //copy all docs corresponds to first word to temp
            doc_t *dup=(doc_t*)malloc(sizeof(doc_t));
            copyDoc(current,dup);
            qput(destination, dup);
            qput(backup_q, current);
    }
    qconcat(source,backup_q);
    //qclose(source);
    //source=backup_q;
}

//compare doc and only leaves documents that has overlaps
void compareDocQ(queue_t *temp, queue_t *wordstruct_q){
	doc_t *retrived;
	queue_t *backup;
	backup=qopen();
	doc_t *current;
	while ((current = qget(temp) )!= NULL){
		retrived = qsearch(wordstruct_q,searchQueue,current);
		if (retrived == NULL){
			free(current);
		}else{
			if (current->count > retrived->count){
				current->count = retrived->count;
			}
			qput(backup, current);
		}
	}
	qconcat(temp,backup);
	//qclose(temp);
	//temp = backup;
}

void compareDocQOr(queue_t *temp, queue_t *wordstruct_q) {
	doc_t *retrived;
	queue_t *backup;
	backup=qopen();
	doc_t *current;
	while ((current = qget(temp) )!= NULL){
		retrived = qsearch(wordstruct_q,searchQueue,current);
		if (retrived == NULL){
			free(current);
		}else{
			if (current->count > retrived->count){
				current->count = retrived->count;
			}
			qput(backup, current);
		}
	}
	qconcat(temp,backup);
}

/*
//rank the final queue
void rankfinal(queue_t *final_q){
    int ranks[500];
    doc_t *current;

    //put all counts in an array
    i = 0;
    while ((current = qget(final_q) )!= NULL){
        ranks[i] = current->count;
        i++;
    }

    //rank the array

}
*/

void orRanks(queue_t *final_q, segment_t *segments, int segslength) {
	int ranks[500], i=0;
	doc_t *current;
	queue_t *temp_q=qopen();
	
	while ((current=qget(final_q))!=NULL) {
		ranks[i] = current->count;
		i++;
	}

	
	
}

void generateResult(char** words,hashtable_t *ht,int max,bool hasor) {
	
	segment_t *segments;
	int segslength;
	
	//queue of docs
	queue_t *temp_q;
	
	//copy the first word in temp_q
	char *first_word = words[0];
	temp_q = qopen();
	wordDoc_t *word_struct_1 = hsearch(ht,search,first_word,strlen(first_word));
	
	copyDocQ(word_struct_1->doc_q, temp_q);
	
	//an array of indexes of the words array that corresponds to the "or"s

	//do Ye's thing from first word through the last word that comes before the first "or"
	//then add the resulting rank to the rank sum for each doc_t (in each wordDoc_t?)
	//repeat the previous two lines until there are no more words left
	
	//make a new temp queue for each ANDsequence, or don't remove the nonoverlapping docs in compareDocQ (???)
	//or maybe I can make one big temp queue for the query, and 
	
	//compare following words with things in temp_q to perform and operation
	for(int i=1; i<max;i++) {
		char* current_word = words[i];
		if(strcmp(current_word,"and")!=0 && strlen(current_word)>=3) {
			if (strcmp(current_word,"or")!=0) {			
				wordDoc_t *word_struct = hsearch(ht,search,current_word,strlen(current_word));
				if (word_struct != NULL) { //if the word exists in any document                
					compareDocQ(temp_q, word_struct->doc_q);             
				}
			} else {
				
			}
		}
	}
	
	if (hasor) {
		segments = (segment_t *)calloc(maxwords,sizeof(segment_t));
		segslength = partition(segments,words,limit);
		orRanks(temp_q, segments, segslength);
	}
	
	//rankfinal(temp_q);
	qapply(temp_q,printcount);
	qclose(temp_q);
	
}

void delete(void *p){
    if (p != NULL){
        wordDoc_t *current_word = (wordDoc_t*) p;
        free(current_word->word);
        qclose(current_word->doc_q);
    }
}

int main(void) {
	char input[500];
	char** words;
	//queue_t *result_final;  //queue of docs for result, step 4
	hashtable_t *ht;  //for index
	segment_t *segments;
	int maxwords, limit, segslength, i=0;
	bool hasor=false;
	printf("> ");
	ht = indexload("../queryfiles/query_depth1"); //temporary name
	while((fgets(input,500,stdin)) != NULL) {

		//parse input
		input[strlen(input)-1] = '\0';
		maxwords = strlen(input)/2;
		words = (char**)calloc(maxwords,sizeof(char*));
		limit = parse(input,words);

		//flag as "or" query if this is the case
		while (i<limit) {
			if (strcmp(words[i], "or")==0) {
				hasor = true;
				break;
			}
			i++;
		}
		
		//a series of input checks
		if (!validate(input)) {
			printf("invalid query! only letters and spaces allowed\n");
			continue;
		}
		if (!isFirstLastCorrect(words, limit)){
			printf("first and last word must not be and/or\n");
			continue;
		}
		if (!isOrderCorrect(words, limit)){
			printf("ands/ors must not be next to one another\n");
			continue;
		}
		if (strlen(input)<=1) {
			printf("> ");
			continue;
		}

		//handle "or" queries
		if (hasor) {
			segments = (segment_t *)calloc(maxwords,sizeof(segment_t));
			segslength = partition(segments,words,limit);
			while (j<segslength) {
				rank+=segments[j].rank;
				j++;
			}
		} else {
			generateResult(words,ht,limit,hasor);
		}
		
		free(words);
		printf("> ");
	}
	
	happly(ht,delete);
	hclose(ht);
	
	return 0;
}
