#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define MAX_THREADS 6

typedef struct Train {
	char* name;
	char* type;
	int delay;
} train_t;

typedef struct TrainTypes {
	train_t* trains;
	int count;
	int delayAvg;
} train_types_t;


int compare_train_type (const void* a, const void* b) {
	const train_t *t1 = (const train_t*) a;
	const train_t *t2 = (const train_t*) b;

	return strcmp(t1->type, t2->type);
}

int compare_train_delay (const void* a, const void* b) {
	const train_t *t1 = (const train_t*) a;
	const train_t *t2 = (const train_t*) b;
	if (t1->delay < t2->delay) return -1;
	if (t1->delay > t2->delay) return 1;
	return 0;
}



void* simulate_train_delay(void * arg) {
	train_types_t* group = (train_types_t*) arg;
	int delaySum = 0;

	for (int i = 0; i < group->count; i++) {
		delaySum = delaySum + group->trains[i].delay;
	}
	group->delayAvg = delaySum / group->count;
	qsort(group->trains, group->count, sizeof(train_t), compare_train_delay);

	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	if (argc < 3 || argc > 3) {
		fprintf(stderr, "Zadali ste prilis vela alebo prilis malo argumentov, pozadovany pocet je 2.\n");
		return 1;
	}

	pthread_t threads[MAX_THREADS];
	train_types_t types[MAX_THREADS];
	int numOfTrains = atoi(argv[1]);
	train_t* trains = malloc(sizeof(train_t) * numOfTrains);
	FILE* f = fopen(argv[2], "r");
	char* line = NULL;
	size_t len = 0;
	int i = 0;
	int j = 0;

	while(getline(&line, &len, f) != -1) {
		char *token = strtok(line, ";");
    	while (token != NULL) {
        	if (j == 0) {
				trains[i].name = strdup(token);
			} else if (j == 1) {
				trains[i].type = strdup(token);
			} else {
				trains[i].delay = atoi(token);
			}
        	token = strtok(NULL, ";");
			j++;
   		}
		i++;
		j = 0;
	}
	free(line);
	fclose(f);

	qsort(trains, numOfTrains, sizeof(train_t), compare_train_type);
	
	for (int t = 0; t < MAX_THREADS; t++) {
        types[t].trains = NULL;
        types[t].count = 0;
    }

	int typeCount = 0;
	types[typeCount].trains = &trains[0];
	types[typeCount].count = 1;
	for (int i = 1; i < numOfTrains; i++) {
		if (strcmp(trains[i - 1].type, trains[i].type) != 0) {
			typeCount++;
			types[typeCount].trains = &trains[i];
		}
		types[typeCount].count++;
	}
	
	for (int i = 0; i < MAX_THREADS; i++) {
		pthread_create(&threads[i], NULL, simulate_train_delay, &types[i]);
	}
	
	for (int i = 0; i < MAX_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	for (int i = 0; i < MAX_THREADS; i++) {
		printf("Typ vlakov: %s\n", types[i].trains[0].type);
		printf("   Najmensie meskanie mal vlak %s s casom %d minut\n", types[i].trains[0].name, types[i].trains[0].delay);
		printf("   Najvacsie meskanie mal vlak %s s casom %d minut\n", types[i].trains[types[i].count - 1].name, types[i].trains[types[i].count - 1].delay);
		printf("   Priemerne meskanie bolo %d minut\n", types[i].delayAvg);
	}
	

	for (int i = 0; i < numOfTrains; i++) {
		free(trains[i].name);
		free(trains[i].type);
	}

	free(trains);
	return 0;
}
