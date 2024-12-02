#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// ������̽ṹ��
typedef struct {
    int pid;         // ���̱��
    int arrival;     // ����ʱ��
    int burst;       // ����ʱ��
    int start;       // ��ʼʱ��
    int finish;      // ���ʱ��
    int remaining;   // ʣ��ʱ�䣨RR �ã�
    bool completed;  // ����Ƿ����
} Process;

// ��������
void sortByArrival(Process processes[], int n); // ������ʱ������
void FCFS(Process processes[], int n);         // FCFS �����㷨
void SPN(Process processes[], int n);          // SPN �����㷨
void RR(Process processes[], int n, int quantum); // RR �����㷨
void calculateAndPrintMetrics(Process processes[], int n); // ���㲢��ӡƽ����תʱ���ƽ����Ȩ��תʱ��

int main() {
    int n, quantum;

    // �����������
    printf("�����������: ");
    scanf("%d", &n);

    // ��ʼ������
    Process processes[n];
    for (int i = 0; i < n; i++) {
        printf("������� P%d �ĵ���ʱ��ͷ���ʱ��: ", i + 1);
        processes[i].pid = i + 1; // ���ý��̱��
        scanf("%d %d", &processes[i].arrival, &processes[i].burst);
        processes[i].remaining = processes[i].burst; // ��ʼ��ʣ��ʱ��Ϊ����ʱ��
        processes[i].completed = false; // ���Ϊδ���
    }

    // ����ʱ��Ƭ��С������ RR��
    printf("����ʱ��Ƭ��С (���� RR): ");
    scanf("%d", &quantum);

    // FCFS ����
    printf("\n===== FCFS ���� =====\n");
    FCFS(processes, n);

    // SPN ����
    printf("\n===== SPN ���� =====\n");
    SPN(processes, n);

    // RR ����
    printf("\n===== RR ���� =====\n");
    RR(processes, n, quantum);

    return 0;
}

// ������ʱ������ð������ʵ�֣�
void sortByArrival(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival > processes[j + 1].arrival) {
                // �����������̵�λ��
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// ���㲢��ӡƽ����תʱ���ƽ����Ȩ��תʱ��
void calculateAndPrintMetrics(Process processes[], int n) {
    double total_turnaround = 0, total_weighted_turnaround = 0;

    for (int i = 0; i < n; i++) {
        // ������תʱ��ʹ�Ȩ��תʱ��
        int turnaround = processes[i].finish - processes[i].arrival; // ��תʱ�� = ���ʱ�� - ����ʱ��
        double weighted_turnaround = (double)turnaround / processes[i].burst; // ��Ȩ��תʱ�� = ��תʱ�� / ����ʱ��

        total_turnaround += turnaround; // �ۼ�����תʱ��
        total_weighted_turnaround += weighted_turnaround; // �ۼ��ܴ�Ȩ��תʱ��

        // ���ÿ�����̵Ľ��
        printf("���̺ţ�P%d  ��תʱ�䣺%d  ��Ȩ��תʱ�䣺%.2f\n", processes[i].pid, turnaround, weighted_turnaround);
    }

    // ���ƽ����תʱ���ƽ����Ȩ��תʱ��
    printf("ƽ����תʱ��: %.2f\n", total_turnaround / n);
    printf("ƽ����Ȩ��תʱ��: %.2f\n", total_weighted_turnaround / n);
}

// FCFS ����
void FCFS(Process processes[], int n) {
    sortByArrival(processes, n);  // ������ʱ������

    int current_time = 0; // ��ǰʱ��

    for (int i = 0; i < n; i++) {
        // �����ǰʱ��С�ڽ��̵ĵ���ʱ�䣬��������ǰ�Ŀ���ʱ��
        if (current_time < processes[i].arrival)
            current_time = processes[i].arrival;

        // ���ÿ�ʼʱ������ʱ��
        processes[i].start = current_time;
        processes[i].finish = current_time + processes[i].burst;
        current_time += processes[i].burst; // ���µ�ǰʱ��
    }

    calculateAndPrintMetrics(processes, n); // ���㲢���ָ��
}

// SPN ����
void SPN(Process processes[], int n) {
    sortByArrival(processes, n); // ������ʱ������

    int completed = 0, current_time = 0;
    while (completed < n) {
        int shortest = -1; // ��ǰ�ҵ�����̷���ʱ��Ľ�������
        int shortest_burst = INT_MAX; // ��̷���ʱ���ʼ��Ϊ�����

        // �������н��̣�Ѱ�ҿ������ҷ���ʱ����̵Ľ���
        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && processes[i].arrival <= current_time && processes[i].burst < shortest_burst) {
                shortest = i;
                shortest_burst = processes[i].burst;
            }
        }

        // ���û�п����еĽ��̣���ǰʱ���ƽ�
        if (shortest == -1) {
            current_time++;
        } else {
            // �����ҵ�����̷���ʱ��Ľ���
            processes[shortest].start = current_time;
            processes[shortest].finish = current_time + processes[shortest].burst;
            current_time += processes[shortest].burst; // ���µ�ǰʱ��
            processes[shortest].completed = true; // ���Ϊ�����
            completed++; // ��ɽ����� +1
        }
    }

    calculateAndPrintMetrics(processes, n); // ���㲢���ָ��
}

// RR ����
void RR(Process processes[], int n, int quantum) {
    sortByArrival(processes, n); // ������ʱ������

    int current_time = 0, completed = 0;
    int queue[n], front = 0, rear = 0; // �������

    bool in_queue[n];
    for (int i = 0; i < n; i++) in_queue[i] = false; // ��ʼ������״̬

    // ����һ������Ľ��̼������
    queue[rear++] = 0;
    in_queue[0] = true;

    while (completed < n) {
        // �������Ϊ�գ��ƽ���ǰʱ�䣬������µ���Ľ���
        if (front == rear) {
            current_time++;
            for (int i = 0; i < n; i++) {
                if (!in_queue[i] && processes[i].arrival <= current_time) {
                    queue[rear++] = i; // �������
                    in_queue[i] = true;
                }
            }
            continue;
        }

        // �Ӷ�����ȡ����ǰ����
        int idx = queue[front++];
        if (front == n) front = 0; // ѭ������

        // �����ǰ�������״����У����ÿ�ʼʱ��
        if (processes[idx].remaining == processes[idx].burst) {
            processes[idx].start = current_time;
        }

        // ִ�е�ǰ���̵�ʱ��Ƭ
        if (processes[idx].remaining <= quantum) {
            current_time += processes[idx].remaining;
            processes[idx].remaining = 0; // ���������
            processes[idx].finish = current_time; // �������ʱ��
            processes[idx].completed = true; // ���Ϊ�����
            completed++;
        } else {
            current_time += quantum;
            processes[idx].remaining -= quantum; // ����ʣ��ʱ��
        }

        //�ص㣺Ĭ���ȼ���Ƿ��µ���Ķ��У��ٽ���ǰ�ո�û������Ľ��̼������

        // ����µ���Ľ��̣����������
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] && processes[i].arrival <= current_time && processes[i].remaining > 0) {
                queue[rear++] = i;
                if (rear == n) rear = 0; // ѭ������
                in_queue[i] = true;
            }
        }

        // �����ǰ����δ��ɣ��������¼������
        if (processes[idx].remaining > 0) {
            queue[rear++] = idx;
            if (rear == n) rear = 0; // ѭ������
        }
    }

    calculateAndPrintMetrics(processes, n); // ���㲢���ָ��
}
