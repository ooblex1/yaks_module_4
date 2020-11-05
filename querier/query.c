  
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

//global output and pagedir variables
static FILE *out;
static char pagedir[50];

void normalizeWord(char *word){
    //check if more than 3 characters
    for(int i=0; word[i]!= '\0'; i++){
        word[i] = tolower(word[i]);
    }
}

bool validate(char* line) {
	printf("yo\n");
    for(int i=0;i<strlen(line);i++){
			if (line[i] == '\t'){
				line[i] = ' ';
			}
			if (isalpha(line[i]) == 0 && isspace(line[i]) == 0){
				printf("not alpha\n");
				return false;
        }
    }
    return true;
}


int parse(char* line, char** word) {
    char* token;
    token = strtok(line," ");
		if (token==NULL) {
			return -1;
		}
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

//prints rank & other output for passed document to appropriate output destination
void fprintfcount(void *elmt) {
	if (elmt!=NULL) {
		doc_t *doc_exist = (doc_t*)elmt;
		fprintf(out, "rank:%d:doc:%d;", doc_exist->count, doc_exist->document);

		FILE *fp;
		char url[100];
		char page_id[10];
		char path[20];

		int id = doc_exist->document;
		sprintf(page_id,"%d",id);
		strcpy(path,pagedir);
		strcat(path,"/");
		strcat(path,page_id);

		fp = fopen(path,"r");
		if (fp==NULL) {
			printf("ERROR: Page is unreadeable\n");
			exit(EXIT_FAILURE);
		}
		fscanf(fp,"%s\n",url);

		fprintf(out, "url:%s\n", url);
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
    qclose(source);
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
	/*
	if ((test=(doc_t *)qget(backup))==NULL) {
		fprintf(out,"No query terms found!\n");
	} else {
		qput(backup,(void *)test);
	}
	*/
	qclose(temp);
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

    //queues of docs
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
		qapply(final_q,fprintfcount);
		
    qclose(final_q);
}

void delete(void *p){
    if (p != NULL){
        wordDoc_t *current_word = (wordDoc_t*) p;
        free(current_word->word);
        qclose(current_word->doc_q);
    }
}

void usage(void) {
	printf("usage: query <pageDirectory> <indexFile> [-q] [< <queriesFile>] [> <myoutput>]\n");
}

int main(int argc, char *argv[]) {
	char input[500], page1[50], indexfile[50];
	char** words;
	hashtable_t *ht;  //for index
	FILE *p1p, *ip, *in=stdin;
	bool quiet=false;
	out=stdout;

	//argument checks & processing
	if (argc!=3 && argc!=6) {
		usage();
		exit(EXIT_FAILURE);
	}
	strcpy(pagedir,argv[1]);
	strcpy(page1,pagedir);
	strcat(page1,"/1");
	strcpy(indexfile,argv[2]);
	if (access(pagedir, F_OK)!=0) {
		printf("ERROR: Directory inaccessible\n");
		exit(EXIT_FAILURE);
	}
	if ((p1p=fopen(page1, "r"))==NULL) {
		printf("ERROR: Directory has not been crawled, invalid pages directory, and/or unreadable file\n");
		exit(EXIT_FAILURE);
	}
	fclose(p1p);
	if ((ip=fopen(indexfile, "r"))==NULL) {
		printf("ERROR: Index file not writeable or does not exist\n");
		exit(EXIT_FAILURE);
	}
	fclose(ip);
	if (argc>3) {
		if (strcmp(argv[3],"-q")==0) {
			quiet=true;
			in = fopen(argv[4], "r");
			out = fopen(argv[5], "w");
			if (in==NULL || out==NULL) {
				printf("ERROR: Failed to open queriesFile to read and/or myoutput to write\n");
				exit(EXIT_FAILURE);
			}
		} else {
			usage();
			exit(EXIT_FAILURE);
		}
	}

	//main body
	fprintf(out,"> ");
	ht = indexload(indexfile);
	while((fgets(input,500,in)) != NULL) {
		if (!validate(input)) {
			fprintf(out,"invalid query!\n");
			fprintf(out,"> ");
			continue;
		}
		if (strlen(input)<=1) {
			fprintf(out,"invalid query!");
			fprintf(out,"> ");
			continue;
		}
		input[strlen(input)-1] = '\0';
		int maxwords = strlen(input)/2;
		words = (char**)calloc(maxwords,sizeof(char*));
		int limit = parse(input,words);
		if (limit < 0){
			free(words);
			fprintf(out,"invalid query!\n");
			fprintf(out,"> ");
			continue;
		}        
		generateResult(words,ht,limit);
		free(words);
		fprintf(out,"> ");
	}
	if (quiet) {
		fclose(in);
		fclose(out);
	}
	happly(ht,delete);
	hclose(ht);
	return 0;
}
