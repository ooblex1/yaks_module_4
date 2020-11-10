/* htest1.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Oct 9 2020
 * Version: 
 * 
 * Description: Tests hput, happly, hremove, hsearch for NULL elements
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
  lhashtable_t *lhpr_null = NULL;
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

  //hopen for size 0
  printf("testing lhopen for size 0\n");
  lhpr = lhopen(0);
  //testing lhput for rect
  lhpr = lhopen(10);
  printf("testing lhput for null lhashtable\n");
  lhput(lhpr_null, r1p, r1p->number, strlen(r1p->number));
  printf("testing lhput for null key\n");
  char *null_char = NULL;
  lhput(lhpr_null, r1p, null_char, 2);
  
  lhput(lhpr, r1p, r1p->number, strlen(r1p->number));
  lhput(lhpr, r2p, r2p->number, strlen(r2p->number));
  lhput(lhpr, r3p, r3p->number, strlen(r3p->number));

  //testing lhapply for null hash table
  printf("testing lhapply for null hash table\n");
  lhapply(lhpr_null,printRectangle);
  
  //lhsearch for null hash and key
  
  printf("testing lhsearch for null hash table\n");
  void *rect_search = lhsearch(lhpr_null,searchNumber,r1p->number,strlen(r1p->number));
  if (rect_search == NULL) {
      printf("SUCCESS: rectangle not found.\n");
  }
  else {
      printf("FAILURE: rectangle found.\n");
  }

  printf("testing lhsearch for element that doesnot exist\n");
  rect_search = lhsearch(lhpr,searchNumber,"two",strlen("two"));
  if (rect_search == NULL) {
      printf("SUCCESS: rectangle not found.\n");
  }
  else {
      printf("FAILURE: rectangle found.\n");
  }

  //lhremove for null hash and key
  
  printf("testing lhremove for null hash table\n");
  void *rect_remove = lhremove(lhpr_null,searchNumber,r1p->number,strlen(r1p->number));
  if (rect_remove == NULL) {
      printf("SUCCESS: rectangle not found.\n");
  }
  else {
      printf("FAILURE: rectangle removed.\n");
  }

  printf("testing lhremove for element that does not exist");
  rect_remove = lhsearch(lhpr,searchNumber,"two",strlen("two"));
  if (rect_remove == NULL) {
      printf("SUCCESS: rectangle not found.\n");
  }
  else {
      printf("FAILURE: rectangle removed.\n");
  }

  
  
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
