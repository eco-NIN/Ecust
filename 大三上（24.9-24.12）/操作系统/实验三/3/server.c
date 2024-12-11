#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHMKEY 75     // 共享内存的键值
#define SHMSIZE 128   // 共享内存大小

int main() {
    int shmid;
    char *shm;

    // 创建共享内存
    if ((shmid = shmget(SHMKEY, SHMSIZE, 0666 | IPC_CREAT)) < 0) {
        perror("shmget failed");
        exit(1);
    }

    // 附接共享内存
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat failed");
        exit(1);
    }

    // 初始化共享内存，第一个字节设置为-1，表示空闲
    *shm = -1;

    printf("Server started, waiting for messages...\n");

    while (1) {
        if (*shm != -1) {  // 检查是否有消息
            printf("Server has received message from Client! Message: %d\n", *shm);

            // 如果消息是0，表示结束信号
            if (*shm == 0) {
                printf("Server received termination signal, exiting...\n");
                break;
            }

            // 处理完消息后，将第一个字节置为-1，表示空闲
            *shm = -1;
        }
        usleep(100000); // 延迟，避免CPU空转
    }

    // 取消共享内存
    if (shmdt(shm) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    // 删除共享内存
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }

    printf("Server has finished.\n");
    return 0;
}
