#include "message.h"

#include <netdb.h>				// struct addrinfo
#include <netinet/in.h>		// sockaddr_in, sockaddr_in6
#include <stdio.h>			  // FILE
#include <stdlib.h>				// EXIT_FAILURE
#include <sys/socket.h>		// struct sockaddr, struct socklen_t
#include <sys/types.h>	  // SOCK_STREAM
#include <arpa/inet.h>    // inet_ntop()
#include <unistd.h>				// close()

static void * get_in_addr(struct sockaddr * sa);
static void close_fd(int fd);

int send_message(const char * message, size_t message_size)
{	
	int rv;
	
  struct addrinfo hints =
	{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_flags = AI_PASSIVE // use my IP
	};

	const char * port = "3490"; 
	struct addrinfo * serv_info;
	rv = getaddrinfo(NULL, port, &hints, &serv_info);
	if (rv != 0)
	{
		fputs("getaddrinfo: ", stderr);
		fputs(gai_strerror(rv), stderr);
		fputc('\n', stderr);
    return EXIT_FAILURE;
	}

	// loop through all the results and bind to the first we can
	int sock_fd;
	struct addrinfo * p;
	for (p = serv_info; p; p = p->ai_next)
	{
		sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock_fd == -1)
		{
			perror("socket");
			continue;
		}
		const int yes = 1;	
		rv = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes); 	
		if (rv == -1)
		{
			perror("setsockopt");
			close_fd(sock_fd);			
			continue;
		}
		rv = bind(sock_fd, p->ai_addr, p->ai_addrlen); 
		if (rv == -1)
		{
			perror("bind");
			close_fd(sock_fd);
			continue;
		}
		break;
	}
	freeaddrinfo(serv_info); // all done with this structure
	if (!p)
	{
		fputs("server: failed to bind\n", stderr);
    return EXIT_FAILURE;
	}

  const int backlog = 0; // no pending connections
	rv = listen(sock_fd, backlog); 
	if (rv == -1)
	{
		perror("listen");
		close_fd(sock_fd);		
    return EXIT_FAILURE;
	}

	fputs("server: waiting for connections...\n", stdout);
	
	struct sockaddr_storage client_addr;		
	socklen_t sin_size = sizeof client_addr;
	const int new_fd = accept(sock_fd, (struct sockaddr *)&client_addr, &sin_size);
	if (new_fd == -1)
	{
		perror("accept");
		close_fd(sock_fd);		
    return EXIT_FAILURE;
	}

	char address[INET6_ADDRSTRLEN];
	const char * dst = inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), address, sizeof address);
	if (dst == NULL)
	{
		perror("inet_ntop");
		close_fd(new_fd);				
		close_fd(sock_fd);		
    return EXIT_FAILURE;
	}
		
	fputs("server: got connection from ", stdout);
	fputs(address, stdout);
	fputc('\n', stdout);
	
	const ssize_t num_bytes = send(new_fd, message, message_size, 0);
	if (num_bytes == -1)
	{
		perror("send");
		close_fd(new_fd);		
		close_fd(sock_fd);		
    return EXIT_FAILURE;
	}
	
	close_fd(new_fd);
	close_fd(sock_fd);
	
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int recv_message(const char * host_name, char * message, size_t message_size)
{
	int rv;
	
	struct addrinfo hints =
	{
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM
	};

	struct addrinfo * servinfo;	
	const char * port = "3490";  	
	rv = getaddrinfo(host_name, port, &hints, &servinfo);
	if (rv != 0)
	{
		fputs("getaddrinfo: ", stderr);
		fputs(gai_strerror(rv) ,stderr);
		fputc('\n', stderr);
    return EXIT_FAILURE;
	}

	// loop through all the results and connect to the first we can
	int sock_fd;
	struct addrinfo * p;
	for (p = servinfo; p; p = p->ai_next)
	{
		sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (sock_fd == -1)
		{
			perror("socket");
			continue;
		}
		rv = connect(sock_fd, p->ai_addr, p->ai_addrlen);
		if (rv == -1)
		{
			perror("connect");
			close_fd(sock_fd);
			continue;
		}
		break;
	}
	if (!p)
	{
		fputs("client: failed to connect\n", stderr);
		close_fd(sock_fd);		
    return EXIT_FAILURE;
	}

	char address[INET6_ADDRSTRLEN];
	const char * dst = inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), address, sizeof address);
	if (dst == NULL)
	{
		perror("inet_ntop");
		close_fd(sock_fd);				
    return EXIT_FAILURE;
	}
	freeaddrinfo(servinfo); // all done with this structure

	fputs("client: connecting to ", stdout);
	fputs(address, stdout);
	fputc('\n', stdout);
	
	const int num_bytes = recv(sock_fd, message, message_size - 1, 0);
	if (num_bytes == -1)
	{
	    perror("recv");
			close_fd(sock_fd);							
	    return EXIT_FAILURE;
	}

	message[num_bytes] = '\0';

	close_fd(sock_fd);
	
	return 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void close_fd(int fd)
{
	const int rv = close(fd);
	if (rv != 0)
	{
		perror("close");
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

static void * get_in_addr(struct sockaddr * sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr); // IPv4
	else
  	return &(((struct sockaddr_in6*)sa)->sin6_addr); // IPv6
}

