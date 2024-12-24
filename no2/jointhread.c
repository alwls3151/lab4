#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *join_thread(void *arg) {
    pthread_exit(arg); /* 또는 return arg; */
}

int main(int argc, char *argv[]) {
    pthread_t tid;
    int arg, status;
    void *result;

    if (argc < 2) {
        fprintf(stderr, "Usage: jointhread <number>\n");
        exit(1);
    }

    arg = atoi(argv[1]);

    /* 쓰레드 생성 */
    status = pthread_create(&tid, NULL, join_thread, (void *)(long)arg);
    if (status != 0) {
        fprintf(stderr, "Creating thread: %d\n", status);
        exit(1);
    }

    /* 쓰레드 종료 상태 수집 */
    pthread_join(tid, &result);

    /* 반환값을 종료 코드로 사용 */
    return (int)(long)result;
}
