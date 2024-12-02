#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

// 定义进程结构体
typedef struct {
    int pid;         // 进程编号
    int arrival;     // 到达时间
    int burst;       // 服务时间
    int start;       // 开始时间
    int finish;      // 完成时间
    int remaining;   // 剩余时间（RR 用）
    bool completed;  // 标记是否完成
} Process;

// 函数声明
void sortByArrival(Process processes[], int n); // 按到达时间排序
void FCFS(Process processes[], int n);         // FCFS 调度算法
void SPN(Process processes[], int n);          // SPN 调度算法
void RR(Process processes[], int n, int quantum); // RR 调度算法
void calculateAndPrintMetrics(Process processes[], int n); // 计算并打印平均周转时间和平均带权周转时间

int main() {
    int n, quantum;

    // 输入进程数量
    printf("输入进程数量: ");
    scanf("%d", &n);

    // 初始化进程
    Process processes[n];
    for (int i = 0; i < n; i++) {
        printf("输入进程 P%d 的到达时间和服务时间: ", i + 1);
        processes[i].pid = i + 1; // 设置进程编号
        scanf("%d %d", &processes[i].arrival, &processes[i].burst);
        processes[i].remaining = processes[i].burst; // 初始化剩余时间为服务时间
        processes[i].completed = false; // 标记为未完成
    }

    // 输入时间片大小（用于 RR）
    printf("输入时间片大小 (用于 RR): ");
    scanf("%d", &quantum);

    // FCFS 调度
    printf("\n===== FCFS 调度 =====\n");
    FCFS(processes, n);

    // SPN 调度
    printf("\n===== SPN 调度 =====\n");
    SPN(processes, n);

    // RR 调度
    printf("\n===== RR 调度 =====\n");
    RR(processes, n, quantum);

    return 0;
}

// 按到达时间排序（冒泡排序实现）
void sortByArrival(Process processes[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (processes[j].arrival > processes[j + 1].arrival) {
                // 交换两个进程的位置
                Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

// 计算并打印平均周转时间和平均带权周转时间
void calculateAndPrintMetrics(Process processes[], int n) {
    double total_turnaround = 0, total_weighted_turnaround = 0;

    for (int i = 0; i < n; i++) {
        // 计算周转时间和带权周转时间
        int turnaround = processes[i].finish - processes[i].arrival; // 周转时间 = 完成时间 - 到达时间
        double weighted_turnaround = (double)turnaround / processes[i].burst; // 带权周转时间 = 周转时间 / 服务时间

        total_turnaround += turnaround; // 累加总周转时间
        total_weighted_turnaround += weighted_turnaround; // 累加总带权周转时间

        // 输出每个进程的结果
        printf("进程号：P%d  周转时间：%d  带权周转时间：%.2f\n", processes[i].pid, turnaround, weighted_turnaround);
    }

    // 输出平均周转时间和平均带权周转时间
    printf("平均周转时间: %.2f\n", total_turnaround / n);
    printf("平均带权周转时间: %.2f\n", total_weighted_turnaround / n);
}

// FCFS 调度
void FCFS(Process processes[], int n) {
    sortByArrival(processes, n);  // 按到达时间排序

    int current_time = 0; // 当前时间

    for (int i = 0; i < n; i++) {
        // 如果当前时间小于进程的到达时间，跳过到达前的空闲时间
        if (current_time < processes[i].arrival)
            current_time = processes[i].arrival;

        // 设置开始时间和完成时间
        processes[i].start = current_time;
        processes[i].finish = current_time + processes[i].burst;
        current_time += processes[i].burst; // 更新当前时间
    }

    calculateAndPrintMetrics(processes, n); // 计算并输出指标
}

// SPN 调度
void SPN(Process processes[], int n) {
    sortByArrival(processes, n); // 按到达时间排序

    int completed = 0, current_time = 0;
    while (completed < n) {
        int shortest = -1; // 当前找到的最短服务时间的进程索引
        int shortest_burst = INT_MAX; // 最短服务时间初始化为无穷大

        // 遍历所有进程，寻找可运行且服务时间最短的进程
        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && processes[i].arrival <= current_time && processes[i].burst < shortest_burst) {
                shortest = i;
                shortest_burst = processes[i].burst;
            }
        }

        // 如果没有可运行的进程，当前时间推进
        if (shortest == -1) {
            current_time++;
        } else {
            // 运行找到的最短服务时间的进程
            processes[shortest].start = current_time;
            processes[shortest].finish = current_time + processes[shortest].burst;
            current_time += processes[shortest].burst; // 更新当前时间
            processes[shortest].completed = true; // 标记为已完成
            completed++; // 完成进程数 +1
        }
    }

    calculateAndPrintMetrics(processes, n); // 计算并输出指标
}

// RR 调度
void RR(Process processes[], int n, int quantum) {
    sortByArrival(processes, n); // 按到达时间排序

    int current_time = 0, completed = 0;
    int queue[n], front = 0, rear = 0; // 定义队列

    bool in_queue[n];
    for (int i = 0; i < n; i++) in_queue[i] = false; // 初始化队列状态

    // 将第一个到达的进程加入队列
    queue[rear++] = 0;
    in_queue[0] = true;

    while (completed < n) {
        // 如果队列为空，推进当前时间，并检查新到达的进程
        if (front == rear) {
            current_time++;
            for (int i = 0; i < n; i++) {
                if (!in_queue[i] && processes[i].arrival <= current_time) {
                    queue[rear++] = i; // 加入队列
                    in_queue[i] = true;
                }
            }
            continue;
        }

        // 从队列中取出当前进程
        int idx = queue[front++];
        if (front == n) front = 0; // 循环队列

        // 如果当前进程是首次运行，设置开始时间
        if (processes[idx].remaining == processes[idx].burst) {
            processes[idx].start = current_time;
        }

        // 执行当前进程的时间片
        if (processes[idx].remaining <= quantum) {
            current_time += processes[idx].remaining;
            processes[idx].remaining = 0; // 进程已完成
            processes[idx].finish = current_time; // 设置完成时间
            processes[idx].completed = true; // 标记为已完成
            completed++;
        } else {
            current_time += quantum;
            processes[idx].remaining -= quantum; // 减少剩余时间
        }

        //重点：默认先检查是否新到达的队列，再将当前刚刚没运行完的进程加入队列

        // 检查新到达的进程，并加入队列
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] && processes[i].arrival <= current_time && processes[i].remaining > 0) {
                queue[rear++] = i;
                if (rear == n) rear = 0; // 循环队列
                in_queue[i] = true;
            }
        }

        // 如果当前进程未完成，将其重新加入队列
        if (processes[idx].remaining > 0) {
            queue[rear++] = idx;
            if (rear == n) rear = 0; // 循环队列
        }
    }

    calculateAndPrintMetrics(processes, n); // 计算并输出指标
}
