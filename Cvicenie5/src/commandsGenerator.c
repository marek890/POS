#include "commandsGenerator.h"
// Príslušné vloženia hlavičkových súborov
// Reprezentácia presunutá zo súboru commandSettings.h
static const char * possibleCommandRepresentation[POSSIBLE_COMMAND_TYPE_COUNT] = {"start", "send", "receive", "end", "quit"};
const char * get_command_type_representation(possible_command_t type) {
// Funkcia vráti textovú reprezentáciu príslušného typu príkazu, pričom sa používa pole possibleCommandRepresentation
}
// Začiatok funkcií presunutých zo súboru main.c z cvičenia 4
static void generate_word(char * word, size_t length, _Bool useNumbers) {
// TODO
}
static void append_random_string_to_command(command_t * command, size_t length, _Bool useNumbers, _Bool isLast) {
// TODO
}
#define TMP_NUMBER_MAX_CAPACITY 10
#define TMP_MESSAGE_MAX_CAPACITY 50
static int get_random_length(_Bool useNumbers) {
// TODO
}
#undef TMP_NUMBER_MAX_CAPACITY
#undef TMP_MESSAGE_MAX_CAPACITY
// Zmena vo funkcii --> odtránenie kvalifikátora static
void generate_command_type(possible_command_t type, command_t * command) {
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

// Koniec funkcií presunutých zo súboru main.c z cvičenia 4
void generate_random_command_type_into_file(FILE * file, const char * delimiter, const char *endCharacter) {
// Funkcia vygeneruje náhodný typ príkazu pomocou funkcie generate_command_type
// Tento príkaz sa potom vloží do súboru, pričom sa použije už existujúca funkcia command_print,
}
