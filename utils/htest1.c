/* htest1.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Oct 9 2020
 * Version: 
 * 
 * Description: Tests hopen, hclose, hput, happly, hremove, hsearch for a non-empty hash 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hash.h"
#include "list.h"

void print(void *p) {
	car_t *cp = (car_t *)p;

    if (cp!=NULL) {
        printf("%s \n", cp->plate);
    }
    else
        printf("FAILURE: car_t does not exist");
}

void yearup(void *p) {
	car_t *cp = (car_t *)p;

	if (cp != NULL){
		printf("old year : %d\n", cp->year);
		cp->year = cp->year +1;
		printf("new year: %d\n", cp->year);
	}else{
		printf("FAILURE: passes argument is NULL\n");
	}
}


bool search(void *elp, const void *keyp) {
	car_t *cp = (car_t *)elp;
	if (strcmp(cp->plate, (char *)keyp) == 0 || cp->price == *(double *)keyp || cp->year == *(int *) keyp){
			return true;
		}
		return false;
}


int main(void) {
	bool fail = false;
  car_t *cr_1 = (car_t*)malloc(sizeof(car_t));              
  car_t *cr_2 = (car_t*)malloc(sizeof(car_t));              
  car_t *cr_3 = (car_t*)malloc(sizeof(car_t));              
  
  //cr_1
  strcpy(cr_1->plate,"max");
    cr_1->price = 5000;
    cr_1->year = 2012;
    cr_1->next = NULL;
    
  //cr_2
  strcpy(cr_2->plate,"antony");
  cr_2->price = 3004;
  cr_2->year = 2011;                                                            
  cr_2->next = NULL;

  //cr_3                                                                            
  strcpy(cr_3->plate,"brandon");
  cr_3->price = 300000000;
  cr_3->year = 2010;
  cr_3->next = NULL;


	//test hopen
	hashtable_t* ht = hopen(10);
	
	if ( ht == NULL ){
		fail = true;
	}else
    printf("SUCCESS: hash opened!\n");
	

	//test hput
  if (hput(ht, cr_1, cr_1->plate,strlen(cr_1->plate))!=0){
    printf("FAILURE: car not added to hash!\n");
	  fail = true;
  }else
    printf("SUCCESS: car is added to hash!\n");

  hput(ht, cr_2, cr_2->plate, strlen(cr_2->plate));
  hput(ht, cr_3, cr_3->plate, strlen(cr_3->plate));


	//test happly by year up
  printf("Testing happly, car year should increase by 1\n");
  happly(ht, yearup);
  happly(ht,print);


 
  //test hsearch
  void *cp_search = hsearch(ht,search, "max",3);
  if (cp_search == NULL) {
      printf("FAILURE: car with plate not found!\n");
			fail = true;
  }
  else {
      printf("SUCCESS: car with plate returned.\n");
  }



  //test hremove
  void *cp_remove = hremove(ht,search,"max",3);
  if (cp_remove == NULL) {
      printf("FAILURE: car with plate max not found!\n");
			fail = true;
  }
  else {
      printf("SUCCESS: car with plate max removed.\n");
  }

  happly(ht,print);
	
	//test hclose
	hclose(ht);

  free(cr_1);
  //free(cr_2);
  //free(cr_3);

	if(fail){
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
