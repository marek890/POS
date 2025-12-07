#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define BOX_MAX 3

typedef struct {
	int priority;
	int duration;
} concert_t;

typedef struct {
	concert_t concerts[BOX_MAX];
	int in, out;
	pthread_mutex_t mutex;
	sem_t space;
	sem_t items;
} box_t;

typedef struct {
	box_t* box;
	concert_t* concerts;
	int in;
	int capacity;
} manager_data_t;

int compare(const void* a, const void* b) {
	concert_t c1 = *(const concert_t*)a;
	concert_t c2 = *(const concert_t*)b;

	if (c1.priority != c2.priority) 
		return c2.priority - c1.priority;
	return c1.duration - c2.duration;

}

void* manager_work(void* arg) {
	manager_data_t* mData = (manager_data_t*)arg;
	box_t* box = mData->box;	
	for (int i = 0; i < mData->capacity; i++) {
		int r = rand () % 7;
		mData->concerts[i].priority = r;
		printf("test\n");
		if (r < 3)
			mData->concerts[i].duration = (rand() % 5) + 1;
		else if (r == 3 || r == 4) 
			mData->concerts[i].duration = (rand() % 5) + 2;
		else
			mData->concerts[i].duration = (rand() % 5) + 4;
		sleep((rand() % 3) + 2);
	}
	qsort(mData->concerts, mData->capacity, sizeof(concert_t), compare);
	
	for (int i = 0; i < mData->capacity; i++) {
		sem_wait(&box->space);
		pthread_mutex_lock(&box->mutex);
		
		printf("Manazer spracuvava poziadavku po zadavatela %d, koncert ma trvat %d hodin\n", mData->concerts[mData->in].priority, mData->concerts[mData->in].duration);
		box->concerts[box->in] = mData->concerts[mData->in];
		mData->in++;
		box->in = (box->in + 1) % BOX_MAX;
		pthread_mutex_unlock(&box->mutex);
		sem_post(&box->items);
	}
	
	for (int i = 0; i < BOX_MAX; i++) {
		concert_t sent;
		sent.priority = -1;
		sent.duration = 0;
		sem_wait(&box->space);
		pthread_mutex_lock(&box->mutex);
		box->concerts[box->in] = sent;
		box->in = (box->in + 1) % BOX_MAX;
		pthread_mutex_unlock(&box->mutex);
		sem_post(&box->items);
	}

	return NULL;
}

void* tech_work(void* arg) {
	box_t* box = (box_t*)arg;
	while(1) {
		sem_wait(&box->items);
		pthread_mutex_lock(&box->mutex);
		concert_t c = box->concerts[box->out];
		box->out = (box->out + 1) % BOX_MAX;
		pthread_mutex_unlock(&box->mutex);
		sem_post(&box->space);
		if (c.priority == -1)
		{
			break;
		}
		printf("Technik dohliada na koncert od zadavatela %d, koncert ma trvat %d hodin\n", c.priority, c.duration);
		sleep(c.duration);
		printf("Koncert od zadavatela %d skoncil, trval %d hodin\n", c.priority, c.duration);
	}
	return NULL;
}

int main (int argc, char** argv) {
	srand(time(NULL));
	if (argc != 2) {
		fprintf(stderr, "Bol zadany nespravny pocet argumentov\n");
		return -1;
	}
	int k = atoi(argv[1]);
	if (k <= 6 || k >= 18)
		k = 10;
	
	box_t box;
	box.in = box.out = 0;
	pthread_t manager;
	pthread_t tech[BOX_MAX];
	pthread_mutex_init(&box.mutex, NULL);
	sem_init(&box.space, 0, BOX_MAX);
	sem_init(&box.items, 0, 0);
	
	concert_t concerts[k];
		
	manager_data_t mData;
	mData.box = &box;
	mData.concerts = concerts;
	mData.capacity = k;
	mData.in = 0;
	
	pthread_create(&manager, NULL, manager_work, &mData);
	for (int i = 0; i < BOX_MAX; i++)
		pthread_create(&tech[i], NULL, tech_work, &box);
	pthread_join(manager, NULL);
	for (int i = 0; i < BOX_MAX; i++)
		pthread_join(tech[i], NULL);
	
	int concertTypeCount[7];
	for (int i = 0; i < 7; i++) {
		concertTypeCount[i] = 0;
	}

	for (int i = 0; i < k; i++) {
		if (concerts[i].priority == 0)
			concertTypeCount[0]++;
		else if (concerts[i].priority == 1)
			concertTypeCount[1]++;
		else if (concerts[i].priority == 2)
			concertTypeCount[2]++;
		else if (concerts[i].priority == 3)
			concertTypeCount[3]++;
		else if (concerts[i].priority == 4)
			concertTypeCount[4]++;
		else if (concerts[i].priority == 5)
			concertTypeCount[5]++;
		else
			concertTypeCount[6]++;
	}

	for (int i = 0; i < 7; i++) {
		printf("Zadavatel %d - %d koncertov z %d, podiel %.2f% \n", i, concertTypeCount[i], k, ((double)concertTypeCount[i] / k * 100));
	}

	pthread_mutex_destroy(&box.mutex);
	sem_destroy(&box.space);
	sem_destroy(&box.items);

	return 0;
}
