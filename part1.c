#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 5
pthread_t philosopher[N];
pthread_mutex_t chopstick[N];

void replaceFork(int philosopherNumber){
	pthread_mutex_unlock(&chopstick[philosopherNumber]);
	pthread_mutex_unlock(&chopstick[(philosopherNumber+1)%N]);
}

int takeFork(int philosopherNumber){
	int one = pthread_mutex_trylock(&chopstick[philosopherNumber]);
	int two = pthread_mutex_trylock(&chopstick[(philosopherNumber+1)%N]);
	if (one != 0 || two != 0){
		replaceFork(philosopherNumber);
		return -1;
	}
	return 1;
}

void *philospher(int philosopherNumber){
	while(1){
		int r1 = (rand() % 5) + 1;
		int r2 = (rand() % 5) + 1;
		printf ("\nPhilosopher %d is thinking for %d second", philosopherNumber, r1);
		sleep(r1);
		int go = takeFork(philosopherNumber);
		if (go == 1){
			printf ("\nPhilosopher %d is eating for %d seconds", philosopherNumber, r2);
			sleep(r2);
		}else{
			printf("\nPhilosopher %d could not eat.", philosopherNumber);
		}
		replaceFork(philosopherNumber);
		if (go == 1)
		printf ("\nPhilosopher %d finished eating ",philosopherNumber);
	}
}

int main(){
	int i;
	for(i=1;i<=N;i++){
		pthread_mutex_init(&chopstick[i],NULL);
	}
	for(i=1;i<=N;i++){
		pthread_create(&philosopher[i],NULL,(void *)philospher,(int *)i);
	}
	for(i=1;i<=N;i++){
		pthread_join(philosopher[i],NULL);
	}
	for(i=1;i<=N;i++){
		pthread_mutex_destroy(&chopstick[i]);
	}
	return 0;
}