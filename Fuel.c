/*
 ============================================================================
 Name        : Fuel.c
 Author      : Ludovico de Nittis
 Version     :
 Copyright   : GPLv3
 Description : Fuel in C, Ansi-style
 ============================================================================
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <errno.h>

#define M 15  // Car number
#define N 4  // Tank number prepared by Fillmore


pthread_mutex_t mutex;
pthread_cond_t fillmore;
pthread_cond_t macchina;

int tank = N;
int i, j, ii = 0;


void *prepare(void *a) {
	// Fill the tanks
	while(ii < (M/N)){
		pthread_mutex_lock(&mutex);  // Acquire the lock
		if (tank != 0) printf("(P)I'm resting\n");
		while (tank != 0){
			pthread_cond_wait(&fillmore, &mutex);
		}
		tank = N;
		printf("(P)Fuel is ready!\n");
		ii++;
		for (j=0; j<N; j++){
			printf("(P)Unlock a car\n");
			pthread_cond_signal(&macchina);
		}
		printf("(P)I go to sleep\n");
		pthread_mutex_unlock(&mutex);  // Release the lock
	}
	printf("(P)I'm out of business!\n");
	pthread_exit(0);
}


void *take(void *b) {
	// Take a tank
	pthread_mutex_lock(&mutex);  // Acquire the lock
	while (tank == 0){
		printf("(C)I'm waiting a tank\n");
		pthread_cond_wait(&macchina, &mutex);
	}
	printf("(C)Thank you for the fuel!\n");
	tank --;
	if (tank == 0){
		printf("(C)I took the last tank, I call Fillmore\n");
		pthread_cond_signal(&fillmore);
	}
	pthread_mutex_unlock(&mutex);  // Release the lock
	printf("(C)See you next time!\n");
	pthread_exit(0);
}


int main(void) {

	// Mutex are initialized
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&fillmore, NULL);
	pthread_cond_init(&macchina, NULL);


	// Threads are created
	pthread_t tid[M+1];
	if (pthread_create(&tid[0],NULL,prepare,NULL)) perror("Error create");
	for(i=1; i < (M+1); i++){
		if (pthread_create(&tid[i],NULL,take,NULL)) perror("Error create");
	}


	// Wait for the threads
	for (i=0;i<(M+1);i++)
	if (pthread_join(tid[i], NULL))  perror("Error Join");


	printf("Everything went smoothly!\n");
	return 0;
}
