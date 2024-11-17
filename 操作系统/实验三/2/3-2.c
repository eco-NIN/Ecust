#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#define MSGKEY 75  // 消息队列的关键字

// 消息结构体
struct msgbuf {
    long mtype;  // 消息类型
    char mtext[100];  // 消息内容
};

void server_process() {
    int msgid;
    struct msgbuf message;

    // 获取消息队列
    msgid = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // 从消息队列中接收消息
        if (msgrcv(msgid, &message, sizeof(message.mtext), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Server has received message from Client!\n");
        printf("Message Type: %ld, Message Text: %s\n", message.mtype, message.mtext);

        // 如果接收到类型为1的消息，则退出
        if (message.mtype == 1) {
            break;
        }
    }

    // 删除消息队列
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    printf("Server process exiting...\n");
}

void client_process() {
    int msgid;
    struct msgbuf message;

    // 获取消息队列
    msgid = msgget(MSGKEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    for (int i = 10; i >= 1; i--) {
        message.mtype = i;
        snprintf(message.mtext, sizeof(message.mtext), "Message %d", i);

        // 发送消息到消息队列
        if (msgsnd(msgid, &message, strlen(message.mtext) + 1, 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        printf("Client has sent message to Server!\n");
    }

    // 发送结束信号，消息类型为1
    message.mtype = 1;
    snprintf(message.mtext, sizeof(message.mtext), "End of communication");
    if (msgsnd(msgid, &message, strlen(message.mtext) + 1, 0) == -1) {
        perror("msgsnd");
        exit(1);
    }

    printf("Client has sent message to Server!\n");
    printf("Client process exiting...\n");
}

int main() {
    pid_t pid;

    // 创建子进程 server 和 client
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // 子进程执行 client
        client_process();
    } else {
        // 父进程执行 server
        server_process();
    }

    return 0;
}
