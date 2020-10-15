/* htest1.c --- 
 * 
 * 
 * Author: Ye Zhang
 * Created: Oct 9 2020
 * Version: 
 * 
 * Description: Tests hput, happly, hremove, hsearch for an empty hash 
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


	//hopen hash
	hashtable_t* ht = hopen(10);
  hashtable_t* ht_empty = NULL;
	
  if ( ht == NULL ){
		fail = true;
    printf("FAILURE: hash can't be opened!\n");
	}
    
  //opening hash table with size 0
  printf("--Testing hopen(0)\n");
  ht_empty=hopen(0);
  if ( ht_empty == NULL ){
    printf("SUCCESS: hash can't be opened with hopen(0)!\n");
  }else{
    printf("FAILURE: hash opened with hopen(0)!\n");
    fail = true;
  }


 //printf("ht_empty is located: %p\n", (void *) ht_empty);

	//test hput to empty hash pointer
  printf("--Testing hput to an empty hashtable\n");
  if (hput(ht_empty, cr_1, cr_1->plate,strlen(cr_1->plate))!=0){
    printf("SUCCESS: car not added to empty hash!\n");
  }else {
    printf("FAILURE: car is added to empty hash!\n");
    fail = true;
  }

  hput(ht, cr_1, cr_1->plate, strlen(cr_1->plate));
  hput(ht, cr_2, cr_2->plate, strlen(cr_2->plate));
	hput(ht, cr_3, cr_3->plate, strlen(cr_3->plate));

	//test happly by year up to an empty hashtable
  printf("--Testing happly to an empty hashtable\n");
  happly(ht_empty, yearup);

 
  //test hsearch to empty hashtable
  printf("--Testing hsearch for an empty hash table\n");
  void *cp_search = hsearch(ht_empty,search, "max",3);
  if (cp_search == NULL) {
      printf("SUCCESS: car with plate not found in empty hash!\n");
  }
  else {
      printf("FAILURE: car with plate returned from empty hash.\n");
      fail = true;
  }

  //test hsearch when the element doesn't exist
  printf("--Testing hsearch when element searched doesn't exist\n");
  void *cp_search2 = hsearch(ht,search, "mary",4);
  if (cp_search2 == NULL) {
      printf("SUCCESS: car with plate mary not found in hash that has no such element!\n");
  }
  else {
      printf("FAILURE: car with plate mary returned from hash that has no such element!.\n");
      fail = true;
  }


  //test hremove for empty list
  printf("--Testing hremove for an empty hash table\n");
  void *cp_remove = hremove(ht_empty,search,"max",3);
  if (cp_remove == NULL) {
      printf("SUCCESS: car with plate not found/removed in empty hash!\n");
  }
  else {
      printf("FAILURE: car with plate returned/removed from empty hash.\n");
      fail = true;
  }

  //test hremove for elements that doesn't exist
  printf("--Testing hremove when element removed doesn't exist\n");
  void *cp_remove2 = hremove(ht,search,"mary",4);
  if (cp_remove2 == NULL) {
      printf("SUCCESS: car with plate mary not found/removed in hash that doesn not contain the element!\n");
  }
  else {
      printf("FAILURE: car with plate mary returned/removed from hash that doesn not contain the element.\n");
      fail = true;
  }

	//test hclose
  printf("--Testing hclose for a null pointer\n");
  hclose(ht_empty);


	hclose(ht);


  //free(cr_1);
  //free(cr_2);
  //free(cr_3);

	if(fail){
		exit(EXIT_FAILURE);
	}
	
	exit(EXIT_SUCCESS);
}
