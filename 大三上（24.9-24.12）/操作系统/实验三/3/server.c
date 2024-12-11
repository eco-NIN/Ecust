#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHMKEY 75     // �����ڴ�ļ�ֵ
#define SHMSIZE 128   // �����ڴ��С

int main() {
    int shmid;
    char *shm;

    // ���������ڴ�
    if ((shmid = shmget(SHMKEY, SHMSIZE, 0666 | IPC_CREAT)) < 0) {
        perror("shmget failed");
        exit(1);
    }

    // ���ӹ����ڴ�
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat failed");
        exit(1);
    }

    // ��ʼ�������ڴ棬��һ���ֽ�����Ϊ-1����ʾ����
    *shm = -1;

    printf("Server started, waiting for messages...\n");

    while (1) {
        if (*shm != -1) {  // ����Ƿ�����Ϣ
            printf("Server has received message from Client! Message: %d\n", *shm);

            // �����Ϣ��0����ʾ�����ź�
            if (*shm == 0) {
                printf("Server received termination signal, exiting...\n");
                break;
            }

            // ��������Ϣ�󣬽���һ���ֽ���Ϊ-1����ʾ����
            *shm = -1;
        }
        usleep(100000); // �ӳ٣�����CPU��ת
    }

    // ȡ�������ڴ�
    if (shmdt(shm) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    // ɾ�������ڴ�
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }

    printf("Server has finished.\n");
    return 0;
}
