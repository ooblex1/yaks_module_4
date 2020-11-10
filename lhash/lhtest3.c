/* htest1.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Oct 9 2020
 * Version: 
 * 
 * Description: Tests hput, happly, hremove, hsearch for multi thread
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

void thin_tall(void *elementp) {
  rectangle_t *r = (rectangle_t *)elementp;
  r->x-=2;
  r->y+=2;
}

void printRectangle(void *elementp) {
  rectangle_t *r = (rectangle_t *)elementp;
  printf("x=%d , y=%d\n", r->x, r->y);
}

void *tfunc1(void *argp) {
  lhashtable_t *lhp = (lhashtable_t *)argp;
  lhapply(lhp,wideshort);
  lhapply(lhp,printRectangle);
  return NULL;
}

void *tfunc2(void *argp) {
  lhashtable_t *lhp = (lhashtable_t *)argp;
  lhapply(lhp,thin_tall);
  lhapply(lhp,printRectangle);
  return NULL;
}

int main(void) {
  

  lhashtable_t *lhpr;
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
  lhput(lhpr, r1p, r1p->number, strlen(r1p->number));
  lhput(lhpr, r2p, r2p->number, strlen(r2p->number));
  lhput(lhpr, r3p, r3p->number, strlen(r3p->number));
  

  pthread_t tid1;
  pthread_t tid2;
  
  if (pthread_create(&tid1,NULL,tfunc1,(void *)lhpr)!=0)
      exit(EXIT_FAILURE);

  if (pthread_create(&tid2,NULL,tfunc2,(void *)lhpr)!=0)
      exit(EXIT_FAILURE);

  if (pthread_join(tid1,NULL)!=0)
      exit(EXIT_FAILURE);

  if (pthread_join(tid2,NULL)!=0)
      exit(EXIT_FAILURE);
  
 lhclose(lhpr);
	
	exit(EXIT_SUCCESS);
}
