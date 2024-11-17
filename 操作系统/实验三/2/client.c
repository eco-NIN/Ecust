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

    // 获取消息队列的ID
    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // 发送消息
    for (int i = 10; i >= 1; i--) {
        message.mtype = i;
        snprintf(message.mtext, sizeof(message.mtext), "Message of type %d", i);

        // 发送消息
        if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        printf("Client has sent message to Server!\n");
        sleep(1);  // 等待一秒，模拟发送间隔
    }

    return 0;
}
