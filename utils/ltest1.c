/* test1.c --- 
 * 
 * 
 * Author: Kevin D. Larkin
 * Created: Fri Oct  9 12:38:25 2020 (-0400)
 * Version: 
 * 
 * Description: Aadhya's test1.c 
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void print(car_t *cp) {
	if (cp != NULL) {
		printf("%s", cp->plate);
	} else {
		printf("FAILURE: car_t does not exist");
	}
}

int main(void) {
	car_t *cr_1 = (care_t *)malloc(sizeof(car_t));
	car_t *cr_2 = (care_t *)malloc(sizeof(car_t));
	car_t *cr_3 = (care_t *)malloc(sizeof(car_t));

	//cr_1
	strcpy(cr_1->plate, "max");
	cr_1->price = 5000;
	cr_1->year = 2012;
	cr_1->next = NULL;

	//cr_2
	strcpy(cr_2->plate, "antony");
	cr_2->price = 3004;
	cr_2->year = 2012;
	cr_2->next = NULL;

	//cr_3
	strcpy(cr_3->plate, "brandon");
	cr_3->price = 300000000;
	cr_3->year = 2012;
	cr_3->next = NULL;

	//test lput
	if (lput(cr_1) != 0) {
		printf("FAILURE: car not added to list!\n");
		exit(EXIT_FAILURE);
	} else {
		printf("SUCCESS: car was added to list!\n");
	}

	//test lget
	car_t *cp = lget();
	if (cp != cr_1) {
		printf("FAILURE: car cannot be returned!\n");
		exit(EXIT_FAILURE);
	} else {
		printf("SUCCESS: car returned!\n");
	}

	lput(cr_1);
	lput(cr_2);
	lput(cr_3);

	//test lapply by printing
	lapply(print);

	//test lremove front
	car_t *cp2 = lremove("brandon");
	if (cp2 == NULL) {
		printf("FAILURE: car with plate not found!\n");
		exit(EXIT_FAILURE);
	} else {
		printf("SUCCESS: car with plate returned.\n");
	}
	lapply(print);
	
	//test lremove end
	car_t *cp3 = lremove("max");
	if (cp3 == NULL) {
		printf("FAILURE: car with plate not found!\n");
		exit(EXIT_FAILURE);
	} else {
		printf("SUCCESS: car with plate not returned.\n");
	}
	
	lapply(print);
	lput(cr_1);
	lput(cr_3);
	lapply(print);

	//test lremove front
	car_t *cp4 = lremove("max");
	if (cp4 == NULL) {
		printf("FAILURE: car with plate not found!\n");
		exit(EXIT_FAILURE);
	} else {
		printf("SUCCESS: car with plate returned.\n");
	}
	lapply(print);

	free(cr_1);
	free(cr_2);
	free(cr_3);
	exit(EXIT_SUCCESS);;
}
