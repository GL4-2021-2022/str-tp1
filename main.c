#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>

#define NB_TRUCKS 5
#define NB_CARS 5
#define NB_VEHICULES (NB_TRUCKS + NB_CARS)

#define WAIT 1
#define TRAVERSE 3
#define IDLE 2

pthread_mutex_t sc;

sem_t sempriv[NB_VEHICULES];

int state[NB_VEHICULES];

int number_of_blocked_trucks = 0;

int limit = 0;

int random_spin(double max)
{
    int j = (int)(max * rand() / (RAND_MAX + 1.0));
    if (j < 1)
        return 1;
    return j;
}

void waiting(double max)
{
    struct timespec delay;
    delay.tv_sec = random_spin(max);
    delay.tv_nsec = 0;
    nanosleep(&delay, NULL);
}

void acces_to_bridge(int tons, int id)
{
    pthread_mutex_lock(&sc);
    if (limit + tons)
    {
        limit += tons;
        state[id] = TRAVERSE;
        sem_post(&sempriv[id]);
    }
    else
    {
        state[id] = WAIT;
        if (tons == 15)
            number_of_blocked_trucks++;
    }
    pthread_mutex_unlock(&sc);
    sem_wait(&sempriv[id]);
}

void liberate_bridge(int tonnes, int pid)
{
    int i;
    pthread_mutex_lock(&sc);
    state[pid] = IDLE;
    limit -= tonnes;

    for (i = 0; i < NB_TRUCKS; i++)
    {
        if ((state[pid] == WAIT) && (limit == 0))
        {
            sem_post(&sempriv[i]);
            limit = 15;
            number_of_blocked_trucks--;
        }
    }
    for (i = NB_TRUCKS; i < NB_VEHICULES; i++)
    {
        if ((limit < 15) && (number_of_blocked_trucks == 0) && (state[i] = WAIT))
        {
            limit += 15;
            sem_post(&sempriv[i]);
        }
    }
    pthread_mutex_unlock(&sc);
}

void *truck(void *args)
{
    int pid = *((int *)args);
    waiting(5.0);
    acces_to_bridge(15, pid);
    printf("Truck %d traversing the bridge \n", pid);
    waiting(5.0);
    printf("Truck %d leaving the bridge\n", pid);
    liberate_bridge(15, pid);
    pthread_exit(NULL);
}

void *car(void *args)
{
    int pid = *((int *)args);
    waiting(5.0);
    acces_to_bridge(5, pid);
    printf("Car %d traversing the bridge \n", pid);
    waiting(5.0);
    printf("Car %d leaving the bridge\n", pid);
    liberate_bridge(5, pid);
    pthread_exit(NULL);
}

int main()
{
    int i;
    pthread_t id;
    for (i = 0; i < NB_VEHICULES; i++)
    {
        state[i] = IDLE;
        sem_init(&sempriv[i], 0, 0);
    }
    pthread_mutex_init(&sc, 0);
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
    return 0;
}