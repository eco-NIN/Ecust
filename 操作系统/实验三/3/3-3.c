#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

#define SHMKEY 75  // 共享内存的关键字
#define SHMSIZE 256  // 共享内存大小
#define SEM_NAME "/shm_sem"  // 信号量名称

// 共享内存结构
struct shared_memory {
    int flag;  // -1: 空闲, 其他: 已接收消息
    int data;  // 数据区，client 发送的消息
};

void server_process() {
    // 获取共享内存
    int shm_id = shmget(SHMKEY, SHMSIZE, 0666|IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    // 将共享内存附加到当前进程的地址空间
    struct shared_memory *shm_ptr = (struct shared_memory*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // 初始化信号量
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    while (1) {
        // 等待 client 发送消息
        sem_wait(sem);

        if (shm_ptr->flag != -1) {
            printf("Server has received message: %d\n", shm_ptr->data);
            shm_ptr->flag = -1;  // 处理完成，清空标志

            if (shm_ptr->data == 0) {
                break;  // 如果接收到消息 0，退出 server
            }
        }

        sem_post(sem);  // 释放信号量
        usleep(100000);  // 模拟服务器处理的延时
    }

    // 断开共享内存并删除
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);

    // 关闭信号量
    sem_close(sem);
    sem_unlink(SEM_NAME);

    printf("Server exiting...\n");
}

void client_process() {
    // 获取共享内存
    int shm_id = shmget(SHMKEY, SHMSIZE, 0666|IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    // 将共享内存附加到当前进程的地址空间
    struct shared_memory *shm_ptr = (struct shared_memory*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // 初始化信号量
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    while (1) {
        // 等待 server 空闲
        sem_wait(sem);

        if (shm_ptr->flag == -1) {
            // 如果 server 空闲，则可以发送消息
            shm_ptr->data = rand() % 10;  // 随机数据 0-9
            shm_ptr->flag = 1;  // 标记为有新数据
            printf("Client has sent message: %d\n", shm_ptr->data);
            sem_post(sem);  // 释放信号量

            if (shm_ptr->data == 0) {
                break;  // 如果数据为 0，退出 client
            }

            usleep(100000);  // 模拟客户端的等待
        } else {
            sem_post(sem);  // 释放信号量
            usleep(100000);  // 等待 server 处理
        }
    }

    // 断开共享内存
    shmdt(shm_ptr);

    // 关闭信号量
    sem_close(sem);

    printf("Client exiting...\n");
}

int main() {
    pid_t server_pid, client_pid;

    // 创建父进程和两个子进程
    server_pid = fork();
    if (server_pid == 0) {
        server_process();
        exit(0);
    }

    client_pid = fork();
    if (client_pid == 0) {
        client_process();
        exit(0);
    }

    // 父进程等待两个子进程退出
    wait(NULL);
    wait(NULL);

    printf("Parent process exiting...\n");
    return 0;
}
