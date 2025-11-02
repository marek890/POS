#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define MAX_THREADS 4
#define MAX_SIMULATIONS 25000

typedef struct TriatlonTimes {
	int swimming;
	int cycling;
	int running;
	int total;
} triatlon_times_t;

int compare_times(const void* a, const void* b) {
	triatlon_times_t* first = (triatlon_times_t*)a;
	triatlon_times_t* second = (triatlon_times_t*)b;

	if (first->total < second->total) return -1;
	if (first->total > second->total) return 1;
	return 0;
}

void* simulate_triatlon(void* arg) {
	triatlon_times_t* times = (triatlon_times_t*) arg;
	triatlon_times_t t;
	//unsigned int seed = (unsigned int) time(NULL) ^ pthread_self();

	for (int i = 0; i < MAX_SIMULATIONS; i++) {
		t.swimming = 12;
		if (/*rand_r(&seed)*/ rand() % 101 < 25) {
			t.swimming = t.swimming + 3;
		}
		t.cycling = 40;
		t.running = (/*rand_r(&seed)*/ rand() % 11) + 20;
		t.total = t.swimming + t.cycling + t.running;

		times[i] = t;
	}
	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	pthread_t threads[MAX_THREADS];
	triatlon_times_t* times = malloc(sizeof(triatlon_times_t) * MAX_SIMULATIONS * MAX_THREADS);
	int sumTime = 0;
	int avgTime = 0;
	srand(time(NULL));

	for (int i = 0; i < MAX_THREADS; i++) {
		pthread_create(&threads[i], NULL, simulate_triatlon, &times[i*MAX_SIMULATIONS]);
	}

	for (int i = 0; i < MAX_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	qsort(times, MAX_THREADS * MAX_SIMULATIONS, sizeof(triatlon_times_t), compare_times);
	
	for (int i = 0; i < MAX_THREADS * MAX_SIMULATIONS; i++) {
		sumTime = sumTime + times[i].total; 
	}
	avgTime = sumTime / (MAX_THREADS * MAX_SIMULATIONS);


	printf("Najlepsie 3 casy:\n");
	for (int i = 0; i < 3; i++) {
		printf("   %d. Celkovy cas: %d, Cas plavania: %d, Cas bicyklovania: %d, Cas behu: %d\n",(i + 1), times[i].total, times[i].swimming, times[i].cycling, times[i].running);
	}

	printf("Ocakavany priemerny cas: %d\n", avgTime);
	if (avgTime < 77)
		printf("Ocakava sa pravdepodobne vitazstvo na sutazi\n");

	free(times);

	return 0;
}




