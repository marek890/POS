#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define APPROP 1
#define INAPPROP 0

typedef struct {
	_Bool isApprop;
	char* sentBy;
} message_t;

typedef struct {
	message_t* messages;
	int in, out;
	int capacity;
	int total;
	pthread_mutex_t mutex;
	sem_t space;
	sem_t items;
} message_stack_t;

typedef struct {
	message_stack_t* stack;
	char* name;
	int messageCount;
	int durationMin;
	int durationMax;
	int chance;
} user_t;

void* send_message(void* arg) {
	user_t* user = (user_t*)arg;
	message_stack_t* stack = user->stack;

	for (int i = 0; i < user->messageCount; i++) {
		/* čakanie na voľné miesto - rezervujeme slot */
		sem_wait(&stack->space);

		/* generovanie správy mimo kritickej sekcie (nemaj sleep v mutexe) */
		printf("Pouzivatel %s pise spravu...\n", user->name);
		sleep((rand() % user->durationMax) + user->durationMin);

		message_t mes;
		mes.sentBy = user->name;
		if (rand() % 100 < user->chance) {
			mes.isApprop = INAPPROP;
			printf("Pouzivatel %s napisal nevhodnu spravu...\n", user->name);
		} else {
			mes.isApprop = APPROP;
			printf("Pouzivatel %s napisal vhodnu spravu...\n", user->name);
		}

		/* zapísanie správy do buffru musí byť chránené mutexom */
		pthread_mutex_lock(&stack->mutex);
		stack->messages[stack->in] = mes;
		stack->in = (stack->in + 1) % stack->capacity;
		pthread_mutex_unlock(&stack->mutex);

		/* oznámime kontrolórovi, že je k dispozícii nová položka */
		sem_post(&stack->items);
	}

	return NULL;
}

void* controller_check(void* arg) {
	message_stack_t* stack = (message_stack_t*)arg;
	printf("Kontrolor sa pripravuje na kontrolu sprav...\n");
	sleep(5); // priprava
	printf("Kontrolor je pripraveny na kontrolu!\n");

	int inappropCountDudy = 0;
	int inappropCountCherry = 0;
	int checked = 0;

	while (checked < stack->total) {
		/* počkáme na aspoň jednu položku */
		sem_wait(&stack->items);

		/* vezmeme mutex a zistíme, koľko je momentálne dostupných položiek */
		pthread_mutex_lock(&stack->mutex);
		int available = (stack->in - stack->out + stack->capacity) % stack->capacity;
		if (available == 0) {
			/* teoreticky by sem_wait zabezpečil available>=1, ale pre istotu */
			pthread_mutex_unlock(&stack->mutex);
			continue;
		}

		int r = (rand() % 5) + 1;     /* cíti sa rozhodne zobrať 1..5 správ */
		if (r > available) r = available; /* ale nemôžeme vziať viac než je */

			/* už sme sem_wait-om zobrali 1 položku; potrebujeme zobrať ešte r-1 ďalších z items semaforu */
			for (int extra = 1; extra < r; extra++) {
				sem_wait(&stack->items);
			}

			/* teraz spracujeme r správ *pod mutexom* (odoberieme ich z buffra a uvoľníme space) */
			for (int i = 0; i < r; i++) {
				message_t m = stack->messages[stack->out];

				if (!m.isApprop) {
					if (strcmp(m.sentBy, "Dudy") == 0) {
						inappropCountDudy++;
					} else {
						inappropCountCherry++;
					}
					printf("Kontrolor nasiel nevhodnu spravu u pouzivatela %s\n", m.sentBy);
				}

				stack->out = (stack->out + 1) % stack->capacity;
				sem_post(&stack->space); /* uvolnime miesto po kazdej sprave */
			}

			checked += r;
			pthread_mutex_unlock(&stack->mutex);

			/* kontrolor spracuvava r sprav: 1 sprava = 1 sekunda (r sekund), mimo kritickej sekcie */
			sleep(r);
	}

	printf("Pouzivatelovi Dudy sa znizuju socialne kredity, pocet nevhodnych sprav bol %d, percentualne vyjadrenie: %.2f%%\n",
		   inappropCountDudy, ((double)inappropCountDudy / stack->total * 100.0));
	printf("Pouzivatelovi Cherry sa znizuju socialne kredity, pocet nevhodnych sprav bol %d, percentualne vyjadrenie: %.2f%%\n",
		   inappropCountCherry, ((double)inappropCountCherry / stack->total * 100.0));
	return NULL;
}

int main (int argc, char** argv) {
	int k = 8;
	if (argc == 2)
		k = atoi(argv[1]);

	srand(time(NULL));

	message_stack_t stack;
	stack.in = stack.out = 0;
	stack.capacity = k;
	stack.total = 50;
	stack.messages = malloc(sizeof(message_t) * k);
	if (!stack.messages) {
		perror("malloc");
		return 1;
	}

	user_t users[2];
	users[0].stack = &stack;
	users[0].name = "Dudy";
	users[0].messageCount = 20;
	users[0].durationMin = 1;
	users[0].durationMax = 3;
	users[0].chance = 30;
	users[1].stack = &stack;
	users[1].name = "Cherry";
	users[1].messageCount = 30;
	users[1].durationMin = 1;
	users[1].durationMax = 2;
	users[1].chance = 50;

	pthread_mutex_init(&stack.mutex, NULL);
	sem_init(&stack.space, 0, k);
	sem_init(&stack.items, 0, 0);

	pthread_t threads[3];

	for (int i = 0; i < 2; i++) {
		pthread_create(&threads[i], NULL, send_message, &users[i]);
	}
	pthread_create(&threads[2], NULL, controller_check, &stack);

	for (int i = 0; i < 2; i++) {
		pthread_join(threads[i], NULL);
	}
	pthread_join(threads[2], NULL);

	pthread_mutex_destroy(&stack.mutex);
	sem_destroy(&stack.space);
	sem_destroy(&stack.items);
	free(stack.messages);

	return 0;
}
