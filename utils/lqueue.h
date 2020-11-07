#pragma once
/* lqueue.h --- public interface to the locked-queue module
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
#include <stdbool.h>
#include <pthread.h>

typedef struct lqueue_t;

//open a new queue that has a mutex lock
lqueue_t *lqopen(void);

//places a lock on associated queue, places an element in the queue, then unlocks the lock
int32_t lqput(lqueue_t *lqp, void *elementp);

//places a lock on associated queue, retrieves an element from the queue, then unlocks the lock
void *lqget(lqueue_t *lqp);

//places a lock on associated queue, applies a function to every element in the queue, then unlocks the lock
void lqapply(lqueue_t *lqp, void (*fn)(void *elementp));

//places a lock on associated queue, applies a search function to every element in the queue, then unlocks the lock
void *lqsearch(lqueue_t *lqp, bool (*searchfn)(void *elementp, const void *keyp), const void *skeyp);

//places a lock on associated queue, closes the queue, then unlocks the lock
void lqclose(lqueue_t *lqp);
