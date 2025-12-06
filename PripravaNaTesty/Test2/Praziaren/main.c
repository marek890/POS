#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define SHOP_CAPACITY 100
#define TOTAL_COFFEE_KG 150
#define MAX_BAG_G 250
#define TOTAL_BAGS (TOTAL_COFFEE_KG * 1000 / MAX_BAG_G)
#define LIGHT 0
#define MEDIUM 1

typedef struct Data {
	int lightCount;
	int mediumCount;
	int lightSold;
	int mediumSold;
	pthread_mutex_t mutex;
	sem_t semShopSpace;
	sem_t semShopItems;
} data_t;

void* doplnanie (void* arg) {
	data_t* data = (data_t*)arg;
	
	for (int i = 0; i < TOTAL_BAGS; i++) {
		int r = rand() % 2;
		
		sem_wait(&data->semShopSpace);
		pthread_mutex_lock(&data->mutex);
		if (r == LIGHT) {
			data->lightCount++;
		}
		else
			data->mediumCount++;
		pthread_mutex_unlock(&data->mutex);
		sem_post(&data->semShopItems);
	}
	return NULL;
}

void* predavavanie (void* arg) {
	data_t* data = (data_t*)arg;
	
	while (1) {
		int r = rand() % 2;

		pthread_mutex_lock(&data->mutex);
		if (data->lightSold + data->mediumSold == TOTAL_BAGS) {
			pthread_mutex_unlock(&data->mutex);
			break;
		}
		pthread_mutex_unlock(&data->mutex);

		sem_wait(&data->semShopItems);
		
		pthread_mutex_lock(&data->mutex);
		if (r == LIGHT) {
			if (data->lightCount > 0) {
				data->lightCount--;
				data->lightSold++;
			}
			else if(data->mediumCount > 0) {
				data->mediumCount--;
				data->mediumSold++;
			}
		}
		else {
			if (data->mediumCount > 0) {
				data->mediumCount--;
				data->mediumSold++;
			}
			else if (data->lightCount > 0) {
				data->lightCount--;
				data->lightSold++;
			}
		}
		pthread_mutex_unlock(&data->mutex);

		sem_post(&data->semShopSpace);
	}
	return NULL;
}

int main(void) {
	pthread_t threads[2];
	srand(time(NULL));
	data_t data;

	data.lightCount = 0;
	data.mediumCount = 0;
	data.lightSold = 0;
	data.mediumSold = 0;
	
	pthread_mutex_init(&data.mutex, NULL);

	sem_init(&data.semShopSpace, 0, SHOP_CAPACITY);
	sem_init(&data.semShopItems, 0, 0);

	pthread_create(&threads[0], NULL, doplnanie, &data);
	pthread_create(&threads[1], NULL, predavavanie, &data);
	pthread_join(threads[0], NULL);
	pthread_join(threads[1], NULL);

	printf("Predalo sa %d vreciek svetlo prazenej kavy\n", data.lightSold);
	printf("Predalo sa %d vreciek stredne prazenej kavy\n", data.mediumSold);

	sem_destroy(&data.semShopSpace);
	sem_destroy(&data.semShopItems);

	pthread_mutex_destroy(&data.mutex);
	
	return 0;
}
