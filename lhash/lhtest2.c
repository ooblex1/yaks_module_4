/* htest1.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Nov 9 2020
 * Version: 
 * 
 * Description: Tests hput, happly, hremove, hsearch for a non-empty lhash single thread
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "lhash.h"

typedef struct rectangle_t {
  char number[20];
  int x;
  int y;
} rectangle_t;

bool searchNumber(void *elementp, const void *keyp) {
  rectangle_t *element = (rectangle_t *)elementp;
  return (element->number == (char *)keyp);
}

void wideshort(void *elementp) {
  rectangle_t *r = (rectangle_t *)elementp;
  r->x++;
  r->y--;
}

void printRectangle(void *elementp) {
  rectangle_t *r = (rectangle_t *)elementp;
  printf("x=%d , y=%d\n", r->x, r->y);
}


int main(void) {
  bool fail = false;

  lhashtable_t *lhpr;
  int32_t lhprput1, lhprput2, lhprput3;
  rectangle_t *r1p, *r2p, *r3p;
  
  //initiating 3 rects
  r1p = (rectangle_t *)malloc(sizeof(rectangle_t));
  r2p = (rectangle_t *)malloc(sizeof(rectangle_t));
  r3p = (rectangle_t *)malloc(sizeof(rectangle_t));
  
  r1p->x = 1;
  r1p->y = 2;
  strcpy(r1p->number,"one");
  r2p->x = 3;
  r2p->y = 4;
  strcpy(r2p->number,"three");
  r3p->x = 5;
  r3p->y = 6;
  strcpy(r3p->number,"five");

  lhpr = lhopen(10);
  //testing lhput for rect
  printf("testing lhput for rectangles\n");
  lhprput1 = lhput(lhpr, r1p, r1p->number, strlen(r1p->number));
  if (lhprput1!=0){
    printf("FAILURE: rectangle not added to hash!\n");
    fail = true;
  }else
    printf("SUCCESS: rectangle is added to hash!\n");

  lhprput2 = lhput(lhpr, r2p, r2p->number, strlen(r2p->number));
  lhprput3 = lhput(lhpr, r3p, r3p->number, strlen(r3p->number));
  printf("lhprput1 = %d\n",(int)lhprput1);
  printf("lhprput2 = %d\n",(int)lhprput2);
  printf("lhprput3 = %d\n",(int)lhprput3);
  

  //testing lhapply for rects
  printf("testing lhapply for rectangles\n");
  printf("\nrectangles before:\n");
  lhapply(lhpr,printRectangle);
  lhapply(lhpr,wideshort);
  printf("\nrectangles after:\n");
  lhapply(lhpr,printRectangle);

  
  //testing lhremove for rects
  
  void *rect_remove = lhremove(lhpr,searchNumber,r1p->number,strlen(r1p->number));
  if (rect_remove == NULL) {
      printf("FAILURE: rectangle not found.\n");
      fail = true;
  }
  else {
      printf("SUCCESS: rectangle removed.\n");
  }

  lhapply(lhpr,printRectangle);

	
  //closing
  //free(r1p);
  //free(rect_remove):

  lhclose(lhpr);

  //end code
  if(fail){
    exit(EXIT_FAILURE);
  }
	
	exit(EXIT_SUCCESS);
}
