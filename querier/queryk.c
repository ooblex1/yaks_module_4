  
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
    int i = 1;
    word[0] = token;
    while (token != NULL) {
        normalizeWord(token); //cant recognize from indexer.c
        word[i] = token;
        token = strtok(NULL," ");

        if (strcmp(word[i-1],"and") == 0 || strcmp(word[i-1],"or") == 0){
            if (strcmp(word[i],"and") == 0 || strcmp(word[i],"or") == 0){
                printf("can't have consecutive ands /ors\n");
                return -1;
            }
        }


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

void sort(queue_t *final_q){
    int ranks[500];
    doc_t *current;
    queue_t *backup_rank;
    backup_rank = qopen();

    //put all counts in an array
    int i = 0;
    while ((current = qget(final_q) )!= NULL){
        ranks[i] = current->count;
        qput(backup_rank,current);
        i++;
    }

    //rank the array
    
    if (i>0){
        qsort(ranks,i,sizeof(int),compareRank);

        doc_t *retrived;
        for(int j = 0; j < i; j++){
            retrived = qremove(backup_rank,searchQueueCount,(void *)&ranks[j]);
            if (retrived != NULL){
                qput(final_q, retrived);
            }
        }
    }
    
    
    qclose(backup_rank);

}




void generateResult(char** words,hashtable_t *ht,int max) {

    //queue of docs
    queue_t *temp_q;

    //copy the first word in temp_q
    char *first_word = words[0];
    temp_q = qopen();
    wordDoc_t *word_struct_1 = hsearch(ht,search,first_word,strlen(first_word));
    if (word_struct_1 == NULL) {
        printf("the word doesn't exist!\n");
        qclose(temp_q);
        return;
    }

    copyDocQ(word_struct_1->doc_q, temp_q);

    //compare following words with things in temp_q to perform and operation
    for(int i=1; i<max;i++) {
        char* current_word = words[i];
        if(strcmp(current_word,"and")!=0 && strlen(current_word)>=3 ) {
            wordDoc_t *word_struct = hsearch(ht,search,current_word,strlen(current_word));
            if (word_struct != NULL) { //if the word exists in any document                
                compareDocQ(temp_q, word_struct->doc_q);             
            } else{
                printf("the word doesn't exist!\n");
                qclose(temp_q);
                return;
            }
        }
    }

    sort(temp_q);
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

int parseArgs(int argc, char *argv[]) {
	int result=0;
	FILE *fp;
	
	switch (argc) {
	case 3:
		result = 1; //return 1 indicates user-generated query mode
		break;
	case 5:
		if (strcmp(argv[3],"-q")!=0 || (fp=fopen(argv[4], "r"))==0 || access(argv[4], R_OK)!=0) {
			result = 0;
		} else {
			result = 2; //return 2 indicates file-provided query mode
		}
		fclose(fp);
		break;
	default:
		result = 0; //return 0 indicates invalid usage
		break;
	}

	if (result!=0) {
		//check argv[1]
		//directory is accessible
		if (access(argv[1], F_OK)!=0) {
			result = 0;
			printf("ERROR: Directory is not accessible or does not exist\n");
		}
		
		//directory has been crawled
		//???
		
		//check argv[2]
		//index file is readable
		if ((fp = fopen(argv[2], "r"))!=NULL) {
			if (access(argv[2], R_OK)!=0) {
				result=0;
				printf("ERROR: Index file is not accessible\n");
			}
		} else {
			result=0;
			printf("ERROR: Index file is not readable or does not exist\n");
		}
		
		fclose(fp);
	}
	
	return result;
}

int main(int argc, char *argv[]) {
	int mode, i=0;
	char input[500], pagedir[50], indexfile[50], queryfile[50], *qc;
	char** words;
	//queue_t *result_final;  //queue of docs for result, step 4
	hashtable_t *ht;  //for index
	FILE *ifp, *qfp;

	strcpy(qc,"a");
	
	mode = parseArgs(argc, argv);
	pagedir=argv[1];
	indexfile=argv[2];
	
	switch (mode) {
	case 0:
		printf("usage: query <pageDirectory> <indexFile> [-q] [<queryFile>]\n");
		exit(EXIT_FAILURE);
		break;
	case 1:
		ifp=fopen(indexfile, "r");
		
		break;
	case 2:
		ifp=fopen(indexfile, "r");
		queryfile=argv[4];
		qfp=fopen(queryfile, "r");
		while (qc!=EOF) {
			while (strcmp(qc,'\n')!=0 ||) {
				qc=fgetc(qfp);
				input[i]=qc;
				i++;
			}
			evaluate(input);
			i=0;
		}
		fclose(qfp);
		break;
	default:
		printf("unknown error\n");
		exit(EXIT_FAILURE);
		break;
	}
	
	printf("> ");
	ht = indexload("../queryfiles/query_depth1"); //temporary name
	while((fgets(input,500,stdin)) != NULL) {
		if (!validate(input)) {
			printf("invalid query! usage: query <pageDirectory> <indexFile> [-q] [<queryFile>]\n");
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
