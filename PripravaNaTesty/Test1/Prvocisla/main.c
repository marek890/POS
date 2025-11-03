#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define MIN_ELEMENTS 3470
#define THREADS 5

typedef struct ThreadData {
	int* data;
	int start;
	int end;
	int count;
} thread_data_t;

void* count_prime_numbers(void* arg) {
	thread_data_t* threadData = (thread_data_t*) arg;

	for (int i = threadData->start; i < threadData->end; i++) {
		if (threadData->data[i] <= 1) continue;

		int isPrime = 1;

		for (int j = 2; j * j <= threadData->data[i]; j++) {
			if (threadData->data[i] % j == 0) {
				isPrime = 0;
				break;
			}
		}
		if (isPrime) threadData->count++;
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	int numOfElements = atoi(argv[1]);
	if (numOfElements < MIN_ELEMENTS) {
		fprintf(stderr, "Bol zadany maly pocet prvkov.\n");
		return 1;
	}
	srand(time(NULL));
	int* elements = malloc(sizeof(int) * numOfElements);
	pthread_t threads[THREADS];
	thread_data_t threadData[THREADS];
	int eleOneThreadCount = numOfElements / THREADS;

	for (int i = 0; i < THREADS; i++) {
		threadData[i].data = elements;
		threadData[i].start = 0 + (i * eleOneThreadCount);
		threadData[i].end = (i == THREADS - 1) 
                        ? numOfElements            // posledné vlákno ide až do konca
                        : (i + 1) * eleOneThreadCount;
		threadData[i].count = 0;
	}

	for (int i = 0; i < numOfElements; i++) {
		elements[i] = (rand() % 211) + 1;
	}

	for (int i = 0; i < THREADS; i++) {
		pthread_create(&threads[i], NULL, count_prime_numbers, &threadData[i]);
	}

	for (int i = 0; i < THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	int finalCount = 0;
	for (int i = 0; i < THREADS; i++) {
		finalCount = finalCount + threadData[i].count;
	}
	double percentage = (double) finalCount / numOfElements * 100;

	printf("Pocet najdenych prvocisel je: %d\nZlomok: %d / %d\nPercetualne vyjadrenie: %.2f%\n", finalCount, finalCount, numOfElements, percentage);
	
	free(elements);
	return 0;
}
