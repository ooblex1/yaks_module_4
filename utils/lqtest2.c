/* lqtest1.c --- 
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Nov  6 21:29:40 2020 (-0500)
 * Version: 2
 * 
 * Description: tests implementation of lqueue module
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "queue.h"
#include "lqueue.h"

typedef struct rectangle {
	int x;
	int y;
} rectangle_t;

bool searchint(void *elementp, const void *keyp) {
	if (*(int *)elementp==*(int *)keyp) {
		return true;
	}
	return false;
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
	lqueue_t *lqpi, *lqpr;
	int *ip, *jp, *kp, *lp, i=5, j=3, k=5, l=4;
	int32_t lqputres, lqprput1, lqprput2, lqprput3;
	rectangle_t *r1p, *r2p, *r3p;
	
	r1p = (rectangle_t *)malloc(sizeof(rectangle_t));
	r2p = (rectangle_t *)malloc(sizeof(rectangle_t));
	r3p = (rectangle_t *)malloc(sizeof(rectangle_t));
	
	r1p->x = 1;
	r1p->y = 2;
	r2p->x = 3;
	r2p->y = 4;
	r3p->x = 5;
	r3p->y = 6;
	
	ip = &i;
	jp = &j;
	kp = &k;
	lp = &l;
	
	lqpi = lqopen();
	lqputres = lqput(lqpi,(void *)ip);
	printf("lqputres = %d\n",(int)lqputres);
	lp = (int *)lqsearch(lqpi,searchint,(void *)kp);
	printf("l: %d\n",*lp);
	jp = (int *)lqget(lqpi);
	printf("j: %d\n",*jp);
	lqclose(lqpi);
	
	lqpr = lqopen();
	lqprput1 = lqput(lqpr,(void *)r1p);
	lqprput2 = lqput(lqpr,(void *)r2p);
	lqprput3 = lqput(lqpr,(void *)r3p);
	printf("lqprput1 = %d\n",(int)lqprput1);
	printf("lqprput2 = %d\n",(int)lqprput2);
	printf("lqprput3 = %d\n",(int)lqprput3);

	printf("\nrectangles before:\n");
	lqapply(lqpr,printRectangle);
	lqapply(lqpr,wideshort);
	printf("\nrectangles after:\n");
	lqapply(lqpr,printRectangle);

	//free(r1p);
	//free(r2p);
	//free(r3p);
	lqclose(lqpr);
	
	return 1;
}
