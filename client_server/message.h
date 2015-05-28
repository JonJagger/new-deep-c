#ifndef MESSAGE_INCLUDED
#define MESSAGE_INCLUDED

#include <stddef.h> // size_t

int send_message(const char * message, size_t message_size);
int recv_message(const char * host_name, char * message, size_t message_size);

#endif
