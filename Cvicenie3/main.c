#include "command.h"
#include "commandSettings.h"

int main(void) {
	command_t command;
	command_init(&command, "start|12345;", COMMAND_DELIMITER, COMMAND_END_CHARACTER);
// Skúste rovnaký príkaz vytvoriť za použitia funkcie command_try_append_string
	command_print(&command);
	command_destroy(&command);
 	return 0;
}
