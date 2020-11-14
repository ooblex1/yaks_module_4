/* library.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha
 * Created: Fri Nov 13 15:37:41 2020 (-0500)
 * Version: 
 * 
 * Description: threads should concurrently retrieve webpages
 using two shared resources: the queue of pages to be crawled
 and the hash-table of urls.
 * 
 */

/*
Library.c file in utils
struct w queue hash etc.
then method that takes total and returns total

in crawler total structure needs to have
dirname maxdepth max queue doc id
*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "lhash.h"
#include "lqueue.h"

struct TotalCrawled { 
    lhashtable_t *lht; 
    lqueue_t *lqp; 
    char* pagedir;
    char* seedurl;
    int maxdepth;
	int threadnm;
    //int doc_id;
};

//create method to take struct input in
