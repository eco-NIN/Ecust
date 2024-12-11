#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>

#define MSGKEY 75  // ��Ϣ���еĹؼ���

// ��Ϣ�ṹ��
struct msgbuf {
    long mtype;  // ��Ϣ����
    char mtext[100];  // ��Ϣ����
};

void server_process() {
    int msgid;
    struct msgbuf message;

    // ��ȡ��Ϣ����
    msgid = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // ����Ϣ�����н�����Ϣ
        if (msgrcv(msgid, &message, sizeof(message.mtext), 0, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("Server has received message from Client!\n");
        printf("Message Type: %ld, Message Text: %s\n", message.mtype, message.mtext);

        // ������յ�����Ϊ1����Ϣ�����˳�
        if (message.mtype == 1) {
            break;
        }
    }

    // ɾ����Ϣ����
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }

    printf("Server process exiting...\n");
}

void client_process() {
    int msgid;
    struct msgbuf message;

    // ��ȡ��Ϣ����
    msgid = msgget(MSGKEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    for (int i = 10; i >= 1; i--) {
        message.mtype = i;
        snprintf(message.mtext, sizeof(message.mtext), "Message %d", i);

        // ������Ϣ����Ϣ����
        if (msgsnd(msgid, &message, strlen(message.mtext) + 1, 0) == -1) {
            perror("msgsnd");
            exit(1);
        }

        printf("Client has sent message to Server!\n");
    }

    // ���ͽ����źţ���Ϣ����Ϊ1
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

    // �����ӽ��� server �� client
    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // �ӽ���ִ�� client
        client_process();
    } else {
        // ������ִ�� server
        server_process();
    }

    return 0;
}
