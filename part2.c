#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_RESOURCES 3
#define NUMBER_OF_CUSTOMERS 5

int i = 0;
int j = 0;
pthread_mutex_t mutex;//mutex lock for access to global variable

int available[NUMBER_OF_RESOURCES];
int allocated[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] =  { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};
int max[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] =  { {4,7,3}, {2,5,7}, {1,5,4}, {2,2,8}, {4,8,8}};
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

void printVector(int vec[]);
void printNeeded();
void printAllocated();
void *customer(void* customerID);
int ifInSafeMode();
int checkAvailable();
int toMuch(int customerNumber,int request[]);
int hasResorces(int customerNumber,int release[]);
int requestResource(int customerNumber,int request[]);
int releaseResource(int customerNumber,int release[]);


int requestResource(int customerNumber,int request[]) {
	//whether request number of resources is greater than needed
	if (toMuch(customerNumber,request) == -1) {
		return -1;
	}
	if(checkAvailable(request) == -1) {
		return -1;
	}
	for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
		need[customerNumber][i] -= request[i];
		allocated[customerNumber][i] += request[i];
		available[i] -= request[i];
	}
	if (ifInSafeMode() == 0) {
		printf("**Customer %d was granted ",customerNumber);
		printVector(request);
		printf("Available resources are:\n");
		printVector(available);
		printf("Allocated matrix is:\n");
		printAllocated();
		printf("Need matrix is:\n");
		printNeeded();
		return 0;
	} else {
		for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
			need[customerNumber][i] += request[i];
			allocated[customerNumber][i] -= request[i];
			available[i] += request[i];
		}
		return -1;
	}
}

int releaseResource(int customerNumber,int release[]) {
	if(hasResorces(customerNumber,release) == -1) {
		return -1;
	}

	//enough to release
	for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
		allocated[customerNumber][i] -= release[i];
		need[customerNumber][i] += release[i];
		available[i] += release[i];
	}
	printf("--Customer %d releases ",customerNumber);
	printVector(release);
	printf("Available resources are:\n");
	printVector(available);
	printf("Allocated matrix is:\n");
	printAllocated();
	printf("Need matrix is:\n");
	printNeeded();
	return 0;
}

int hasResorces(int customerNumber,int release[]) {
	for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
		if (release[i] > allocated[customerNumber][i]) {
			return -1;
		}
	}
	return 0;
}

int toMuch(int customerNumber,int request[]) {
	int j;
	for (j = 0; j < NUMBER_OF_RESOURCES; ++j) {
		if (request[j] > need[customerNumber][j]) {
			return -1;
		}
	}
	return 0;
}

int checkAvailable(int request[]) {
	//first element of request is customerNumber
	int j;
	for (j = 0; j < NUMBER_OF_RESOURCES; ++j) {
		if (request[j] > available[j]) {
			return -1;
		}
	}
	return 0;
}

void printNeeded() {
	for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
		printf("Customer %d {", i);
		for (j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			if(j){
				printf(", ");
			}
			printf("%d", need[i][j]);
		}
		printf("}\n");
	}
	return;
}

void printAllocated() {
	for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
		printf("Customer %d {", i);
		for (j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			if(j){
				printf(", ");
			}
			printf("%d", allocated[i][j]);
		}
		printf("}\n");
	}
	return;
}

void printVector(int vec[]) {
	for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
		if(i){
			printf(", ");
		}
		printf("%d",vec[i]);
	}
	printf("\n");
	return;
}

int ifInSafeMode() {
	int finished[NUMBER_OF_CUSTOMERS] =  {0};
	int work[NUMBER_OF_RESOURCES];
	for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
		work[i] = available[i];
	}
	int k;
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		if (finished[i] == 0) {
			for(j = 0; j < NUMBER_OF_RESOURCES; j++) {
				if(need[i][j] <= work[j]) {
					if(j == NUMBER_OF_RESOURCES - 1) {//means we checked whole vector, so this process can execute
						finished[i] = 1;
						for (k = 0; k < NUMBER_OF_RESOURCES; ++k) {
							work[k] += allocated[i][k];
							//execute and release resources
						}
						//if we break here, it will not check all process, so we should reset i to let it check from beginning
						//If we cannot find any runnable process from beginning to the end in i loop, we can determine that
						//there is no any runnable process, but we cannot know if we do not reset i.
						i = -1;//at the end of this loop, i++, so -1++ = 0
						break;//in loop j, break to loop i and check next runnable process
					} else {//not finished checking all resource, but this kind resources is enough
						continue;
					}
				} else {//resources not enough, break to loop i for next process
					//because there is no change happened, so we do not need to reset i in this condition.
					break;
				}
			}
		} else {
			continue;
		}
	}
	//there are two condition if we finish loop i
	//1. there is no process can run in this condition.
	//2. all processes are runned, which means it is in safe status.
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		if (finished[i] == 0) {
			//not all processes are runned, so it is condition 1.
			return -1;
		} else {
			continue;
		}
	}
	//finished loop, so it is condition 2
	return 0;
}

void *customer(void* customerID) {
	int customerNumber = *(int*)customerID;

	while(1) {
		//request random number of resources
		sleep(1);
		int request[NUMBER_OF_RESOURCES];

		//Because i is global variable, we should lock from here
		//lock mutex for accessing global variable and printf
		pthread_mutex_lock(&mutex);
		//initialize request
		for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
			if(need[customerNumber][i] != 0) {
				request[i] = rand() % need[customerNumber][i]+1;
			} else {
				request[i] = 0;
			}
		}


		printf("!!Customer %d is requesting ",customerNumber);
		printVector(request);
		//requestResource() will still return -1 when it fail and return 0 when succeed in allocate, like textbook says
		//altough I put the error message output part in the requestResource function
		requestResource(customerNumber,request);
		//unlock
		pthread_mutex_unlock(&mutex);
	
		//release random number of resources		
		sleep(1);
		int release[NUMBER_OF_RESOURCES];
		//Because i is global variable, we should lock from here
		//lock mutex for accessing global variable and printf
		pthread_mutex_lock(&mutex);
		//initialize release
		for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
			if(allocated[customerNumber][i] != 0) {
				release[i] = rand() % allocated[customerNumber][i] + 1;
			} else {
				release[i] = 0;
			}
		}
		//releaseResource() will still return -1 when it fail and return 0 when succeed in allocate, like textbook says
		//altough I put the error message output part in the releaseResource function
		releaseResource(customerNumber,release);
		//unlock
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, char const *argv[]) {
	srand(time(NULL));
	if(argc != NUMBER_OF_RESOURCES + 1) {
		printf("Quantity of parameter is not correct.\n");
		return -1;
	}
	for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
		available[i] = atoi(argv[i+1]);
	}

	//initialize need
	for (i = 0; i < NUMBER_OF_CUSTOMERS; ++i) {
		for (j = 0; j < NUMBER_OF_RESOURCES; ++j) {
			need[i][j] = max[i][j] - allocated[i][j];
		}
	}

	printf("Available resources are:\n");
	printVector(available);

	printf("Initial allocation matrix is:\n");
	printAllocated();

	printf("Initial need matrix is:\n");
	printNeeded();

	pthread_mutex_init(&mutex,NULL);
	pthread_t *threadAddress = malloc(sizeof(pthread_t) * NUMBER_OF_CUSTOMERS);
	int *cNumber = malloc(sizeof(int) * NUMBER_OF_CUSTOMERS);
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		*(cNumber + i) = i;
		pthread_create((threadAddress+i), NULL, customer, (cNumber+i));
	}
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		pthread_join(*(threadAddress+i),NULL);
	}
	free(cNumber);
	free(threadAddress);
	return 0;
}

