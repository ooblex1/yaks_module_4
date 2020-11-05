  
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
    for(int i=0; word[i]!= '\0'; i++){
        word[i] = tolower(word[i]);
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
    normalizeWord(token);
    if (strcmp(token,"and") == 0 || strcmp(token,"or") == 0){
        printf("first word can not be and/or\n");
        return -1;
    }
    word[0] = token;
    int i = 1;
    token = strtok(NULL," ");
    while (token != NULL) {
        normalizeWord(token); //cant recognize from indexer.c
        word[i] = token;
        if (strcmp(word[i-1],"and") == 0 || strcmp(word[i-1],"or") == 0){
            if (strcmp(word[i],"and") == 0 || strcmp(word[i],"or") == 0){
                printf("can't have consecutive ands /ors\n");
                return -1;
            }
        }
        token = strtok(NULL," ");
        i++;

    }

    if (strcmp(word[i-1],"and") == 0 || strcmp(word[i-1],"or") == 0){
        printf("last word can not be and/or\n");
        return -1;
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


//search queue of doc_t for the count
bool searchQueueCount(void* elementp, const void* keyp){
    if (elementp == NULL){
        return NULL;
    }
    int* page_key = (int*)keyp;
    doc_t* current_doc = (doc_t*) elementp;
    return (current_doc->count == *page_key);
}


void printcount(void *elmt) {
    if (elmt != NULL) {
        doc_t *doc_exist = (doc_t*)elmt;
        printf("rank:%d:doc:%d;", doc_exist->count, doc_exist->document);

        //finding url
        char *pagedir = "../crawler/pages3/";

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
queue_t *copyDocQ(queue_t *source, queue_t *destination){
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
    //qconcat(source,backup_q);
    qclose(source);
    //source=backup_q;
    return backup_q;
}

//compare doc and only leaves documents that has overlaps
queue_t *compareDocQ(queue_t *temp, queue_t *wordstruct_q){
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
    //qconcat(temp,backup);
    qclose(temp);
    //temp = backup;
    return(backup);
}


//updates final doc queue with passed doc queue; used for OR operations
void compareDocQOr(queue_t *final_q, queue_t *temp) {
    doc_t *final_doc, *doc_to_add;

    while ((doc_to_add = qget(temp))!=NULL) {
        final_doc = qsearch(final_q,searchQueue,doc_to_add);
        if (final_doc == NULL) {
            qput(final_q,doc_to_add);
        } else {
            final_doc->count += doc_to_add->count;
            free(doc_to_add);
        }
    }
}


int compareRank(const void * a, const void * b) {
   return ( *(int*)b - *(int*)a );
}

//rank the final queue

void sort(queue_t *final_q){
    int ranks[500];
    doc_t *current;
    queue_t *backup_rank;
    backup_rank = qopen();

    //put all counts in an array
    int i = 0;

    //qapply(final_q, printcount);
    //printf("------------\n");

    while ((current = qget(final_q) )!= NULL){
        ranks[i] = current->count;
        qput(backup_rank,current);
        i++;
    }

    //rank the array
    if (i>0){
        qsort(ranks,i,sizeof(int),compareRank);
        //printf("%d\n", i);
        doc_t *retrived;
        for(int j = 0; j < i; j++){
            int rank_int = ranks[j];
            retrived = qremove(backup_rank,searchQueueCount, &rank_int);
            if (retrived != NULL){
                qput(final_q, retrived);
            }
        }
    }
    qclose(backup_rank);
}




void generateResult(char** words,hashtable_t *ht,int max) {

    //queue of docs
    queue_t *temp_q = NULL;
    queue_t *final_q;
    final_q = qopen();

    //compare following words with things in temp_q to perform and operation
    for(int i=0; i<max;i++) {
        char* current_word = words[i];
        if(strcmp(current_word,"or")==0){
            compareDocQOr(final_q,temp_q);
            qclose(temp_q);
            temp_q = NULL;
        }else if(strcmp(current_word,"and")!=0 && strlen(current_word)>=3 ) {
            wordDoc_t *word_struct = hsearch(ht,search,current_word,strlen(current_word));
            if (word_struct != NULL) { //if the word exists in any document   
                if (temp_q == NULL){
                    temp_q = qopen();
                    word_struct->doc_q = copyDocQ(word_struct->doc_q, temp_q);
                } else {            
                    temp_q = compareDocQ(temp_q, word_struct->doc_q);   
                }          
            } 
            else{
                qclose(temp_q);
                temp_q = NULL;
                int j = i+1;
                while(j<max){
                    if (strcmp(words[j],"or") == 0){
                        break;
                    } 
                    j++;
                }
                i=j;
            }
            
        }

    }

    if (temp_q != NULL){
        compareDocQOr(final_q,temp_q);
        qclose(temp_q);
    }


    sort(final_q);
    qapply(final_q,printcount);
    
    qclose(final_q);

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
    ht = indexload("../queryfiles/query_depth3"); //temporary name
    while((fgets(input,500,stdin)) != NULL) {
        if (!validate(input)) {
            printf("invalid query!\n");
            printf("> ");
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

        if (limit < 0){
            free(words);
            printf("invalid query!\n");
            printf("> ");
            continue;
        }        
        

        generateResult(words,ht,limit);

        free(words);
        printf("> ");
    }

    happly(ht,delete);
    hclose(ht);

    return 0;
}