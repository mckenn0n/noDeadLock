#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMBER_OF_RESOURCES 3
#define NUMBER_OF_CUSTOMERS 5

int i = 0;
int j = 0;
pthread_mutex_t mutex;

int available[NUMBER_OF_RESOURCES];
int allocated[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] =  {{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};
int max[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES] =  {{4,7,3}, {2,5,7}, {1,5,4}, {2,2,8}, {4,8,8}};
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

void printVector(int vec[]);
void printNeeded();
void printAllocated();
void *customer(void* customerID);
int safe();
int checkAvailable();
int toMuch(int customerNumber,int request[]);
int hasResorces(int customerNumber,int release[]);
int requestResource(int customerNumber,int request[]);
int releaseResource(int customerNumber,int release[]);


int requestResource(int customerNumber,int request[]) {
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
	if (safe() == 0) {
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
}

void printVector(int vec[]) {
	for (i = 0; i < NUMBER_OF_RESOURCES; ++i) {
		if(i){
			printf(", ");
		}
		printf("%d",vec[i]);
	}
	printf("\n");
}

int safe() {
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
					if(j == NUMBER_OF_RESOURCES - 1) {
						finished[i] = 1;
						for (k = 0; k < NUMBER_OF_RESOURCES; ++k) {
							work[k] += allocated[i][k];
						}
						i = -1;
						break;
					} else {
						continue;
					}
				} else {
					break;
				}
			}
		} else {
			continue;
		}
	}
	for(i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
		if (finished[i] == 0) {
			return -1;
		} else {
			continue;
		}
	}
	return 0;
}

void *customer(void* customerID) {
	int customerNumber = *(int*)customerID;
	while(1) {
		sleep(1);
		int request[NUMBER_OF_RESOURCES];
		pthread_mutex_lock(&mutex);
		for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
			request[i] = rand() % need[customerNumber][i]+1;
		}
		printf("!!Customer %d is requesting ",customerNumber);
		printVector(request);
		requestResource(customerNumber,request);
		pthread_mutex_unlock(&mutex);
		sleep(1);
		int release[NUMBER_OF_RESOURCES];
		pthread_mutex_lock(&mutex);
		for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
			release[i] = rand() % allocated[customerNumber][i] + 1;
		}
		releaseResource(customerNumber,release);
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc, char const *argv[]) {
	srand(time(NULL));
	if(argc != NUMBER_OF_RESOURCES + 1) {
		printf("The number of arguments is incorrect. %d are needed.\n", NUMBER_OF_RESOURCES);
		return -1;
	}
	for(i = 0; i < NUMBER_OF_RESOURCES; i++) {
		available[i] = atoi(argv[i+1]);
	}
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

