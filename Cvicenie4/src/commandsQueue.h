#ifndef COMMANDS_QUEUE_H
#define COMMANDS_QUEUE_H
#include "command.h"
#include <stddef.h>
#define MAX_COMMANDS_COUNT 10

typedef struct CommandsQueue {
// Fixné pole príkazov
// Index, na ktorom sa bude vkladať
// Index, z ktorého sa bude vyberať
// Aktuálny počet príkazov vo fronte
	command_t commands[MAX_COMMANDS_COUNT];
	size_t inIndex;
	size_t outIndex;
	size_t size;
} commands_queue_t;
// Deklarácie funkcií pracujúcich s frontom príkazov
void commands_queue_init(commands_queue_t * this);
void commands_queue_destroy(commands_queue_t * this);
_Bool commands_queue_enqueue(commands_queue_t * this, const command_t * command);
_Bool commands_queue_dequeue(commands_queue_t * this, command_t * command);
command_t * commands_queue_front(commands_queue_t * this);
command_t * commands_queue_back(commands_queue_t * this);
void commands_queue_print(const commands_queue_t * this);
_Bool commands_queue_is_empty(const commands_queue_t * this);
_Bool commands_queue_is_full(const commands_queue_t * this);
#endif //COMMANDS_QUEUE_H
