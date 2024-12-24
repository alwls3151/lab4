#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 9090
#define BUFFER_SIZE 4096

void handle_get_request(int client_socket, const char *path) {
    char response[BUFFER_SIZE];
    FILE *file = fopen(path, "r");

    if (file == NULL) {
        snprintf(response, sizeof(response), "HTTP/1.1 404 Not Found\r\n\r\n404 Not Found");
        send(client_socket, response, strlen(response), 0);
        return;
    }

    snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\n");
    send(client_socket, response, strlen(response), 0);

    while (fgets(response, sizeof(response), file) != NULL) {
        send(client_socket, response, strlen(response), 0);
    }

    fclose(file);
}

void handle_post_request(int client_socket, const char *path, const char *body) {
    char response[BUFFER_SIZE];

    // 실행 가능한 CGI 프로그램인지 확인
    if (access(path, X_OK) == 0) {
        FILE *pipe = popen(path, "w");
        if (pipe != NULL) {
            fprintf(pipe, "%s", body);
            pclose(pipe);

            snprintf(response, sizeof(response), "HTTP/1.1 200 OK\r\n\r\nCGI Program Executed Successfully");
            send(client_socket, response, strlen(response), 0);
        } else {
            snprintf(response, sizeof(response), "HTTP/1.1 500 Internal Server Error\r\n\r\nCGI Execution Failed");
            send(client_socket, response, strlen(response), 0);
        }
    } else {
        snprintf(response, sizeof(response), "HTTP/1.1 404 Not Found\r\n\r\nCGI Program Not Found");
        send(client_socket, response, strlen(response), 0);
    }
}

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    // 파싱
    char method[16], path[256], protocol[16];
    sscanf(buffer, "%s %s %s", method, path, protocol);

    // URL에서 첫 '/' 제거
    if (path[0] == '/') memmove(path, path + 1, strlen(path));

    // HTTP GET 처리
    if (strcmp(method, "GET") == 0) {
        if (strlen(path) == 0) {
            strcpy(path, "index.html");
        }
        handle_get_request(client_socket, path);

    // HTTP POST 처리
    } else if (strcmp(method, "POST") == 0) {
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4; // 헤더 이후의 본문
            handle_post_request(client_socket, path, body);
        } else {
            char response[BUFFER_SIZE];
            snprintf(response, sizeof(response), "HTTP/1.1 400 Bad Request\r\n\r\nInvalid POST Request");
            send(client_socket, response, strlen(response), 0);
        }
    } else {
        char response[BUFFER_SIZE];
        snprintf(response, sizeof(response), "HTTP/1.1 405 Method Not Allowed\r\n\r\nMethod Not Allowed");
        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // 연결 대기
    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Web server is running on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Failed to accept connection");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}
