/* indexio.c --- 
 * 
 * 
 * Author: Ye Zhang, Aadhya Kocha
 * Created: Sat Oct 24 15:31:16 2020 (-0400)
 * Version: 
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include "hash.h"
#include "queue.h"
#include "indexio.h"

static FILE *fp;

//qprintf takes us within every element of the queue which has crawled documents with each word in the index
static void qprintf(void *elmt){ 
    if (elmt == NULL) {
	    printf("Error in qprintf function\n");
	}
    else {
        doc_t *qelmt = (doc_t *)elmt;
        fprintf(fp," %d %d",qelmt->document,qelmt->count);
    }
}

static void hprintf(void *elmt){
    if (elmt == NULL) {
	    printf("Error in hprintf function\n");
	}
    else {
        wordDoc_t *helmt = (wordDoc_t *)elmt;
        fprintf(fp,"%s",helmt->word);
        qapply(helmt->doc_q,qprintf);
        fprintf(fp,"\n");
    }
}

int32_t indexsave(hashtable_t *ht, char *indexnm){
    if (indexnm == NULL) {
        printf("index is null! \n");
        return 1;
    }

    fp = fopen(indexnm,"w");
    if (fp == NULL) {
        printf("usage: directory not found");
        return 2;
    }
    happly(ht,hprintf);
    fclose(fp);
    
    return 0;
}

hashtable_t *indexload(char *indexnm){
	if (indexnm == NULL) {
        printf("file is null! \n");
        return NULL;
    }
    hashtable_t *index;
    index = hopen(100);
    char word[50];

    FILE *f;
    if((f = fopen("./oldindex", "r")) == NULL) {
        printf("file is empty! \n");
        return NULL;
    }
    while(fscanf(f,"%s",word) == 1) {
        int id, count;
        wordDoc_t *wordDoc = (wordDoc_t*)malloc(sizeof(wordDoc_t));
        char *wordcpy = malloc(strlen(word)+1);
        strcpy(wordcpy,word);
        wordDoc->word = wordcpy;
        wordDoc->doc_q = qopen();

        while (fscanf(f," %d %d",&id,&count) == 2) {
            doc_t* new_doc = (doc_t*)malloc(sizeof(doc_t));
            new_doc->document = id;
            new_doc->count = count;
            qput(wordDoc->doc_q,new_doc);
        }

        hput(index,wordDoc,word,strlen(word));
        
    }

    fclose(f);
    return index;
}