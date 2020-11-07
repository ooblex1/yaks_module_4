/* lqueue.c --- private implementation of the locked-queue module
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Nov  6 17:11:28 2020 (-0500)
 * Version: 1
 * 
 * Description: For ENGS50 Module 7 Step 1
 * 
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"
#include "lqueue.h"

static void lock(lqueue_t *lqp) {
	pthread_mutex_lock(lqp->m);
}

static void unlock(lqueue_t *lqp) {
	pthread_mutex_unlock(lqp->m);
}

//open a new queue that has a mutex lock
lqueue_t *lqopen(void) {
	lqueue_t *lqp;
	
	//allocate memory for the lqueue struct
	lqp = (lqueue_t *)malloc(sizeof(lqueue_t));
	
	//open queue, then initialize mutex
	lqp->q = qopen();
	pthread_mutex_init(lqp->m,NULL);
	lqp->m=NULL;

	return lqp;
}

//places a lock on associated queue, places an element in the queue, then unlocks the lock
int32_t lqput(lqueue_t *lqp, void *elementp) {
	int32_t result;
	
	lock(lqp);
	result = qput(lqp->q,elementp);
	unlock(lqp);
	
	return result;
}

//places a lock on associated queue, retrieves an element from the queue, then unlocks the lock
void *lqget(lqueue_t *lqp) {
	void *result;
	
	lock(lqp);
	result = qget(lqp->q);
	unlock(lqp);

	return result;
}

//places a lock on associated queue, applies a function to every element in the queue, then unlocks the lock
void lqapply(lqueue_t *lqp, void (*fn)(void *elementp)) {
	lock(lqp);
	qapply(lqp->q,fn);
	unlock(lqp);
}

//places a lock on associated queue, applies a search function to every element in the queue, then unlocks the lock
void *lqsearch(lqueue_t *lqp, bool (*searchfn)(void *elementp, const void *keyp), const void *skeyp) {
	void *result;
	
	lock(lqp);
	result = qsearch(lqp->q,searchfn,skeyp);
	unlock(lqp);

	return result;
}

//places a lock on associated queue, then closes the queue
void lqclose(lqueue_t *lqp) {
	lock(lqp);
	if (lqp!=NULL) {
		if (lqp->q!=NULL)
			qclose(lqp->q);
		if (lqp->m!=NULL) {
			free(lqp->m);
			lqp->m = NULL;
		}
		free(lqp);
		lqp = NULL;
	}
}
