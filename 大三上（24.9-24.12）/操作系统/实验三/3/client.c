#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

#define SHMKEY 75     // �����ڴ�ļ�ֵ
#define SHMSIZE 128   // �����ڴ��С

int main() {
    int shmid;
    char *shm;
    srand(time(NULL));  // ��ʼ�����������

    // ��ȡ�����ڴ�
    if ((shmid = shmget(SHMKEY, SHMSIZE, 0666)) < 0) {
        perror("shmget failed");
        exit(1);
    }

    // ���ӹ����ڴ�
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat failed");
        exit(1);
    }

    printf("Client started, sending messages to server...\n");

    // ���������������
    while (1) {
        if (*shm == -1) {  // ����Ƿ���Է���
            int message = rand() % 10; // �������һ��0��9����Ϣ
            *shm = message;  // ������Ϣ
            printf("Client has sent message to Server! Message: %d\n", message);

            if (message == 0) {  // �����Ϣ��0����ʾ����
                break;
            }
        }
        usleep(100000); // �ӳٱ���CPU��ת
    }

    // �Ͽ������ڴ�
    if (shmdt(shm) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    printf("Client has finished.\n");
    return 0;
}
