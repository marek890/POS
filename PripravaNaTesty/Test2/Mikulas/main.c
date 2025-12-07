#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

typedef struct {
	int type;       
	_Bool isBroken; 
} present_t;

typedef struct {
	present_t *bag;    
	int capacity;      

	int in, out;       
	int count;         

	int *kids;         
	int kidsCount;

	int kidsWithPresents;
	int totalGiven;   

	int elfCount;     

	_Bool finished;   

	pthread_mutex_t mutex;
	sem_t space;      
	sem_t items;      
} bag_t;

typedef struct {
	bag_t *bag;
	int type;          
	unsigned int seed; 
} elf_arg_t;

void *elf_thread(void *arg) {
	elf_arg_t *ea = (elf_arg_t*)arg;
	bag_t *bag = ea->bag;

	while (1) {
		pthread_mutex_lock(&bag->mutex);
		if (bag->finished) {
			pthread_mutex_unlock(&bag->mutex);
			break;
		}
		pthread_mutex_unlock(&bag->mutex);

		present_t p;
		p.type = ea->type;
		p.isBroken = (rand_r(&ea->seed) % 4 == 0); 

		if (sem_wait(&bag->space) != 0) {
			break;
		}

		pthread_mutex_lock(&bag->mutex);
		if (bag->finished) {
			pthread_mutex_unlock(&bag->mutex);
			sem_post(&bag->space);
			break;
		}

		bag->bag[bag->in] = p;
		bag->in = (bag->in + 1) % bag->capacity;
		bag->count++;

		pthread_mutex_unlock(&bag->mutex);
		sem_post(&bag->items);
	}

	return NULL;
}

void *santa_thread(void *arg) {
	bag_t *bag = (bag_t*)arg;

	for (int kid = 0; kid < bag->kidsCount; kid++) {
		int lastType = -1;
		while (1) {
			sem_wait(&bag->items);

			pthread_mutex_lock(&bag->mutex);
			present_t p = bag->bag[bag->out];
			bag->out = (bag->out + 1) % bag->capacity;
			bag->count--;
			pthread_mutex_unlock(&bag->mutex);

			sem_post(&bag->space);

			if (!p.isBroken) {
				bag->kids[kid]++;
				bag->totalGiven++;
				if (p.type == lastType) {
					break;
				}
				lastType = p.type;
			} else {
			}
		}

		pthread_mutex_lock(&bag->mutex);
		bag->kidsWithPresents++;
		pthread_mutex_unlock(&bag->mutex);
	}

	pthread_mutex_lock(&bag->mutex);
	bag->finished = 1;
	pthread_mutex_unlock(&bag->mutex);

	for (int i = 0; i < bag->elfCount; i++) {
		sem_post(&bag->space);
	}

	return NULL;
}

int main(int argc, char **argv) {
	if (argc != 4) {
		fprintf(stderr, "Pouzitie: %s <elfCount> <k_capacity> <kidsCount>\n", argv[0]);
		return 1;
	}

	int elfCount = atoi(argv[1]);
	int k_capacity = atoi(argv[2]);
	int kidsCount = atoi(argv[3]);

	if (elfCount <= 0 || k_capacity <= 0 || kidsCount <= 0) {
		fprintf(stderr, "Argumenty musia byt kladne cele cisla.\n");
		return 1;
	}

	bag_t bag;
	bag.capacity = k_capacity;
	bag.bag = malloc(sizeof(present_t) * bag.capacity);
	if (!bag.bag) { perror("malloc bag"); return 1; }
	bag.in = bag.out = 0;
	bag.count = 0;
	bag.kidsCount = kidsCount;
	bag.kids = malloc(sizeof(int) * bag.kidsCount);
	if (!bag.kids) { perror("malloc kids"); free(bag.bag); return 1; }
	for (int i = 0; i < bag.kidsCount; ++i) bag.kids[i] = 0;
	bag.kidsWithPresents = 0;
	bag.totalGiven = 0;
	bag.elfCount = elfCount;
	bag.finished = 0;

	pthread_mutex_init(&bag.mutex, NULL);
	sem_init(&bag.space, 0, bag.capacity);
	sem_init(&bag.items, 0, 0);

	pthread_t *elf_threads = malloc(sizeof(pthread_t) * elfCount);
	elf_arg_t *elf_args = malloc(sizeof(elf_arg_t) * elfCount);
	if (!elf_threads || !elf_args) {
		perror("malloc threads");
		free(bag.bag);
		free(bag.kids);
		return 1;
	}

	srand((unsigned)time(NULL));

	for (int i = 0; i < elfCount; ++i) {
		elf_args[i].bag = &bag;
		elf_args[i].type = rand() % 3;                  /* kazdy elf ma pevny typ pri vytvoreni */
		elf_args[i].seed = (unsigned int)rand() ^ (unsigned int)(uintptr_t)i;
		if (pthread_create(&elf_threads[i], NULL, elf_thread, &elf_args[i]) != 0) {
			perror("pthread_create elf");
			/* jednoduchy cleanup - nie idealny ale dostacujuci pre skolsku ulohu */
			for (int j = 0; j < i; ++j) pthread_cancel(elf_threads[j]);
			free(elf_threads); free(elf_args);
			free(bag.bag); free(bag.kids);
			return 1;
		}
	}

	pthread_t santa;
	if (pthread_create(&santa, NULL, santa_thread, &bag) != 0) {
		perror("pthread_create santa");
		for (int i = 0; i < elfCount; ++i) pthread_cancel(elf_threads[i]);
		free(elf_threads); free(elf_args);
		free(bag.bag); free(bag.kids);
		return 1;
	}

	pthread_join(santa, NULL);

	for (int i = 0; i < elfCount; ++i) {
		pthread_join(elf_threads[i], NULL);
	}

	double avg = 0.0;
	if (bag.kidsCount > 0) avg = (double)bag.totalGiven / (double)bag.kidsCount;
	printf("Priemerne darcekov na dieta: %.2f\n", avg);

	free(elf_threads);
	free(elf_args);
	free(bag.bag);
	free(bag.kids);

	pthread_mutex_destroy(&bag.mutex);
	sem_destroy(&bag.space);
	sem_destroy(&bag.items);

	return 0;
}
