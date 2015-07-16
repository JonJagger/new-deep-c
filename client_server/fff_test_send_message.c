
#include "message.h"
#include "fff.h"
#include <string.h>     // used in fff
#include <assert.h>     // assert()
#include <stdio.h>      // printf()
#include <stdlib.h>     // EXIT_FAILURE
#include <netdb.h>			// struct addrinfo

#define UNUSED(variable)  ((void)(variable))

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

DEFINE_FFF_GLOBALS

FAKE_VALUE_FUNC(int, getaddrinfo, const char *, const char *, const struct addrinfo *, struct addrinfo **)
FAKE_VALUE_FUNC(int, fputs, const char *, FILE *)
FAKE_VALUE_FUNC(int, fputc, int, FILE *)
FAKE_VALUE_FUNC(int, socket, int, int, int)
		
#define FFF_FAKES_LIST(FAKE)  \
  FAKE(getaddrinfo)						\
  FAKE(fputs)                 \
  FAKE(fputc)          				\
  FAKE(socket) 

static void reset()
{
	FFF_FAKES_LIST(RESET_FAKE);
	FFF_RESET_HISTORY();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static int custom_getaddrinfo(
	const char * host_name, 
	const char * server_name, 
	const struct addrinfo * hints,
  struct addrinfo ** res)
{
	UNUSED(host_name);
	UNUSED(server_name);
	UNUSED(res);
	
	assert(hints != NULL);
	assert(hints->ai_family == AF_UNSPEC);
	assert(hints->ai_socktype == SOCK_STREAM);
	assert(hints->ai_flags == AI_PASSIVE);
	
	return getaddrinfo_fake.return_val;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void getaddrinfo_failure_prints_gai_diagnostic_to_stderr()
{
	reset();
	
	getaddrinfo_fake.custom_fake = 	custom_getaddrinfo;
	getaddrinfo_fake.return_val = EAI_AGAIN;
	
	const char message[] = "Hello";
	int rv = send_message(message, sizeof message);
	
	assert(rv == EXIT_FAILURE);
  
	assert(getaddrinfo_fake.call_count == 1);
	assert(fputs_fake.call_count == 2);
	assert(fputc_fake.call_count == 1);
	assert(socket_fake.call_count == 0);

	assert(strcmp("getaddrinfo: ", fputs_fake.arg0_history[0]) == 0);
	assert(fputs_fake.arg1_history[0] == stderr);
	
	assert(strcmp(gai_strerror(getaddrinfo_fake.return_val), fputs_fake.arg0_history[1]) == 0);
	assert(fputs_fake.arg1_history[1] == stderr);
	
	assert('\n' == fputc_fake.arg0_history[0]);		
	assert(fputc_fake.arg1_history[0] == stderr);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int main(void)
{
	getaddrinfo_failure_prints_gai_diagnostic_to_stderr();
	printf("All tests passed\n");
}
