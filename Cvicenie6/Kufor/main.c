#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORKERS_AND_SUITCASES 10
#define MAX_ATTEMPTS 10000
#define DIGITS 4

void* try_to_open_suitcase(void* arg) {
	char* line = (char*)arg;
	int attempts = 0;
	char code[8];
	memset(code, 0, sizeof(code));

	for (int i = 0; i < MAX_ATTEMPTS; i++) {
		for (int j = 0; j < DIGITS; j++) {
			code[j*2] = '0' + rand() % 10;
			code[j*2+1] = ' '; 
		}
		code[DIGITS*2-1] = '\0';
		attempts++;
		if (strcmp(line, code) == 0) {	
			break;
		}
	}
	if (strcmp(line, code) != 0) {
		printf("Worker wasnt succesful at opening the suitcase, the code was: %s\n", line);
	}
	else {
		printf("Worker has successfully opened the suitcase with code: %s on the %d attempt\n", line, attempts);
	}

	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	if (atoi(argv[1]) > MAX_WORKERS_AND_SUITCASES) {
		fprintf(stderr, "Too many workers/suitcases\n");
		return 1;
	}
	pthread_t threads[atoi(argv[1])];
	char* codes[atoi(argv[1])];
	srand(time(NULL));
	
	FILE* f = fopen(argv[2], "r");
	for (int i = 0; i < atoi(argv[1]); i++) {
		codes[i] = malloc(9);
		fgets(codes[i], 9, f);
    	codes[i][strcspn(codes[i], "\n")] = '\0';
		pthread_create(&threads[i], NULL, try_to_open_suitcase, codes[i]);
	}
	fclose(f);
	
	for (int i = 0; i < atoi(argv[1]); i++) {
		pthread_join(threads[i], NULL);
		free(codes[i]);
	}

	return 0;
}


