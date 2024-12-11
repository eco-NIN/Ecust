#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define SHMKEY 75     // 共享内存的键值
#define SHMSIZE 128   // 共享内存大小

int main() {
    int shmid;
    char *shm;
    srand(time(NULL));  // 初始化随机数种子

    // 获取共享内存
    if ((shmid = shmget(SHMKEY, SHMSIZE, 0666)) < 0) {
        perror("shmget failed");
        exit(1);
    }

    // 附接共享内存
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat failed");
        exit(1);
    }

    printf("Client started, sending messages to server...\n");

    // 向服务器发送数据
    while (1) {
        if (*shm == -1) {  // 检查是否可以发送
            int message = rand() % 10; // 随机生成一个0到9的消息
            *shm = message;  // 发送消息
            printf("Client has sent message to Server! Message: %d\n", message);

            if (message == 0) {  // 如果消息是0，表示结束
                break;
            }
        }
        usleep(100000); // 延迟避免CPU空转
    }

    // 断开共享内存
    if (shmdt(shm) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    printf("Client has finished.\n");
    return 0;
}
