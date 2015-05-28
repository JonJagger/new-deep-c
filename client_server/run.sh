#!/bin/bash

rm -f a.out server client
gcc -Wall -Wextra -Werror -pedantic test_send_message.c && ./a.out
gcc -Wall -Wextra -Werror -pedantic message.c server.c -o server
gcc -Wall -Wextra -Werror -pedantic message.c client.c -o client
./server &
./client localhost
