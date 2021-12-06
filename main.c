#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>
#include <math.h>

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

int current_load = 0;

int random_spin(double max)
{
    int j = (int)(max * rand() / (RAND_MAX + 1.0));
    return ceil(j);
}

void random_sleep(double max)
{
    struct timespec delay;
    delay.tv_sec = random_spin(max);
    delay.tv_nsec = 0;
    nanosleep(&delay, NULL);
}

void acces_to_bridge(int tons, int id)
{
    pthread_mutex_lock(&sc);
    if (current_load + tons <= 15)
    {
        current_load += tons;
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
    current_load -= tonnes;

    for (i = 0; i < NB_TRUCKS; i++)
    {
        if ((state[pid] == WAIT) && (current_load == 0))
        {
            sem_post(&sempriv[i]);
            current_load = 15;
            number_of_blocked_trucks--;
        }
    }
    for (i = NB_TRUCKS; i < NB_VEHICULES; i++)
    {
        if ((current_load < 15) && (number_of_blocked_trucks == 0) && (state[i] = WAIT))
        {
            current_load += 15;
            sem_post(&sempriv[i]);
        }
    }
    pthread_mutex_unlock(&sc);
}

void *truck(void *args)
{
    int pid = *((int *)args);
    random_sleep(5.0);
    acces_to_bridge(15, pid);
    printf("Truck %d traversing the bridge \n", pid);
    random_sleep(5.0);
    printf("Truck %d leaving the bridge\n", pid);
    liberate_bridge(15, pid);
    pthread_exit(NULL);
}

void *car(void *args)
{
    int pid = *((int *)args);
    random_sleep(5.0);
    acces_to_bridge(5, pid);
    printf("Car %d traversing the bridge \n", pid);
    random_sleep(5.0);
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

    for (int i = 0; i < NB_TRUCKS; i++)
    {
        int j = i;
        pthread_create(&id, NULL, truck, &j);
    }

    for (int i = 0; i < NB_CARS; i++)
    {
        int j = i;
        pthread_create(&id, NULL, car, &j);
    }

    pthread_exit(NULL);
    return 0;
}