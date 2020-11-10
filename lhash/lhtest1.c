/* htest1.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Nov 9 2020
 * Version: 
 * 
 * Description: Tests hopen, hclose for a non-empty lhash 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "lhash.h"

int main(void) {
  bool fail = false;

	lhashtable_t *lhp;
  
  lhp = lhopen(10);

  if ( lhp == NULL ){
    fail = true;
  }else
    printf("SUCCESS: hash opened!\n");


  lhclose(lhp);

  if(fail){
    exit(EXIT_FAILURE);
  }
	
	exit(EXIT_SUCCESS);
}
