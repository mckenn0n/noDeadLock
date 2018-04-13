#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>

#define N 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define TOLEFT (philosopherNumber + 4) % N
#define TORIGHT (philosopherNumber + 1) % N

int state[N];
int philosophers[N] = { 0, 1, 2, 3, 4 };
sem_t mutex;
sem_t S[N];

void checkAvailability(int philosopherNumber, int time){
	if (state[philosopherNumber] == HUNGRY && state[TOLEFT] != EATING && state[TORIGHT] != EATING) {
		state[philosopherNumber] = EATING;
		printf("Philosopher %d is eating for %d seconds\n", philosopherNumber, time);
		sleep(time);
		sem_post(&S[philosopherNumber]);
	}
}

void takeFork(int philosopherNumber,int time){
	sem_wait(&mutex);
	state[philosopherNumber] = HUNGRY;
	printf("***Philosopher %d is trying to take a fork***\n", philosopherNumber);
	checkAvailability(philosopherNumber, time);
	sem_post(&mutex);
	sem_wait(&S[philosopherNumber]);
}

void replaceFork(int philosopherNumber,int time){
	sem_wait(&mutex);
	state[philosopherNumber] = THINKING;
	printf("Philosopher %d is thinking for %d seconds\n", philosopherNumber, time);
	checkAvailability(TOLEFT, time);
	checkAvailability(TORIGHT, time);
	// sleep(time);
	sem_post(&mutex);
}

void* philospher(void* num){
	while (1){
		int* i = num;
		int r1 = (rand() % 5) + 1;
		int r2 = (rand() % 5) + 1;
		takeFork(*i, r1);
		// sleep(r1);
		replaceFork(*i, r2);
		sleep(r2);
	}
}

int main(){
	int i;
	pthread_t thread_id[N];
	sem_init(&mutex, 0, 1);

	for (i = 0; i < N; i++){
		sem_init(&S[i], 0, 0);
	}

	for (i = 0; i < N; i++){
		pthread_create(&thread_id[i], NULL, philospher, &philosophers[i]);
	}

	for (i = 0; i < N; i++){
		pthread_join(thread_id[i], NULL);
	}

	return 0;
}