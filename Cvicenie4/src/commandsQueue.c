#include "commandsQueue.h"
#include <string.h>
#include <stdio.h>
// Príslušné vloženia hlavičkových súborov
static void commands_queue_clear(commands_queue_t * this) {
// Pomocná funkcia dostupná len v tomto súbore, ktorá vyčistí front príkazov
	memset(this, 0, sizeof(commands_queue_t)); 
}
void commands_queue_init(commands_queue_t * this) {
// Inicializácia frontu príkazov
	commands_queue_clear(this);
}
void commands_queue_destroy(commands_queue_t * this) {
	commands_queue_clear(this);
}
_Bool commands_queue_enqueue(commands_queue_t * this, const command_t * command) {
// Ak je plný front, príkaz sa nevloží!
// Vloženie príkazu, posun indexu na vloženie (pozor na hraničné hodnoty!) a navýšenie počtu prvkov,→
	if (this->size == MAX_COMMANDS_COUNT) 
		return 0;
	
	this->commands[this->inIndex] = *command;
	this->inIndex = (this->inIndex + 1) % MAX_COMMANDS_COUNT;
	this->size++;

	return 1;
}
_Bool commands_queue_dequeue(commands_queue_t * this, command_t * command) {
// Ak je front prázdny, nie je čo vybrať!
// Výber príkazu, posun indexu na výber (pozor na hraničné hodnoty!) a zníženie počtu prvkov,→
	if (this->size == 0)
		return 0;

	*command = this->commands[this->outIndex];
	this->outIndex = (this->outIndex + 1) % MAX_COMMANDS_COUNT;
	this->size--;

	return 1;
}
command_t * commands_queue_front(commands_queue_t * this) {
// Ak je prázdny front, tak vráť NULL
// Inak vráť odkaz na príkaz, ktorý sa najbližšie vyberie
	if (this->size == 0)
		return NULL;

	return &this->commands[this->outIndex];
}
command_t * commands_queue_back(commands_queue_t * this) {
// Ak je prázdny front, tak vráť NULL
// Inak vráť odkaz na miesto vo fronte, kde sa bude vkladať nový príkaz
	if (this->size == 0)
		return NULL;

	return &this->commands[this->inIndex];
}
void commands_queue_print(const commands_queue_t * this) {
// Výpis všetkých platných príkazov vo fronte a v platnom poradí!
	size_t index = this->outIndex;

	for (size_t i = 0; i < this->size; i++) {
		command_print(&this->commands[index], stdout);
		index = (index + 1) % MAX_COMMANDS_COUNT;
		printf("\n");
	}
}
_Bool commands_queue_is_empty(const commands_queue_t * this) {
// vráti 1, resp. true, ak je front prázdny, inak vráti 0, resp. false
	return this->size == 0;
}
_Bool commands_queue_is_full(const commands_queue_t * this) {
// vráti 1, resp. true, ak je front plný, inak vráti 0, resp. false
	return this->size == MAX_COMMANDS_COUNT;
}
