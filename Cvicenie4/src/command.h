#ifndef COMMAND_H
#define COMMAND_H
#include <stdio.h>
#define COMMAND_MAX_LENGTH 101
#define SEPARATOR_MAX_LENGTH 2
#define DELIMITER_MAX_LENGTH 2

typedef struct Command {
// reťazec predstavujúci príkaz s maximálne 100 platnými znakmi
// reťazec predstavujúci oddelovač hodnôt v príkaze, ktorý ma 1 znak
// reťazec predstavujúci ukončovací znak príkazu, ktorý ma 1 znak
	char string[COMMAND_MAX_LENGTH];
	char separator[SEPARATOR_MAX_LENGTH];
	char delimiter[DELIMITER_MAX_LENGTH];
} command_t;
// Deklarácie funkcií pracujúcich s príkazom
void command_init(command_t * this, const char * commandContent, const char * separator, const char * endCharacter);
void command_destroy(command_t * this);
_Bool command_try_append_string(command_t * this, const char * string, _Bool last);
void command_get_command_copy(const command_t * this, command_t * copy);
void command_print(const command_t * this, FILE* stream);

#endif //COMMAND_:H
