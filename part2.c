#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3
pthread_t customers[NUMBER_OF_CUSTOMERS];
pthread_mutex_t customerLocks[NUMBER_OF_CUSTOMERS];
int available[NUMBER_OF_RESOURCES];
int completed[NUMBER_OF_CUSTOMERS];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int main(){
	int i;
	for(i = 0; i< NUMBER_OF_CUSTOMERS; i++){
        completed[i] = 0;
	}
}