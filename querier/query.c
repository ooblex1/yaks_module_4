  
/* query.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha Ye Zhang
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

/*
bool isFirstLastCorrect(char** word, int limit){
    char* first_word = word[1];

    //check first word
    if (strcmp(first_word,"and")==0 ||strcmp(first_word,"or")==0){
        return false;
    }

    //check last word
    char* last_word = word[limit-1];
    if (strcmp(last_word,"and")==0 ||strcmp(last_word,"or")==0){
        return false;
    }

    return true;
}
*/

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
        printf("rank:%d:doc:%d;", doc_exist->count, doc_exist->document);

        //finding url
        char *pagedir = "../crawler/pages1/";

        FILE *fp;
        char url[100];
        char page_id[10];
        char path[20];

        int id = doc_exist->document;
        sprintf(page_id,"%d",id);
        strcpy(path,pagedir);
        strcat(path,page_id);
        fp = fopen(path,"r");
        fscanf(fp,"%s\n",url);

        printf("url:%s\n", url);

        fclose(fp);
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


int compareRank(const void * a, const void * b) {
   return ( *(int*)b - *(int*)a );
}

//rank the final queue
/*
void rankfinal(queue_t *final_q){
    int ranks[500];
    doc_t *current;
    queue_t *backup_rank;

    //put all counts in an array
    int i = 0;
    while ((current = qget(final_q) )!= NULL){
        ranks[i] = current->count;
        qput(backup_rank,current);
        i++;
    }

    //rank the array
    qsort(ranks,i,sizeof(int),compareRank);

    //debug printing code
    
    int loop;

    for(loop = 0; loop < 10; loop++){
      printf("%d ", ranks[loop]);
    }
    

    int j = 0;
    for(j = 0; j < 10; j++){
      printf("%d ", ranks[loop]);
    }

    


    qclose(backup_rank);


}
*/



void generateResult(char** words,hashtable_t *ht,int max) {

    //queue of docs
    queue_t *temp_q;

    //copy the first word in temp_q
    char *first_word = words[0];
    temp_q = qopen();
    wordDoc_t *word_struct_1 = hsearch(ht,search,first_word,strlen(first_word));

    copyDocQ(word_struct_1->doc_q, temp_q);

    //compare following words with things in temp_q to perform and operation
    for(int i=1; i<max;i++) {
        char* current_word = words[i];
        if(strcmp(current_word,"and")!=0 && strlen(current_word)>=3 ) {
            wordDoc_t *word_struct = hsearch(ht,search,current_word,strlen(current_word));
            if (word_struct != NULL) { //if the word exists in any document                
                compareDocQ(temp_q, word_struct->doc_q);             
            }
        }
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

int main() {
    char input[500];
    char** words;
    //queue_t *result_final;  //queue of docs for result, step 4
    hashtable_t *ht;  //for index
    printf("> ");
    ht = indexload("../queryfiles/query_depth1"); //temporary name
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

        /*
        if (!isFirstLastCorrect(words, limit)){
            printf("first and last word can not be and/or");
            continue;
        }
        */
        
        

        generateResult(words,ht,limit);

        free(words);
        printf("> ");
    }

    happly(ht,delete);
    hclose(ht);

    return 0;
}