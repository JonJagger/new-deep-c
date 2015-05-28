/*
** server.c -- a stream socket server demo
*/

#include "message.h"

#include <stdio.h>				// stdout, fprintf(), perror()
#include <stdlib.h>				// EXIT_FAILURE

int main(void)
{
	const char greeting[] = "Hello, world!";
	int rv = send_message(greeting, sizeof greeting);
	if (rv != 0)
	{
		perror("send_message");
		return EXIT_FAILURE;		
	}

	fprintf(stdout, "server: sent '%s'\n", greeting);
}
