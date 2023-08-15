#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

#define NUM_ATTEMPTS 4

void ping_time(const char *host) {
    struct sockaddr_in server_addr;
    struct hostent *server;
    int sock, attempt;
    struct timeval start_time, end_time;
    long elapsed_time;

    // resolve hostname to IP address
    server = gethostbyname(host);
    if (server == NULL) {
        perror("Failed to resolve hostname");
        return;
    }

    // set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(80);

    // perform connection attempts
    for (attempt = 1; attempt <= NUM_ATTEMPTS; attempt++) {
        // Create socket
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("Failed to create socket");
            return;
        }

        // get current time for start of connection attempt
        gettimeofday(&start_time, NULL);

        // attempt to establish TCP connection
        if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
        } else {
            // get current time for end of connection attempt
            gettimeofday(&end_time, NULL);
            // calculate elapsed time in milliseconds
            elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_usec - start_time.tv_usec) / 1000;
            printf("Attempt %d %ld ms\n", attempt, elapsed_time);
            close(sock);  // close the socket after successful connection
        }

        sleep(1);  // Wait between attempts
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    ping_time(argv[1]);

    return 0;
}
