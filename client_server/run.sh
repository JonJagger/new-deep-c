#!/bin/bash

rm -f a.out server client

echo Running the brute force test
gcc -Wall -Wextra -Werror -pedantic brute_test_send_message.c message.c && ./a.out

echo Running the fff test
gcc -Wall -Wextra -Werror -pedantic fff_test_send_message.c message.c && ./a.out

echo Building the server
gcc -Wall -Wextra -Werror -pedantic message.c server.c -o server

echo Building the client
gcc -Wall -Wextra -Werror -pedantic message.c client.c -o client

echo Running the server and client
./server &
./client localhost
