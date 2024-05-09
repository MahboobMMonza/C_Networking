#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_LEN 128
#define LOCALHOST "127.0.0.1"
#define PORT 8080
#define PCOL 0

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void communicate(int);

int main() {
    int servfd, result;
    SA_IN servaddr;

    // Set up socket
    servfd = socket(AF_INET, SOCK_STREAM, PCOL);
    if (servfd == -1) {
        perror("main::socket failed");
        return EXIT_FAILURE;
    }
    memset(&servaddr, 0, sizeof(servaddr));

    // assign IP, port address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(LOCALHOST); // local host
    servaddr.sin_port = htons(PORT);

    // Connect to TCP server
    result = connect(servfd, (SA *)&servaddr, sizeof(servaddr));
    if (result == -1) {
        perror("main::connect failed");
        return EXIT_FAILURE;
    }

    printf("Connected to server successfully. Communicating...\n\n");
    // run server operations
    communicate(servfd);

    // Close socket before exiting
    close(servfd);
    printf("Connection closed\n\n");
    return EXIT_SUCCESS;
}

void communicate(int servfd) {
    char buffer[BUF_LEN];
    int result;
    // Serve until told otherwise
    for (;;) {
        memset(buffer, 0, BUF_LEN);
        printf("Enter message: ");
        fflush(stdout);
        // Read is not technically the safest sure, but alas
        result = read(STDIN_FILENO, buffer, BUF_LEN);
        if (result == -1) {
            perror("communicate::read [client input] failed");
        }
        if (!strncmp(buffer, "EXIT", 4)) {
            printf("Client is closing server connection.\n");
            break;
        }
        result = send(servfd, buffer, BUF_LEN, 0);
        if (result == -1) {
            perror("communicate::send failed");
        }
        memset(buffer, 0, BUF_LEN);
        result = recv(servfd, buffer, BUF_LEN, 0);
        if (result == -1) {
            perror("communicate::read [server connection] failed");
        }
        if (result == 0) {
            printf("Server has closed server.\n");
            break;
        }
        printf("Server sent: %s\n", buffer);
    }
}
