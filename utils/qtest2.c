/* qtest2.c --- 
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Oct  9 16:43:51 2020 (-0400)
 * Version: 1
 * 
 * Description: tests queue.c: qput, qget, qapply, qsearch, qremove, qconcat for a non-empty queue
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "queue.h"

void print(void *p) {
	car_t *cp = (car_t *)p;
	
	if (cp != NULL) {
		printf("%s\n", cp->plate);
	} else {
		printf("FAILURE: passed argument is NULL\n");
	}
}

void yearup(void *p) {
	car_t *cp = (car_t *)p;
	
	if (cp != NULL) {
		printf("old year: %d\n", cp->year);
		cp->year = cp->year + 1;
		printf("new year: %d\n", cp->year);
	} else {
		printf("FAILURE: passed argument is NULL\n");
	}
}

bool search(void *elp, const void *keyp) {
	car_t *cp = (car_t *)elp;
	if (strcmp(cp->plate, (char *)keyp) == 0 || cp->price == *(double *)keyp || cp->year == *(int *)keyp) {
		return true;
	}
	return false;
}

int main(void) {
	bool fail = false;
	int32_t pr1, pr2, pr3, pr4;
	queue_t *a, *b;
	car_t *cr_1, *cr_2, *cr_3, *cr_4, *cr_a, *cr_b, *cr_c, *cr_d, *cp;
	int y1 = 2013, y2 = 2001;
	const void *yp1, *yp2;
	
	yp1 = &y1;
	yp2 = &y2;
	
	//allocate memory for elements (cars)
	cr_1 = (car_t *)malloc(sizeof(car_t));
	cr_2 = (car_t *)malloc(sizeof(car_t));
	cr_3 = (car_t *)malloc(sizeof(car_t));
	cr_4 = (car_t *)malloc(sizeof(car_t));
	
	//initialize elements (cars)
	strcpy(cr_1->plate, "amy");
	cr_1->price = 5000;
	cr_1->year = 2012;
	cr_1->next = NULL;
	
	strcpy(cr_2->plate, "bob");
	cr_2->price = 3;
	cr_2->year = 2010;
	cr_2->next = NULL;
	
	strcpy(cr_3->plate, "caz");
	cr_3->price = 5000;
	cr_3->year = 2010;
	cr_3->next = NULL;
	
	strcpy(cr_4->plate, "don");
	cr_4->price = 20000;
	cr_4->year = 2007;
	cr_4->next = NULL;
	
	//call qopen
	a = qopen();
	
	//test qput: successful return (0) for empty and non-empty list
	pr1 = qput(a, (void *)cr_1);
	pr2 = qput(a, (void *)cr_2);
	pr3 = qput(a, (void *)cr_3);
	pr4 = qput(a, (void *)cr_4);
	if (pr1 != 0 || pr2 != 0 || pr3 != 0 || pr4 != 0) {
		fail = true;
		printf("\nFAIL: test qput: successful return (0) for empty and non-empty list\n");
	}
	
	//test qapply: function was applied to each element
	qapply(a, yearup);
	if (cr_1->year != 2013 || cr_2->year != 2011 || cr_3->year != 2011 || cr_4->year != 2008) {
		fail = true;
		printf("\nFAIL: test qapply: function was applied to each element\n");
	}
	
	//test qget: first one in, first one out
	if ((car_t *)qget(a) != cr_1) {
		fail = true;
		printf("\nFAIL: test qget: first one in, first one out\n");
	}
	
	//test qget: removes element from queue
	if ((car_t *)qget(a) != cr_2) {
		fail = true;
		printf("\nFAIL: test qget: removes element from queue\n");
	}
	
	//test qput: successful return (0) for re-entry
	pr1 = qput(a, (void *)cr_1);
	pr2 = qput(a, (void *)cr_2);
	if (pr1 != 0 || pr2 !=0) {
		fail = true;
		printf("\nFAIL: test qput: successful return (0) for re-entry\n");
	}
	
	//the queue is now in the following order: cr_3 (front), cr_4, cr_1, cr_2 (back)
	printf("Queue should now be in the order 3,4,1,2, so the following message should read \"caz don amy bob\":\n");
	qapply(a, print);
	
	//test qsearch: successful return (pointer to element) when skeyp is in queue
	if ((car_t *)qsearch(a, search, yp1) != cr_1) {
		fail = true;
		printf("\nFAIL: test qsearch: successful return (pointer to element) when skeyp is in queue\n");
	}
	
	//test qsearch: successful return (NULL) when skeyp is not in queue
	if (qsearch(a, search, yp2) != NULL) {
		fail = true;
		printf("\nFAIL: test qsearch: successful return (NULL) when skeyp is not in queue\n");
	}
	
	//test qremove: successful return (pointer to element) when skeyp is in queue
	cp = (car_t *)qremove(a, search, yp1);
	if (cp != cr_1) {
		fail = true;
		printf("\nFAIL: test qremove: successful return (pointer to element) when skeyp is in queue\n");
	}
	
	//the queue is now in the following order: cr_3 (front), cr_4, cr_2 (back)
	printf("Queue should now be in the order 3,4,2, so the following message should read \"caz don bob\":\n");
	qapply(a, print);
	
	//test qremove: removes element when element is found
	qget(a);
	qget(a);
	cr_b = (car_t *)qget(a);
	if (cr_b != cr_2) {
		fail = true;
		printf("\nFAIL: test qremove: removes element when element is found\n");
	}

	//the queue is now empty
	printf("Queue should now be empty, so an approporiate message should follow:\n");
	qapply(a, print);
	
	//re-fill queue
	qput(a, cr_1);
	qput(a, cr_2);
	qput(a, cr_3);
	qput(a, cr_4);
	
	//the queue is now in the following order: cr_1 (front), cr_2, cr_3, cr_4 (back)
	printf("Queue should now be in the order 1,2,3,4, so the following message should read \"amy bob caz don\":\n");
	qapply(a, print);
	
	//test qremove: successful return (NULL) when skeyp is not in queue
	if (qsearch(a, search, yp2) != NULL) {
		fail = true;
		printf("\nFAIL: test qremove: successful return (NULL) when skeyp is not in queue\n");
	}
	
	//test qremove: does not remove any elements when element is not found
	cr_a = qget(a);
	cr_b = qget(a);
	cr_c = qget(a);
	cr_d = qget(a);
	if (cr_a != cr_1 || cr_b != cr_2 || cr_c != cr_3 || cr_d != cr_4) {
		fail = true;
		printf("\nFAIL: test qremove: does not remove any elements when element is not found\n");
	}
	
	//the queue is now empty
	printf("Queue should now be empty, so an approporiate message should follow:\n");
	qapply(a, print);
	
	//make two queues
	qput(a, (void *)cr_1);
	qput(a, (void *)cr_2);
	b = qopen();
	qput(b, (void *)cr_3);
	qput(b, (void *)cr_4);
	
	//the queues are now in the following orders: a: cr_1 (front), cr_2 (back); b: cr_3 (front), cr_4 (back)
	printf("\nQueue a, right before qconcat is called:\n");
	qapply(a, print);
	printf("\nQueue b, right before qconcat is called:\n");
	qapply(b, print);
	
	//call qconcat
	qconcat(a, b);

	//the queues are now in the following orders: a: cr_1 (front), cr_2, cr_3, cr_4 (back); b: empty
	printf("\nQueue a, right after qconcat is called:\n");
	qapply(a, print);
	printf("\nTrying to print queue b would result in a valgrind error, since it was deallocated by qconcat.\n");
	
	//test qconcat: successfully adds to q1 & deletes q2 when neither are empty
	cr_a = qget(a);
	cr_b = qget(a);
	cr_c = qget(a);
	cr_d = qget(a);
	if (cr_a != cr_1 || cr_b != cr_2 || cr_c != cr_3 || cr_d != cr_4) {
		fail = true;
		printf("\nFAIL: test qconcat: adds to q1 & deletes q2 when neither are empty\n");
	}
	
	//deallocate memory
	qclose(a);
	free(cr_1);
	free(cr_2);
	free(cr_3);
	free(cr_4);
	
	//exit with appropriate exit status
	if (fail) {
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
