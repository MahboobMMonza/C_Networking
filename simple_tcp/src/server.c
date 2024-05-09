#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_LEN 128
#define PORT 8080
#define PCOL 0
#define BACKLOG 20

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void serve(int);

int main() {
    int sockfd, connfd, result;
    socklen_t len;
    SA_IN servaddr, cli;
    // Manually sets up socket stuff, idk probably might not be idiomatic
    // Set up IPv4 socket
    sockfd = socket(AF_INET, SOCK_STREAM, PCOL);
    if (sockfd == -1) {
        perror("main::socket failed");
        return EXIT_FAILURE;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    // assign IP, port address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // local host
    servaddr.sin_port = htons(PORT);
    // Bind IP address and verification to socket
    result = bind(sockfd, (SA *)&servaddr, sizeof(servaddr));
    if (result == -1) {
        perror("main::bind failed");
        return EXIT_FAILURE;
    }
    printf("Server set up successfully. Listening...\n");
    // Listen for requests (backlog up to 20)
    result = listen(sockfd, BACKLOG);
    if (result == -1) {
        perror("main::listen failed");
        return EXIT_FAILURE;
    }
    len = sizeof(cli);
    // Accept client data packet and identification
    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd == -1) {
        perror("main::accept failed");
        return EXIT_FAILURE;
    }
    printf("Server accepted client successfully. Serving...\n\n");
    // run server operations
    serve(connfd);
    // Close socket before exiting
    close(sockfd);
    return EXIT_SUCCESS;
}

void serve(int connfd) {
    char buffer[BUF_LEN];
    int result;
    // Serve until told otherwise
    /**
     * Serving involves receiving a message from the user (wait to receive),
     * processing any connection closes, then responding back to the client.
     * If the response contains "EXIT", close the connection.
     */
    for (;;) {
        memset(buffer, 0, BUF_LEN);
        result = recv(connfd, buffer, BUF_LEN, 0);
        if (result == -1) {
            perror("serve::recv [client connection] failed");
        }
        if (result == 0) {
            printf("Client has closed the connection. Closing server...\n");
            break;
        }
        printf("Client sent: %s\n", buffer);
        printf("Enter response: ");
        fflush(stdout);
        memset(buffer, 0, BUF_LEN);
        result = read(STDIN_FILENO, buffer, BUF_LEN);
        if (result == -1) {
            perror("serve::read failed");
        }
        if (!strncmp(buffer, "EXIT", 4)) {
            printf("Server is closing server.\n");
            break;
        }
        result = send(connfd, buffer, BUF_LEN, 0);
        if (result == -1) {
            perror("serve::send failed");
        }
    }
}
