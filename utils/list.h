#pragma once
/* 
 * list.h -- public interface to a list of cars
 */
#include <stdint.h>

#define MAXREG 10

/* the representation of a car */
typedef struct car {
	struct car *next;
	char plate[MAXREG];
	double price;
	int year;
} car_t;

/* put(): place a car at the beginning of the list
 * returns 0 if successful; nonzero otherwise 
 */
int32_t lput(car_t *cp); 

/* get(): remove and return the first car in the list;
 * return NULL if the list is empty
 */
car_t *lget();

/* apply a function to every car in the list */
void lapply(void (*fn)(car_t *cp));

/* remove(): find, remove, and return any car with
 * the designated plate; return NULL if not present
 */
car_t *lremove(char *platep);
