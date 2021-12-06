#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>

#define NB_TRUCKS 5
#define NB_CARS 5
#define NB_VEHICULES (NB_TRUCKS+ NB_CARS)


sem_t sem_compteur;
pthread_mutex_t camions_sc;



void passPont(int tonnes);
int randomGenerating(double max);
void accessToPont(int tonnes);



// wait function
void waitFor(double max)
{
    struct timespec delai;
    delai.tv_sec = randomGenerating(max);
    delai.tv_nsec = 0;
    nanosleep(&delai, NULL);
}

//return int between 1 and max*RAND_MAX/(RAND_MAX+1)

int randomGenerating(double max)
{
    int j = (int)(max * rand() / (RAND_MAX + 1.0));
    if (j < 1)
        return 1;
    return j;
}

void *car(void *args)
{
    int pid = *((int *)args);
    waitFor(5.0);
    accessToPont(5);
    printf("Car %d is passing the pont \n", pid);
    waitFor(5.0);
    printf("Car %d has passed the pont\n", pid);
    passPont(5);
    pthread_exit(NULL);
}

void *truck(void *args)
{
    int pid = *((int *)args);
    waitFor(5.0);
    accessToPont(15);
    printf("Truck %d is passing the pont \n", pid);
    waitFor(5.0);
    printf("Truck %d has passed the pont\n", pid);
    passPont(15);
    pthread_exit(NULL);
}


void accessToPont(int tonnes)
{
    if (tonnes == 15)
    {
        pthread_mutex_lock(&camions_sc);
        while (tonnes > 0)
        {
            sem_wait(&sem_compteur);
            tonnes -= 5;
        }
        pthread_mutex_unlock(&camions_sc);
    }
    else sem_wait(&sem_compteur);
}

void passPont(int tonnes)
{
    while (tonnes > 0)
    {
        sem_post(&sem_compteur);
        tonnes -= 5;
    }
}

int main()
{
    int i;
    pthread_t id;
    sem_init(&sem_compteur, 0, 3);
    for (i = 0; i < NB_VEHICULES; i++)
    {
        int *j = (int *)malloc(sizeof(int));
        *j = i;
        if (i < NB_TRUCKS)
            pthread_create(&id, NULL, truck, j);
        else
            pthread_create(&id, NULL, car, j);
    }
    pthread_exit(NULL);
}