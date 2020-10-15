/* qtest1.c --- 
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Oct  9 13:33:51 2020 (-0400)
 * Version: 1
 * 
 * Description: tests queue.c: qopen()
 * 
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "queue.h"

int main(void) {
	bool fail = false;
	queue_t *a;
	
	//call qopen
	a = qopen();
	if (*(int *)a == 1) {
		fail = true;
	}
	
	if (a != NULL) {
		qclose(a);
	}
	if (fail) {
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
