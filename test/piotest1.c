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

#include <stdio.h>
#include <stdlib.h>
#include "pageio.h"
#include "webpage.h"

int main(void) {
	webpage_t *wp = pageload(1, "../crawler/pages");
	
	pagesave(wp, 1000, "../crawler/pages");
	
	exit(EXIT_SUCCESS);
}
