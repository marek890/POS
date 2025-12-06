#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	char* line = NULL;
	size_t length = 0;
	FILE* f = fopen("data.txt", "r");
	getline(&line, &length, f);
	printf("%s", line);
	int count = 0;
	while(getline(&line, &length,  f) != -1) {
		char* token = strtok(line, " \n");
        while (token != NULL) {
            printf("%s", token);     // vytlačí číslo
            token = strtok(NULL, " \n");

            if (token != NULL)       // medzera medzi číslami, NIE za posledným
                printf(" ");
        }
        printf("\n"); 
	}
	free(line);
	fclose(f);

	return 0;
}
