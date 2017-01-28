/*
 ============================================================================
 Name        : FuelSem.c
 Author      : Ludovico de Nittis
 Version     :
 Copyright   : GPLv3
 Description : FuelSem in C, Ansi-style
 ============================================================================
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <semaphore.h>
#include <errno.h>

#define M 12  // Car number
#define N 5  // Tank number prepared by Fillmore


sem_t mutex;
sem_t fillmore;
sem_t macchina;

int tank = N;
int i, j, ii = 0;


void *prepare(void *a) {
	// Fill the tanks
	while(ii < (M/N)){
		sem_wait(&fillmore);
		sem_wait(&mutex);  // Acquire the lock
		if (tank != 0) printf("(P)You called me for nothing, tanks are not out of stock! ERROR!\n");
		tank = N;
		printf("(P)Fuel is ready!\n");
		ii++;
		for (j=0; j<N; j++){
			printf("(P)Unlock a car\n");
			sem_post(&macchina);
		}
		printf("(P)I go to sleep\n");
		sem_post(&mutex);  // Release the lock
	}
	printf("(P)I'm out of business!\n");
	pthread_exit(0);
}


void *take(void *b) {
	// Take a tank
	printf("(C)I'm waiting a tank\n");
	sem_wait(&macchina);
	sem_wait(&mutex);  // Acquire the lock
	if (tank == 0) printf("(C)Tank out of stock, I came here for nothing. ERROR!\n");
	printf("(C)Thank you for the fuel!\n");
	tank --;
	if (tank == 0){
		printf("(C)I took the last tank, I call Fillmore\n");
		sem_post(&fillmore);
	}
	sem_post(&mutex);  // Release the lock
	printf("(C)See you next time!\n");
	pthread_exit(0);
}


int main(void) {

	// Semaphores are initialized
	sem_init(&fillmore, 0, 0);
	sem_init(&macchina, 0, N);  // At the beginning the tanks are in stock
	sem_init(&mutex, 0, 1);


	// Threads are created
	pthread_t tid[M+1];
	if (pthread_create(&tid[0],NULL,prepare,NULL)) perror("Error create");
	for(i=1; i < (M+1); i++){
		if (pthread_create(&tid[i],NULL,take,NULL)) perror("Error create");
	}


	// Wait for the threads
	for (i=0;i<(M+1);i++)
	if (pthread_join(tid[i], NULL))  perror("Error Join");

	sem_destroy(&fillmore);
	sem_destroy(&macchina);
	sem_destroy(&mutex);


	printf("Everything went smoothly!\n");
	return 0;
}
