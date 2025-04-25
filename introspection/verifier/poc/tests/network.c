#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Define the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // HTTP port
    inet_pton(AF_INET, "64.71.34.53", &server_addr.sin_addr); // rossbach.to's correct IP


    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return 1;
    }

    // Construct the HTTP GET request
    const char *request = "GET / HTTP/1.1\r\n"
                          "Host: rossbach.to\r\n"
                          "Connection: close\r\n"
                          "\r\n";

    // Send the request
    send(sockfd, request, strlen(request), 0);

    // Receive and print the response
    char response[4096];
    int bytes;
    while ((bytes = recv(sockfd, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes] = '\0'; // Null-terminate the response
        printf("%s", response);
    }

    // Clean up
    close(sockfd);
    return 0;
}

