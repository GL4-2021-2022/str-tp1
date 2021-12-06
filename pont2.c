#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>

#define NB_TRUCKS 5
#define NB_CARS 5
#define NB_VEHICULES (NB_CARS + NB_TRUCKS)

#define WAITING 1
#define IDLE 2
#define PASSING 3

pthread_mutex_t sc;
sem_t sempriv[NB_VEHICULES];
int etat[NB_VEHICULES];
int nb_blocked_trucks = 0;
int limit = 0;

void waitFor(double max);
int randomGenerating(double max);
void *car(void *args);
void *truck(void *args);
void accessToPont(int tonnes, int id);
void passPont(int tonnes, int pid);

void accessToPont(int tonnes, int id)
{
    printf("access tonnes=%d \n", tonnes);
    pthread_mutex_lock(&sc);
    if (limit + tonnes <= 15)
    {
        limit += tonnes;
        etat[id] = PASSING;

        pthread_mutex_unlock(&sc);
    }
    else
    {
        etat[id] = WAITING;
        if (tonnes == 15)
            nb_blocked_trucks++;
        pthread_mutex_unlock(&sc);
        sem_wait(&sempriv[id]);
    }
}

void passPont(int tonnes, int pid)
{
    int i;
    pthread_mutex_lock(&sc);

    etat[pid] = IDLE;
    limit -= tonnes;

    for (i = 0; i < NB_TRUCKS; i++)
    {
        if ((etat[i] == WAITING) && (limit == 0))
        {
            limit = 15;
            nb_blocked_trucks--;
            sem_post(&sempriv[i]);
        }
    }
    for (i = NB_TRUCKS; i < NB_VEHICULES; i++)
    {

        if ((limit < 15) && (nb_blocked_trucks == 0) && (etat[i] == WAITING))
        {

            limit += 5;
            sem_post(&sempriv[i]);
        }
    }
    pthread_mutex_unlock(&sc);
}

void waitFor(double max)
{
    struct timespec delai;
    delai.tv_sec = randomGenerating(max);
    delai.tv_nsec = 0;
    nanosleep(&delai, NULL);
}

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
    accessToPont(5, pid);
    printf("Car %d is passing the pont \n", pid);
    waitFor(5.0);
    printf("Car %d has passed the pont\n", pid);
    passPont(5, pid);
    pthread_exit(NULL);
}

void *truck(void *args)
{
    int pid = *((int *)args);
    waitFor(5.0);
    accessToPont(15, pid);
    printf("Truck %d is passing the pont\n", pid);
    waitFor(5.0);
    printf("Truck %d has passed the pont\n", pid);
    passPont(15, pid);
    pthread_exit(NULL);
}

int main()
{
    int i;
    pthread_t id;

    pthread_mutex_init(&sc, 0);

    for (i = 0; i < NB_VEHICULES; i++)
    {

        etat[i] = IDLE;
        sem_init(&sempriv[i], 0, 0);

        int *j = (int *)malloc(sizeof(int));
        *j = i;
        if (i < NB_TRUCKS)
            pthread_create(&id, NULL, truck, j);
        else
            pthread_create(&id, NULL, truck, j);
    }
    pthread_exit(NULL);
    return 0;
}