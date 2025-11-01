#include "command.h"
#include "commandSettings.h"
#include "commandsQueue.h"

int main(void) {
	command_t command;
	command_t command2;
	commands_queue_t commands;
	command_init(&command, "start|12345|", COMMAND_DELIMITER, COMMAND_END_CHARACTER);
	command_init(&command2, "ahoj|8794|", COMMAND_DELIMITER, COMMAND_END_CHARACTER); commands_queue_init(&commands);
	commands_queue_enqueue(&commands, &command);
	commands_queue_enqueue(&commands, &command2);
	commands_queue_print(&commands);
	commands_queue_is_empty(&commands);
	commands_queue_destroy(&commands);
	command_destroy(&command);
	command_destroy(&command2);
	return 0;
}
