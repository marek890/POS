#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// Pridanie vytvorených hlavičiek súborov
#include "command.h"
#include "commandSettings.h"
#include "commandsQueue.h"
// Funkcia dostupná len v tomto súbore pre získanie hodnoty premennej prostredia, pričom sa vráti 1 ak bolo vloženie úspešné, inak sa vráti 0,
static _Bool get_env_value(const char * name, char ** value) {
	*value = getenv(name);
	return *value != NULL;
}

// Funkcia dostupná len v tomto súbore na zavolanie príkazu v systéme ("v bashi")
static void call_system_command(const char * command) {
	printf("Volam prikaz \"%s\"!\n", command);
	system(command);
}

// Funkcia dostupná len v tomto súbore na vygenerovanie náhodného reťazca danej dĺžky, ktorý je tvorený buď len číslami, alebo len malými písmenami,
static void generate_word(char * word, size_t length, _Bool useNumbers) {
// pre každý znak od 0 až po dĺžku, pričom na znak na dĺžke sa vloží znak '\0'
// Ak používaš len čísla, tak vygeneruj číslicu, pričom ak je to prvá číslica v reťazci a reťazec je dlhší ako 1, tak vynechaj číslicu 0 a vlož ju na danú pozíciu v reťazci
// Inak vygeneruj malé písmeno a vlož ho na danú pozíciu v reťazci
	for (size_t i = 0; i < length; i++) {
		if (useNumbers) {
			if (i == 0 && length > 1) {
				word[i] = '1' + (rand() % 9);
			}
			else {
				word[i] = '0' + (rand() % 10);
			}
		}
		else {
			word[i] = 'a' + (rand() % 26);
		}
	}
	word[length] = '\0';
}

// Funkcia dostupná len v tomto súbore ktorá pridá náhodný reťazec danej dĺžky do príkazu, pričom je možné zvoliť, či sa jedná o reťazec tvorený malými písmenami alebo číselný reťazec
static void append_random_string_to_command(command_t * command, size_t length, _Bool useNumbers, _Bool isLast) {
// Vytvorenie pomocnej premennej typu reťazec s danou dĺžkou
// Vygenerovanie príslušného reťazca
// Pridanie reťazca na koniec príkazu, pričom sa použije buď oddeľovač alebo ukončovací znak,
	char string[length + 1];
	generate_word(string, length, useNumbers);
	command_try_append_string(command, string, isLast);
}
// Funkcia dostupná len v tomto súbore na vygenerovanie náhodnej dĺžky číselného reťazca alebo textového reťazca tvoreného len malými písmenami,
#define TMP_NUMBER_MAX_CAPACITY 10
#define TMP_MESSAGE_MAX_CAPACITY 50
static int get_random_length(_Bool useNumbers) {
// Vráť náhodnú hodnotu z rozsahu <1,9> pre čísla a <1, 49> malé písmená s použitím príslušných makier,
	if (useNumbers)
		return (rand() % (TMP_NUMBER_MAX_CAPACITY - 1)) + 1;

	return (rand() % (TMP_MESSAGE_MAX_CAPACITY - 1)) + 1 ;
}
#undef TMP_NUMBER_MAX_CAPACITY
#undef TMP_MESSAGE_MAX_CAPACITY
// Funkcia dostupná len v tomto súbore na vygenerovanie príkazu daného typu
static void generate_command_type(possible_command_t type, command_t * command) {
	command_try_append_string(command, possibleCommandRepresentation[type], type == QUIT);
	switch (type) {
		case START: case END: {
			append_random_string_to_command(command, get_random_length(1), 1, 1);
			break;
		} case SEND: {
			append_random_string_to_command(command, get_random_length(1), 1, 0);
			append_random_string_to_command(command, get_random_length(0), 0, 1);
			break;
		} case RECEIVE: {
			append_random_string_to_command(command, get_random_length(0), 0, 0);
			append_random_string_to_command(command, get_random_length(1), 1, 1);
			break;
		} case QUIT: {
			break;
		}
	}
}
int main(int argc, char ** argv) {
// Program vyžaduje 2 argumenty: cesta k súboru a počet príkazov. Ak sa neuvedú, je potrebné skončiť program a vypísať na štandardný chybový výstup informáciu o použití programu
	if (argc != 3) {
		fprintf(stderr, "Nebol zadany spravny pocet argumentov, potrebny pocet je 2\n");
		return 1;
	}
// Získanie hodnoty oddelovača najskôr z prostredia a ak nie, tak potom z makra
	char * delimiter;
	if (!get_env_value("DELIMITER", &delimiter)) {
		delimiter = COMMAND_DELIMITER;
	}
// Získanie hodnoty ukončovacieho znaku príkazov najskôr z prostredia a ak nie, tak potom z makra,
	char * endCharacter;
	if (!get_env_value("END_CHAR", &endCharacter)) {
		endCharacter = COMMAND_END_CHARACTER;
	}
	srand(time(NULL));
	char * fileName = argv[1]; // Sem vložte názov súboru zadaný ako argument
	int numOfCommands = atoi(argv[2]); // Získanie počtu vygenerovaných príkazov z argumentu
// Otvorenie súboru s názvom uloženým v premennej fileName pre zápis, pričom ak sa súbor nepodarí otvoriť, je potrebné vypísať chybu a skončiť program chybovou hodnotou,
// Prvé dva riadky súboru obsahujú oddeľovač a ukončovací znak pre vygenerované príkazy
// Vygenerujte zadaný počet príkazov, pričom použite funkciu generate_command_type a command_print,
// Po skončení zatvorte súbor
	FILE* f = fopen(fileName, "w");
	if (f == NULL) {
		fprintf(stderr, "Nepodarilo sa otvorit subor");
		return 1;
	}

	fprintf(f, "%s\n", delimiter);
	fprintf(f, "%s\n", endCharacter);
	for (int i = 0; i < numOfCommands; i++) {
		command_t command;
		command_init(&command, "", delimiter, endCharacter);
		possible_command_t type = (possible_command_t)(rand() % POSSIBLE_COMMAND_TYPE_COUNT);
		generate_command_type(type, &command);
		command_print(&command, f);
		command_destroy(&command);
	}
	fclose(f);
	char tmpString[6 + strlen(fileName)];
	strcpy(tmpString, "stat ");
	strcat(tmpString, fileName);
	call_system_command(tmpString);
	strcpy(tmpString, "cat ");
	strcat(tmpString, fileName);
	call_system_command(tmpString);
// Opätovné otvorenie súboru s názvom uloženým v premennej fileName, pričom sa tentokrát otvorí súbor pre čítanie a ak by došlo k chybe, je potrebné vypísať chybu, vymazať súbor použitím funkcie remove a skončiť program chybovou hodnotou
// Príprava frontu príkazov a pomocných reťazcov pre oddeľovač, ukončovací znak a príkaz,
// V cykle najskôr získajte z prvých dvoch riadkov hodnoty oddeľovača a ukončovacieho znaku a následne všetkých príkazov, pričom ich postupne vkladajte do frontu,
// Po vložení vypíšte obsah frontu príkazov a následne ho aj vymažte
// Pred skončením zistite od používateľa, či chce odstrániť súbor a ak by o to mal záujem, tak ho odstránňte pomocou funkcie remove,
	f = fopen(fileName, "r");
	if (f == NULL) {
		remove(fileName);
		fprintf(stderr, "Nepodarilo sa otvorit subor");
		return 1;
	}
	
	char delimiter2[SEPARATOR_MAX_LENGTH];
	fscanf(f, "%s", delimiter2);
	char endCharacter2[DELIMITER_MAX_LENGTH];
	fscanf(f, "%s", endCharacter2);
	commands_queue_t queue;
	commands_queue_init(&queue);
	char buf[COMMAND_MAX_LENGTH];
	memset(&buf, 0, sizeof(buf));
	while (fscanf(f, "\n", buf) == 1) {
		command_t command;
		command_init(&command, buf, delimiter2, endCharacter2);
		commands_queue_enqueue(&queue, &command);
	}
	fclose(f);
	
	commands_queue_print(&queue);
	commands_queue_destroy(&queue);
	int input;
	printf("Chcete odstranit subor? (Zadajte cislo)\n");
	printf("1. Ano\n");
	printf("2. Nie\n");

	scanf("%d", &input);
	if (input == 1) {
		remove(fileName);
	}

	return 0;
}
