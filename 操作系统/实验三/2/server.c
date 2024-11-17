#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#define MSGKEY 75  // 消息队列的key值

// 消息结构体
struct msgbuf {
    long mtype;         // 消息类型
    char mtext[100];    // 消息内容
};

int main() {
    key_t key = MSGKEY;
    int msgid;
    struct msgbuf message;

    // 创建消息队列
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Server is running...\n");

    while (1) {
        // 接收消息
        if (msgrcv(msgid, &message, sizeof(message.mtext), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Server has received message from Client!\n");

        // 判断是否为结束信号
        if (message.mtype == 1) {
            printf("Server received end signal. Exiting...\n");
            // 删除消息队列
            msgctl(msgid, IPC_RMID, NULL);
            break;
        }
    }

    return 0;
}
