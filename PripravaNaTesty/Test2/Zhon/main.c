#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// 5 minut = 1 sekunda v programe
// 60 minut = 12 sekund v programe
// 4 hodiny = 4 * 12 = 48
#define OPEN_TIME 48
#define WORKERS 1

typedef struct {
	_Bool taken;
} present_t;

typedef struct {
	present_t* presents;
	pthread_mutex_t mutex;
	int count;
	int capacity;
	int in, out;
	int timer;
	int sold;
	sem_t space;
	sem_t items;
} regal_t;

void* countdown(void* arg) {
	regal_t* regal = (regal_t*)arg;
	while (regal->timer != 0) {
		regal->timer--;
		sleep(1);
	}
	return NULL;
}

void* put_on_regal(void* arg) {
	regal_t* regal = (regal_t*)arg;

	while (regal->timer != 0) {
		if ((double)regal->count < (double) regal->capacity/3) {
			sleep(1);
			present_t p1, p2;
			p1.taken = 0;
			p2.taken = 0;

			sem_wait(&regal->space);
			pthread_mutex_lock(&regal->mutex);
			regal->presents[regal->in] = p1;
			regal->in = (regal->in + 1) % regal->capacity;
			regal->count++;
			printf("Pracovnik dolozil darcek..\n");
			sem_post(&regal->items);
			if ((double)regal->count > (double) (regal->capacity*2)/3) {
				pthread_mutex_unlock(&regal->mutex);
				continue;
			}
		
			regal->presents[regal->in] = p2;
			regal->in = (regal->in + 1) % regal->capacity;
			regal->count++;
			printf("Pracovnik dolozil darcek..\n");
			sem_post(&regal->items);
			pthread_mutex_unlock(&regal->mutex);
		}

	}

	return NULL;
}

void* take_present(void* arg) {
	regal_t* regal = (regal_t*)arg;
	while (regal->timer != 0) {
		sleep((rand() % 2) + 2); // kazdych 10-15 min
		sem_wait(&regal->items);
		pthread_mutex_lock(&regal->mutex);
		int takesPresents = (rand() % 5) + 1;
		if (takesPresents > regal->count) {
			takesPresents = regal->count;
			printf("Zakaznik je nespokojny a berie vsetky darceky!\n");
		}
		for (int i = 0; i < takesPresents - 1; i++)
			sem_wait(&regal->items);

		for (int i = 0; i < takesPresents; i++) {
			regal->presents[regal->out].taken = 1;
			regal->out = (regal->out + 1) % regal->capacity;
			regal->count--;
			regal->sold++;
			sem_post(&regal->space);
		}
		pthread_mutex_unlock(&regal->mutex);
		printf("Zakaznik zobral %d darcek(y)(ov)\n", takesPresents);
	}

	return NULL;
}


int main (int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Bol zadany nespravny pocet argumentov\n");
		return -1;
	}
	srand(time(NULL));
	int k = atoi(argv[1]);
	regal_t regal;
	regal.count = 0;
	regal.capacity = k;
	regal.in = 0;
	regal.out = 0;
	regal.timer = OPEN_TIME;
	
	pthread_mutex_init(&regal.mutex, NULL);
	sem_init(&regal.space, 0, k/2);
	sem_init(&regal.items, 0, k/2);
	regal.presents = malloc(sizeof(present_t) * k);

	for (int i = 0; i < k/2; i++) {
		present_t p;
		p.taken = 0;
		regal.presents[i] = p;
		regal.count++;
		regal.in = (regal.in + 1) % regal.capacity;
	}

	pthread_t customer;
	pthread_t timer;
	pthread_t workers[WORKERS];

	for (int i = 0; i < WORKERS; i++) {
		pthread_create(&workers[i], NULL, put_on_regal, &regal);
	}
	pthread_create(&customer, NULL, take_present, &regal);	
	pthread_create(&timer, NULL, countdown, &regal);

	for (int i = 0; i < WORKERS; i++) {
		pthread_join(workers[i], NULL);
	}
	pthread_join(customer, NULL);
	pthread_join(timer, NULL);

	printf("Celkovo sa predalo %d darcekov\n", regal.sold);

	free(regal.presents);
	pthread_mutex_destroy(&regal.mutex);
	sem_destroy(&regal.space);
	sem_destroy(&regal.items);

	return 0;
}
