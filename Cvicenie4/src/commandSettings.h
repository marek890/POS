#ifndef COMMAND_SETTINGS_H
#define COMMAND_SETTINGS_H

#define COMMAND_DELIMITER "|"
#define COMMAND_END_CHARACTER ";"
// cvičenie 04 -- pridanie
#define POSSIBLE_COMMAND_TYPE_COUNT 5
// Vymenovaný typ, ktorého hodnoty sú START = 0, SEND = 1, ... , QUIT = 4
typedef enum {
	START, SEND, RECEIVE, END, QUIT
} possible_command_t;
// Reťazcová reprezentácia pre jednotlivé príkazy
const char * possibleCommandRepresentation[POSSIBLE_COMMAND_TYPE_COUNT] = {"start", "send", "receive", "end", "quit"};
#endif
