#include "command.h"
#include <stdio.h>
#include <string.h>

static void command_clear(command_t * this) {
 // Pomocná funkcia dostupná len v tomto súbore, ktorá vyčistí všetky reťazce -> nastavísa všade hodnota znaku '\0', resp. hodnota 0,→
	memset(this, 0, sizeof(command_t));
}

void command_init(command_t * this, const char * commandContent, const char * separator, const char * endCharacter) {
	command_clear(this);
 // Nastavia sa všetci členovia štruktúry na príslušné hodnoty, pričom sa v prípade oddeľovača a ukončovacieho znaku zoberie len prvý znak,→
	strncpy(this->string, commandContent, COMMAND_MAX_LENGTH); 
	this->separator[0] = separator[0];
	this->delimiter[0] = endCharacter[0];
}

void command_destroy(command_t * this) {
 	command_clear(this);
}

_Bool command_try_append_string(command_t * this, const char * string, _Bool last) {
 // Ak už sa v príkaze nachádza ukončovací znak, vypíš príslušné hlásenie na štandardný chybový výstup a vráť 0, resp. false,→
 // Ak sa reťazec už rozmerovo nedá vložiť do príkazu, vypíš príslušné hlásenie na štandardný chybový výstup a vráť 0, resp. false,→
// Pridaj reťazec do príkazu
// Ak je to posledná hodnota, ukonči príkaz ukončovacím znakom
// Inak ukonči pridaný reťazec oddeľovačom
	if (strchr(this->string, this->delimiter[0]) != NULL) {
		fprintf(stderr, "Command is already finished\n");
		return 0;
	}
	
	if (strlen(string) + strlen(this->string) > COMMAND_MAX_LENGTH) {
		fprintf(stderr, "String is too long\n");
	}

	strcat(this->string, string);

	if (last)
		strcat(this->string, this->delimiter);
	else
		strcat(this->string, this->separator);

return 1;
}

void command_get_command_copy(const command_t * this, command_t * copy) {
// Funkcia do kópie skopíruje celý príkaz
	memcpy(copy, this, sizeof(command_t));
}

void command_print(const command_t * this) {
// Funkcia vypíše textovú reprezentáciu príkazu
	printf("%s", this->string);
}
