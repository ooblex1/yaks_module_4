/* ltest2.c --- 
 * 
 * 
 * Author: Aadhya N. Kocha
 * Created: Sat Oct 10 07:12:52 2020 (-0400)
 * Version: 
 * 
 * Description: tests lput, lget, lapply, lremove for an empty list
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

void print(car_t *cp) {
    if (cp!=NULL) {
        printf("%s ", cp->plate);
    }
    else {
        printf("FAILURE: car_t does not exist");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    car_t *cr_1 = (car_t*)malloc(sizeof(car_t));
  
    //cr_1
    strcpy(cr_1->plate,"max");
    cr_1->price = 5000;
    cr_1->year = 2012;
    cr_1->next = NULL;
    
    //test lget in empty list
    lget();

    //test lapply by printing
    lapply(print);

    //test lremove
    car_t *cp2 = lremove("max");
    if (cp2 == NULL) {
        printf("List is empty, cannot remove car from List\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("SUCCESS: car with plate returned.\n");
    }

    //test lput in empty list
    if (lput(cr_1)!=0) {
        printf("FAILURE: car not added to list!\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("SUCCESS: car is added to list!\n");

    free(cr_1);
    return 0;                                                                
}
