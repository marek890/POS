#ifndef COMMANDS_GENERATOR_H
#define COMMANDS_GENERATOR_H
#include <stdio.h>
#include "command.h"
// Presun vymenovaného typu a makra zo súboru commandSettings.h
#define POSSIBLE_COMMAND_TYPE_COUNT 5
typedef enum {
START, SEND, RECEIVE, END, QUIT
} possible_command_t;
// Hlavičky funkcií, pričom sa budú využívať funkcie z pôvodného main.c súboru
const char * get_command_type_representation(possible_command_t type);
void generate_command_type(possible_command_t type, command_t * command);
void generate_random_command_type_into_file(FILE * file, const char * delimiter, const char *
endCharacter);
#endif //COMMANDS_GENERATOR_H
