
#include "userLibrary.h"
#include "shell.h"
#include <stdlib.h>
#include <stdio.h>
#include <user_time.h>
#include <string.h>
#define MAX_BUFFER 254
#define COMMANDS_SIZE 10

static void printLine(char c);
int linePos = 0;
char lastc;
int lastEnter = 0;
char line[MAX_BUFFER + 1] = {0};

char command[MAX_BUFFER + 1] = {0};
char parameter[MAX_BUFFER + 1] = {0};

char buffer[MAX_BUFFER] = {0};

char *command_names[] = {"help", "time", "clear", "snake 1", "snake 2", "inforeg", "zerodiv", "invopcode", "increment", "decrement"};

void (*command_func[COMMANDS_SIZE])() = {
		help,
		user_time,
		clear,
		shell_snake_1,
		shell_snake_2,
		inforeg,
		exc_zerodiv,
		exc_invopcode,
		increment_size_char,
		decrement_size_char};

void shell()
{
	// increment_size_char();
	// increment_size_char();

	printStr("Bienvenido al shell\n");
	printStr("Ingrese un caracter\n");
	char c;

	while (1)
	{
		c = getChar();
		if (lastc != c)
		{
			printLine(c);
		}
	}
}

static void printLine(char c)
{
	if (isChar(c) || isDigit(c))
	{
		buffer[lastEnter] = c;
		lastEnter++;
	}
	if (c == '\n')
	{
		buffer[lastEnter] = '\0';
		readCommand();
		buffer[0] = '\0'; // vacio el buffer
		printStr(buffer);
		lastEnter = 0;
	}

	if (c == '\b' && lastEnter > 0)
	{
		lastEnter--;
		buffer[lastEnter] = '\0';
	}
	else if (c == '\b' && lastEnter == 0)
	{
		lastc = c;
		return;
	}

	// salvo el caso de querer borrar despues de un enter, todos los caracteres se imprimen
	printChar(c);
	lastc = c;
}

// make the newline function

void help()
{
	printStr("\n--------------------| MANUAL |--------------------");
	printStr("\n time                     shows actual time");
	printStr("\n clear                    clears the screen");
	printStr("\n snake [cant jugadores]   starts de Snake game");
	printStr("\n inforeg                  prints the regsiter's values");
	printStr("\n zerodiv                  division by cero exception");
	printStr("\n invopcode                tests the invalid operation code");
	printStr("\n increment                increase letter size");
	printStr("\n decrement                decrease letter size");
	printStr("\n---------------------------------------------------");
}

static int strcmp_shell(char *str1, char *str2)
{
	int i = 0;
	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
		{
			return 0;
		}
		i++;
	}
	if (str1[i] != str2[i])
	{
		return 0;
	}
	return 1;
}

void readCommand()
{
	for (int i = 0; i < COMMANDS_SIZE; i++)
	{
		if (strcmp_shell(buffer, command_names[i]))
		{
			command_func[i]();
			return;
		}
	}
	notFound();
}

void notFound()
{
	printStr("\n command not found. Try using 'help'");
	return;
}

void shell_snake_1()
{
	// snake(1);
}

void shell_snake_2()
{
	// snake(2);
}