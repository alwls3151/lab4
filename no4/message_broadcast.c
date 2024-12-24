#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define NUM_CLIENTS 3
#define MESSAGE_SIZE 256

char message_buffer[MESSAGE_SIZE];  // 메시지 버퍼
int message_ready = 0;             // 메시지 준비 플래그
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t message_cond = PTHREAD_COND_INITIALIZER;

void *server_thread(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        // 메시지가 준비될 때까지 대기
        while (!message_ready) {
            pthread_cond_wait(&message_cond, &mutex);
        }

        // 메시지 방송
        printf("Server broadcasting message: %s\n", message_buffer);

        // 메시지 상태 초기화
        message_ready = 0;

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *client_thread(void *arg) {
    int client_id = *(int *)arg;
    char client_message[MESSAGE_SIZE];

    while (1) {
        sleep(rand() % 5 + 1); // 임의 시간 대기

        // 클라이언트 메시지 생성
        snprintf(client_message, MESSAGE_SIZE, "Hello from Client %d", client_id);

        pthread_mutex_lock(&mutex);

        // 메시지 버퍼에 작성
        strncpy(message_buffer, client_message, MESSAGE_SIZE);
        message_ready = 1;

        printf("Client %d sent message: %s\n", client_id, client_message);

        // 서버에게 알림
        pthread_cond_signal(&message_cond);

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t server_tid;
    pthread_t client_tids[NUM_CLIENTS];
    int client_ids[NUM_CLIENTS];

    // 서버 쓰레드 생성
    pthread_create(&server_tid, NULL, server_thread, NULL);

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < NUM_CLIENTS; i++) {
        client_ids[i] = i + 1;
        pthread_create(&client_tids[i], NULL, client_thread, &client_ids[i]);
    }

    // 클라이언트 쓰레드 유지
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(client_tids[i], NULL);
    }

    // 서버 쓰레드 유지 (여기서 종료되지 않음)
    pthread_join(server_tid, NULL);

    return 0;
}
