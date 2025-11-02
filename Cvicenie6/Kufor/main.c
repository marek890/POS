#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORKERS_AND_SUITCASES 10
#define MAX_ATTEMPTS 10000
#define DIGITS 4

void try_to_open_suitcase(char* filePath) {
	FILE* f = fopen(filePath, "r");	
	char line[9];
	int attempts = 0;
	char code[9];
	fgets(line, sizeof line, f);
	line[strcspn(line, "\n")] = 0;
	for (int i = 0; i < MAX_ATTEMPTS; i++) {
		for (int j = 0; j < DIGITS; j++) {
			code[j*2] = '0' + rand() % 10;
			code[j*2+1] = ' '; 
		}
		code[DIGITS*2-1] = '\0';
		attempts++;
		if (strcmp(line, code) == 0) {	
			printf("Worker has successfully opened the suitcase with code: %s on the %d attempt\n", code, attempts);
			break;
		}
	}
	fclose(f);
	//pthread_exit(NULL);
}

int main(int argc, char** argv) {
	if (atoi(argv[1]) > MAX_WORKERS_AND_SUITCASES) {
		fprintf(stderr, "Too many workers/suitcases\n");
		return 1;
	}

	srand(time(NULL));
	try_to_open_suitcase(argv[2]);

	return 0;
}


