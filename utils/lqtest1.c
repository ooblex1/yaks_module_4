/* lqtest1.c --- 
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Nov  6 21:29:40 2020 (-0500)
 * Version: 1
 * 
 * Description: tests implementation of lqueue module
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"
#include "lqueue.h"

int main(void) {
	lqueue_t *lqp;
	
	lqp = lqopen();
	lqclose(lqp);
	
	return 1;
}
