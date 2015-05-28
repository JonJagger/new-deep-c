/*
** client.c -- a stream socket client demo
*/

#include "message.h"			// recv_message

#include <stdio.h>				// stderr, stdout, fprintf(), perror()
#include <stdlib.h>				// EXIT_FAILURE

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
	    fprintf(stderr,"use: client host_name\n");
	    exit(EXIT_FAILURE);
	}
	const char * host_name = argv[1];
	char message[100];
	int rv = recv_message(host_name, message, sizeof message);
	if (rv != 0)
	{
		perror("get_message");
		return EXIT_FAILURE;
	}
	
	fprintf(stdout, "client: received '%s'\n", message);
}
