#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define N 5
pthread_t philosopher[N];
pthread_mutex_t chopstick[N];
pthread_cond_t cond_vars[N];

void replaceFork(int philosopherNumber){
	pthread_mutex_unlock(&chopstick[philosopherNumber]);
	pthread_mutex_unlock(&chopstick[(philosopherNumber+1)%N]);
}

void takeFork(int philosopherNumber){
	pthread_mutex_lock(&chopstick[philosopherNumber]);
	pthread_mutex_lock(&chopstick[(philosopherNumber+1)%N]);
}

void *philospher(int philosopherNumber){
	while(1){
		int r1 = (rand() % 5) + 1;
		int r2 = (rand() % 5) + 1;
		printf ("\nPhilosopher %d is thinking for %d second", philosopherNumber, r1);
		sleep(r1);
		takeFork(philosopherNumber);
		printf ("\nPhilosopher %d is eating for %d seconds", philosopherNumber, r2);
		sleep(r2);
		replaceFork(philosopherNumber);
		printf ("\nPhilosopher %d finished eating ",philosopherNumber);
	}
}

int main(){
	int i;
	for(i=1;i<=N;i++){
		pthread_mutex_init(&chopstick[i],NULL);
		pthread_cond_init(&cond_vars[i],NULL);
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