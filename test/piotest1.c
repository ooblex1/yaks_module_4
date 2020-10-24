/* piotest1.c --- Tests implementation of pageio module for Module 5 Step 1
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Oct 23 17:51:35 2020 (-0400)
 * Version: 1
 * 
 * Description: Tests the pagesave() and pageload() functions
 * 
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "hash.h"
#include "pageio.h"
#include "queue.h"
#include "webpage.h"

int main(void) {
	
	webpage_t *wp = pageload(1, "../crawler/pages");
	
	pagesave(wp, 1000, "../crawler/pages");
	
	exit(EXIT_SUCCESS);
}
