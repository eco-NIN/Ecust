#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#define MSGKEY 75  // ��Ϣ���е�keyֵ

// ��Ϣ�ṹ��
struct msgbuf {
    long mtype;         // ��Ϣ����
    char mtext[100];    // ��Ϣ����
};

int main() {
    key_t key = MSGKEY;
    int msgid;
    struct msgbuf message;

    // ��ȡ��Ϣ���е�ID
    msgid = msgget(key, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    // ������Ϣ
    for (int i = 10; i >= 1; i--) {
        message.mtype = i;
        snprintf(message.mtext, sizeof(message.mtext), "Message of type %d", i);

        // ������Ϣ
        if (msgsnd(msgid, &message, sizeof(message.mtext), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        printf("Client has sent message to Server!\n");
        sleep(1);  // �ȴ�һ�룬ģ�ⷢ�ͼ��
    }

    return 0;
}
