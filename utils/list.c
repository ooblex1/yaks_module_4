/* list.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha
 * Created: Thu Oct  1 17:56:45 2020 (-0400)
 * Version: 
 * 
 * Description: Implements interface of Makefile which hides the list of cars
 * 
 */

#include <stdio.h>
#include <string.h>
#include "list.h"
static car_t *front=NULL;

int32_t lput(car_t *cp) {
	printf("********\nAdding car at front \n");
	if (cp==NULL)
		return 1;
	if (front==NULL) {
		front=cp;
		front->next = cp->next;  
	}
	else {                                                          
		cp->next = front;
		front = cp;
	}
	return 0;	
}

car_t *lget() {
	printf("********\nReturning first car in the list \n"); 
	if (front==NULL) {
		printf("List is empty, cannot apply lget.\n");
		return NULL;
	}	
	car_t *temp = front;
	front = front->next;
	return temp;
}

void lapply(void (*fn)(car_t *cp)){
	if (fn==NULL) {
		printf("Error: Incorrect function.\n");
	}
	if (front==NULL) {
		printf("List is empty, cannot apply function.\n");
	}
	else {
		printf("********\nApplying func to every car \nPLATES: "); 
		car_t *p;
		for (p=front;p!=NULL;p=p->next) {
			fn(p);
		}
		printf("\n");
	}
}

car_t *lremove(char *platep) {
	printf("********\nReturning car with same plate \n");
	car_t *p = NULL;
	car_t *f = front;
	
	if (front==NULL || platep==NULL) {
		printf("front or platep is null\n");
		return NULL;
	}
	if (strcmp(f->plate, platep)==0) {
		printf("Removing %s from front.\n", platep);
		front=f->next;
		return f;
	}
	else {
		printf("removing %s after front.\n", platep);
		for (p=front; p!= NULL; p=p->next) {
			if (strcmp(p->plate, platep)==0) {
				f->next=p->next;
				return f;
			}
			f=p;
		}
	}
	return f;
}
