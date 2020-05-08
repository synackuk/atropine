#ifndef COMMAND_H
#define COMMAND_H

typedef struct command_args {
	signed int unk1;
	unsigned int uinteger;
	signed int integer;
	unsigned int type;
	char* string;
} command_args;

typedef int(*command_t)(int argc, command_args* argv);

typedef struct command_descriptor {
	char* name;
	command_t handler;
	char* description;
} command_descriptor;

int add_command(char* name, command_t handler, char* description);
int parse_command(int argc, command_args* argv);

extern int num_commands;

extern command_descriptor* commands[];

#endif