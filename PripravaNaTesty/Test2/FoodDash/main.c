#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define N 3
#define K 6
#define M 16
#define CAS_DORUCENIA 4
#define ODMENA 5

typedef struct {
	int cislo;
	int hodnota;
	int casGener;
	int odmena;
} objednavka_t;

typedef struct {
	objednavka_t objednavky[K];
	_Bool vsetkySpracovane;
	int in, out;
	pthread_mutex_t mutex;
	sem_t pocet;
	sem_t miesto;
} zoznam_objednavok_t;

typedef struct {
	zoznam_objednavok_t* zoznam;
	int id;
	int dorucene;
	int celkovyCas;
	int celkovyZarobok;
} kurier_t;

void* generuj_objednavky(void* arg) {
	zoznam_objednavok_t* zoznam = (zoznam_objednavok_t*)arg;

	for (int i = 0; i < M; i++) {
		sem_wait(&zoznam->miesto);
		printf("Dispecing generuje objednavku c.%d\n", i);
		objednavka_t obj;
		obj.cislo = i;
		obj.hodnota = rand() % 41 + 10;
		obj.casGener = rand() % 3 + 1;
		obj.odmena = 5;
		sleep(obj.casGener);

		pthread_mutex_lock(&zoznam->mutex);

		zoznam->objednavky[zoznam->in] = obj;
		zoznam->in = (zoznam->in + 1) % K;
		
		pthread_mutex_unlock(&zoznam->mutex);
		sem_post(&zoznam->pocet);
		printf("Dispecing uspesne vygeneroval objednavku c.%d\n", i);
	}
	pthread_mutex_lock(&zoznam->mutex);
	zoznam->vsetkySpracovane = 1;
	for (int i = 0; i < N; i++)
		sem_post(&zoznam->pocet);
	pthread_mutex_unlock(&zoznam->mutex);
	return NULL;
}

void* spracuj_objednavky(void* arg) {
	kurier_t* kurier = (kurier_t*)arg;
	zoznam_objednavok_t* zoznam = kurier->zoznam;

	while (1) {
		sem_wait(&zoznam->pocet);
		pthread_mutex_lock(&zoznam->mutex);
		if (zoznam->vsetkySpracovane == 1 && zoznam->in == zoznam->out) {
			pthread_mutex_unlock(&zoznam->mutex);
			break;
		}

		printf("Kurier %d prebera objednavku c.%d\n", kurier->id, zoznam->objednavky[zoznam->out].cislo);
		sleep(CAS_DORUCENIA);
		printf("Kurier %d dorucil objednavku c.%d\n", kurier->id, zoznam->objednavky[zoznam->out].cislo);
		kurier->celkovyCas += CAS_DORUCENIA;
		kurier->dorucene++;
		kurier->celkovyZarobok += zoznam->objednavky[zoznam->out].odmena;
		zoznam->out = (zoznam->out + 1) % K;
		pthread_mutex_unlock(&zoznam->mutex);
		sem_post(&zoznam->miesto);
	}

	printf("Kurier %d dorucil %d objednavok, stravil pri tom %d sekund a celkovo zarobil %d\n", kurier->id, kurier->dorucene, kurier->celkovyCas, kurier->celkovyZarobok);

	return NULL;
}

int main (int argc, char** argv) {
	srand(time(NULL));
	zoznam_objednavok_t zoznam;
	zoznam.in = zoznam.out = 0;
	zoznam.vsetkySpracovane = 0;
	pthread_mutex_init(&zoznam.mutex, NULL);
	sem_init(&zoznam.pocet, 0, 0);
	sem_init(&zoznam.miesto, 0, K);

	kurier_t kurieri[N];
	for (int i = 0; i < N; i++) {
		kurieri[i].zoznam = &zoznam;
		kurieri[i].id = i;
		kurieri[i].dorucene = 0;
		kurieri[i].celkovyCas = 0;
		kurieri[i].celkovyZarobok = 0;
	}

	pthread_t dispecing;
	pthread_t kurier[N];

	pthread_create(&dispecing, NULL, generuj_objednavky, &zoznam);
	for (int i = 0; i < N; i++) {
		pthread_create(&kurier[i], NULL, spracuj_objednavky, &kurieri[i]);
	}

	pthread_join(dispecing, NULL);
	for (int i = 0; i < N; i++) {
		pthread_join(kurier[i], NULL);
	}

	pthread_mutex_destroy(&zoznam.mutex);
	sem_destroy(&zoznam.pocet);
	sem_destroy(&zoznam.miesto);

	return 0;
}
