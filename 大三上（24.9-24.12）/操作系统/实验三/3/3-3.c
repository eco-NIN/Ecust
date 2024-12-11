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

#define SHMKEY 75  // �����ڴ�Ĺؼ���
#define SHMSIZE 256  // �����ڴ��С
#define SEM_NAME "/shm_sem"  // �ź�������

// �����ڴ�ṹ
struct shared_memory {
    int flag;  // -1: ����, ����: �ѽ�����Ϣ
    int data;  // ��������client ���͵���Ϣ
};

void server_process() {
    // ��ȡ�����ڴ�
    int shm_id = shmget(SHMKEY, SHMSIZE, 0666|IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    // �������ڴ渽�ӵ���ǰ���̵ĵ�ַ�ռ�
    struct shared_memory *shm_ptr = (struct shared_memory*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // ��ʼ���ź���
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    while (1) {
        // �ȴ� client ������Ϣ
        sem_wait(sem);

        if (shm_ptr->flag != -1) {
            printf("Server has received message: %d\n", shm_ptr->data);
            shm_ptr->flag = -1;  // ������ɣ���ձ�־

            if (shm_ptr->data == 0) {
                break;  // ������յ���Ϣ 0���˳� server
            }
        }

        sem_post(sem);  // �ͷ��ź���
        usleep(100000);  // ģ��������������ʱ
    }

    // �Ͽ������ڴ沢ɾ��
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);

    // �ر��ź���
    sem_close(sem);
    sem_unlink(SEM_NAME);

    printf("Server exiting...\n");
}

void client_process() {
    // ��ȡ�����ڴ�
    int shm_id = shmget(SHMKEY, SHMSIZE, 0666|IPC_CREAT);
    if (shm_id == -1) {
        perror("shmget failed");
        exit(1);
    }

    // �������ڴ渽�ӵ���ǰ���̵ĵ�ַ�ռ�
    struct shared_memory *shm_ptr = (struct shared_memory*)shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    // ��ʼ���ź���
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open failed");
        exit(1);
    }

    while (1) {
        // �ȴ� server ����
        sem_wait(sem);

        if (shm_ptr->flag == -1) {
            // ��� server ���У�����Է�����Ϣ
            shm_ptr->data = rand() % 10;  // ������� 0-9
            shm_ptr->flag = 1;  // ���Ϊ��������
            printf("Client has sent message: %d\n", shm_ptr->data);
            sem_post(sem);  // �ͷ��ź���

            if (shm_ptr->data == 0) {
                break;  // �������Ϊ 0���˳� client
            }

            usleep(100000);  // ģ��ͻ��˵ĵȴ�
        } else {
            sem_post(sem);  // �ͷ��ź���
            usleep(100000);  // �ȴ� server ����
        }
    }

    // �Ͽ������ڴ�
    shmdt(shm_ptr);

    // �ر��ź���
    sem_close(sem);

    printf("Client exiting...\n");
}

int main() {
    pid_t server_pid, client_pid;

    // ���������̺������ӽ���
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

    // �����̵ȴ������ӽ����˳�
    wait(NULL);
    wait(NULL);

    printf("Parent process exiting...\n");
    return 0;
}
