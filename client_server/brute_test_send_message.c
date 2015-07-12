
#include "message.h"
#include <assert.h>       // assert()
#include <stdio.h>        // printf()
#include <stdlib.h>       // EXIT_FAILURE
#include <netdb.h>				// struct addrinfo

#define UNUSED(variable)  ((void)(variable))

int getaddrinfo(
	const char * host_name, 
	const char * server_name, 
	const struct addrinfo * hints,
  struct addrinfo ** res)
{
	UNUSED(host_name);
	UNUSED(server_name);
	UNUSED(hints);
	UNUSED(res);
  return 42;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	
static void getaddrinfo_failure_prints_gai_diagnostic_to_stderr(void)
{	
	const char message[] = "Hello";
	int rv = send_message(message, sizeof message);	
	assert(rv == EXIT_FAILURE);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(void)
{
	getaddrinfo_failure_prints_gai_diagnostic_to_stderr();
	printf("All tests passed\n");
}
