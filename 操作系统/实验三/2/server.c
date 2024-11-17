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

    // ������Ϣ����
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Server is running...\n");

    while (1) {
        // ������Ϣ
        if (msgrcv(msgid, &message, sizeof(message.mtext), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Server has received message from Client!\n");

        // �ж��Ƿ�Ϊ�����ź�
        if (message.mtype == 1) {
            printf("Server received end signal. Exiting...\n");
            // ɾ����Ϣ����
            msgctl(msgid, IPC_RMID, NULL);
            break;
        }
    }

    return 0;
}
