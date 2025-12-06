#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define SIMULATIONS 1000

typedef struct Station {
	int* sims;
	int id;
	int burnt;
	int oneStationSims;
} station_t;

void* simulate_station(void* arg) {
	station_t* stations = (station_t*) arg;
		
	for (int i = stations->id * stations->oneStationSims; i < stations->id * stations->oneStationSims + stations->oneStationSims; i++)
	{
		stations->sims[i] = 12;
		if ((rand() % 101) < 21) 
			stations->sims[i] = stations->sims[i] + 3;	
		if ((rand() % 101) < 11) 
			stations->sims[i] = stations->sims[i] - 1;	
		if ((rand() % 101) < 10) 
			stations->burnt++;
	}
	pthread_exit(NULL);
}

int compare_times(const void* a, const void *b) {
	const int* t1 = (const int*) a;
	const int* t2 = (const int*) b;

	if (*t1 > *t2) return -1;
	if (*t1 < *t2) return 1;
	return 0;
}

int main (int argc, char** argv) {
	int numOfStations = atoi(argv[1]);
	if (numOfStations < 2 || numOfStations > 6) {
		fprintf(stderr, "Bol zadany nespravny pocet pracovnych stanic, musia byt z intervalu <2,6>\n");
		return 1;
	}

	int* simulations = malloc(sizeof(int) * SIMULATIONS);
	pthread_t threads[SIMULATIONS];
	station_t stations[SIMULATIONS];
	srand(time(NULL));

	for (int i = 0; i < numOfStations; i++) {
		stations[i].sims = simulations;
		stations[i].id = i;
		stations[i].burnt = 0;
		stations[i].oneStationSims = SIMULATIONS / numOfStations;
	}

	for (int i = 0; i < numOfStations; i++) {
		pthread_create(&threads[i], NULL, simulate_station, &stations[i]);
	}

	for (int i = 0; i < numOfStations; i++) {
		pthread_join(threads[i], NULL);	
	}
	
	int sum = 0;
	int effectivityCount = 0;
	int capacity = 0;

	for (int i = 0; i < SIMULATIONS; i++) {
		sum = sum + simulations[i];
		if (simulations[i] < 13)
			effectivityCount++;
		if (sum <= 60)
			capacity++;
	}
	
	int totalBurnt = 0;

	for (int i = 0; i < numOfStations; i++) {
		totalBurnt = totalBurnt + stations[i].burnt;
	}

	double avg = (double)sum / SIMULATIONS;
	double percBurnt = (double)totalBurnt / SIMULATIONS * 100;
	double effectivity = (double)effectivityCount / sum * 100;
	qsort(simulations, SIMULATIONS, sizeof(int), compare_times);

	printf("Priemerny cas bol: %.2f minut\n", avg);
	printf("Najrychlejsi cas bol: %d minut\n", simulations[SIMULATIONS - 1]);
	printf("Najpomalsi cas bol: %d minut\n", simulations[0]);
	printf("Efektivita pizzerie: %.2f %\n", effectivity);
	printf("Kapacita pizzerie: %d\n", capacity);
	
	if (avg < 12.5 && percBurnt < 10) {
		printf("Pizzeria dosahuje vybornu efektivitu\n");
	}

	free(simulations);
	return 0;
}
