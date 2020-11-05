/* 
 * queue.c -- implements a generic queue.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

typedef struct pivot_t {
	struct pivot_t *next;
	void *e;
} pivot_t;

typedef struct guide_t {
	pivot_t *front;
	pivot_t *back;
} guide_t;

queue_t* qopen(void) {
	guide_t *q;
	
	q = (guide_t *)malloc(sizeof(guide_t));
	q->front = NULL;
	q->back = NULL;

	if (q == NULL) {
		printf("memory allocation failed");
		return NULL;
	}
	
	return (queue_t *)q;
}

void qclose(queue_t *qp) {
	pivot_t *temp1, *temp2, *ip;
	guide_t *gp = (guide_t *)qp;
	
	if (gp != NULL && gp->front != NULL) {
		for (ip = gp->front; ip != NULL; ip = temp2) {
			if (ip->e != NULL) {
				free(ip->e);
				ip->e = NULL;
			}
			
			temp1 = ip;
			temp2 = ip->next;
			
			free(temp1);
			temp1 = NULL;
		}
	}
	
	free(qp);
	qp = NULL;
}

int32_t qput(queue_t *qp, void *ep) {
	guide_t *q = (guide_t *)qp;
	pivot_t *p;
	int result = 0;
	
	if (q == NULL){
		return 1;
	}

	p = (pivot_t *)malloc(sizeof(pivot_t));
	if (p == NULL) {
		printf("memory allocation failed");
		result = 1;
	} else {
		if (ep == NULL) {
			printf("passed element is NULL");
			result = 1;
		} else {
			p->e = ep;
			p->next = NULL;
			
			if (q->front == NULL) {
				q->front = p;
			} else {
				q->back->next = p;
			}
			q->back = p;
			
			if (q->back != p) {
				result = 1;
			}
		}
	}
	
	return result;
}

void* qget(queue_t *qp) {
	guide_t *q = (guide_t *)qp;
	
	if (q == NULL || q->front == NULL) {
		//printf("Queue is empty, cannot apply qget.\n");
		return NULL;
	}
	
	void *temp = q->front->e;
	pivot_t *p = q->front;
	
	if (q->front == q->back) {
		q->front = NULL;
		q->back = NULL;
	} else {
		q->front = q->front->next;
	}

	free(p);
	p = NULL;
	return temp;
}

void qapply(queue_t *qp, void (*fn)(void *ep)) {
	guide_t *q = (guide_t *)qp;
	
	if (fn == NULL) {
		printf("Error: Invalid function.\n");
	}
	
	if (q == NULL) {
		printf("Queue is empty, cannot apply function.\n");
	} else {
		pivot_t *p;
		
		for (p = q->front; p != NULL; p = p->next) {
			fn(p->e);
		}
	}
}

void* qsearch(queue_t *qp, bool (*searchfn)(void *ep, const void *keyp),
							const void *skeyp) {
	guide_t *q = (guide_t *)qp;
	pivot_t *p;
	
	if (q == NULL || q->front == NULL || q->back == NULL) {
		//printf("Queue is empty.\n");
		return NULL;
	}
	
	for (p = q->front; p != NULL; p = p->next) {
		if (searchfn(p->e, skeyp)) {
			return p->e;
		}
	}

	//printf("Element not found in queue.\n");
	return NULL;
}

void* qremove(queue_t *qp,
							bool (*searchfn)(void *ep, const void *keyp),
							const void *skeyp) {
	if (qp == NULL || searchfn == NULL || skeyp == NULL){
		return NULL;
	}

	void *data = NULL;
	guide_t *q1 = (guide_t *)qp;

	if (q1->front == NULL){
		return NULL;
	}

	if (searchfn(q1->front->e,skeyp) ){
		data = q1->front->e;
		pivot_t *temp = q1->front;
		q1->front = q1->front->next;
		free(temp);
	}else{
		for (pivot_t *p = q1->front; p->next != NULL; ){
			if (searchfn(p->next->e,skeyp)){
				data = p->next->e;
				pivot_t *temp = p->next;
				p->next = temp -> next;
				free(temp);
			}else{
				p=p->next;
			}
		}
	}
	return data;

}

void qconcat(queue_t *qp1, queue_t *qp2) {
	guide_t *q1 = (guide_t *)qp1, *q2 = (guide_t *)qp2;
	
	if (q1 == NULL) {
		printf("Queue to be added to does not exist.\n");
	} else {
		if (q2 == NULL || q2->front == NULL || q2->back == NULL) {
			printf("Queue to be added is empty.\n");
		} else {
			if (q1->front == NULL) {
				q1->front = q2->front;
				q1->back = q2->back;
			} else {
				q1->back->next = q2->front;
				q1->back = q2->back;
			}

			free(q2);
			q2 = NULL;
		}
	}
}