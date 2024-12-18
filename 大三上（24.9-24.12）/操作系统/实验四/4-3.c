#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define INVALID -1

#define total_instruction 320  /* 指令流长 */
#define total_vp 32             /* 虚页长 */
#define clear_period 50         /* 清零周期 */

/* 页面结构 */
typedef struct {
    int pn, pfn, counter, time;
} pl_type;

pl_type pl[total_vp];  /* 页面结构数组 */

struct pfc_struct {     /* 页面控制结构 */
    int pn, pfn;
    struct pfc_struct *next;
};

struct pfc_struct pfc[total_vp], *freepf_head, *busypf_head, *busypf_tail;

int diseffect, a[total_instruction];
int page[total_instruction], offset[total_instruction];

void initialize(int total_pf);
void FIFO(int total_pf);
void LRU(int total_pf);
void OPT(int total_pf);
void LFU(int total_pf);
void CLOCK(int total_pf);

int main() {
    int S, i;
    srand(10 * getpid());  /* 使用进程号作为随机数种子 */

    /* 产生指令队列 */
    S = (int)(319.0 * rand() / RAND_MAX) + 1;
    for (i = 0; i < total_instruction; i += 4) {
        a[i] = S;                  /* 任选一指令访问点 */
        a[i + 1] = a[i] + 1;       /* 顺序执行下一条指令 */
        a[i + 2] = (int)(1.0 * a[i] * rand() / RAND_MAX);  /* 执行前地址指令m' */
        a[i + 3] = a[i + 2] + 1;   /* 执行后地址指令 */
        S = (int)(1.0 * rand() * (318 - a[i + 2]) / RAND_MAX) + a[i + 2] + 2;
    }

    /* 将指令序列变换成页地址流 */
    for (i = 0; i < total_instruction; i++) {
        page[i] = a[i] / 10;
        offset[i] = a[i] % 10;
    }

    /* 用户内存工作区从4个页面到32个页面 */
    for (i = 4; i <= 32; i++) {
        printf("%2d page frames\t", i);
        FIFO(i);
        LRU(i);
        OPT(i);
        LFU(i);
        CLOCK(i);
        printf("\n");
    }

    return 0;
}

void initialize(int total_pf) {
    int i;
    diseffect = 0;

    for (i = 0; i < total_vp; i++) {
        pl[i].pn = i;
        pl[i].pfn = INVALID;    /* 置页面控制结构中的页号，页面为空 */
        pl[i].counter = 0;
        pl[i].time = -1;         /* 置页面控制结构中的访问次数，时间为-1 */
    }

    for (i = 1; i < total_pf; i++) {
        pfc[i - 1].next = &pfc[i];
        pfc[i - 1].pfn = i - 1;
    }

    pfc[total_pf - 1].next = NULL;
    pfc[total_pf - 1].pfn = total_pf - 1;
    freepf_head = &pfc[0];  /* 空页面队列的头指针为pfc[0] */
}

void FIFO(int total_pf) {
    int i;
    struct pfc_struct *p;
    initialize(total_pf);   /* 初始化相关页面控制用数据结构 */
    busypf_head = busypf_tail = NULL;  /* 忙页面队列，队列尾链接 */

    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {  /* 页面失效 */
            diseffect += 1;  /* 失效次数 */

            if (freepf_head == NULL) {  /* 无空闲页面 */
                p = busypf_head->next;
                pl[busypf_head->pn].pfn = INVALID;
                freepf_head = busypf_head;  /* 释放忙页面队列中的第一个页面 */
                freepf_head->next = NULL;
                busypf_head = p;
            }

            p = freepf_head->next;  /* 按FIFO方式调新页面入内存页面 */
            freepf_head->next = NULL;
            freepf_head->pn = page[i];
            pl[page[i]].pfn = freepf_head->pfn;

            if (busypf_tail == NULL) {
                busypf_head = busypf_tail = freepf_head;
            } else {
                busypf_tail->next = freepf_head;
                busypf_tail = freepf_head;
            }

            freepf_head = p;
        }
    }

    printf("FIFO: %6.4f ", 1 - (float)diseffect / 320);
}

void LRU(int total_pf) {
    int iMin, minIndex, i, j, present_time;
    initialize(total_pf);
    present_time = 0;

    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {  /* 页面失效 */
            diseffect++;
            if (freepf_head == NULL) {  /* 如果没有空闲页面 */
                iMin = 32765;
                for (j = 0; j < total_vp; j++) {
                    /* 每次都找到最近最久未使用的那个页面， 并且暂存起来 */
                    if (iMin > pl[j].time && pl[j].pfn != INVALID) {
                        iMin = pl[j].time;
                        minIndex = j;
                    }
                }

                freepf_head = &pfc[pl[minIndex].pfn];
                pl[minIndex].pfn = INVALID;
                pl[minIndex].time = -1;
                freepf_head->next = NULL;
            }

            pl[page[i]].pfn = freepf_head->pfn;  /* 有空闲页面则加入内存块 */
            pl[page[i]].time = present_time;
            freepf_head = freepf_head->next;  /* 同时减少一个空闲页面 */
        } else {
            pl[page[i]].time = present_time;
            present_time++;
        }
    }

    printf("LRU: %6.4f ", 1 - (float)diseffect / 320);
}

void OPT(int total_pf) {
    int i, j, max, maxpage, dist[total_vp];
    initialize(total_pf);

    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {  /* 页面失效 */
            diseffect++;
            if (freepf_head == NULL) {
                for (j = 0; j < total_vp; j++) {
                    if (pl[j].pfn != INVALID) {
                        dist[j] = 32767;
                    } else {
                        dist[j] = 0;
                    }
                }

                max = -1;
                for (j = 0; j < total_vp; j++) {
                    if (max < dist[j]) {
                        max = dist[j];
                        maxpage = j;
                    }
                }

                freepf_head = &pfc[pl[maxpage].pfn];
                freepf_head->next = NULL;
                pl[maxpage].pfn = INVALID;
            }

            pl[page[i]].pfn = freepf_head->pfn;
            freepf_head = freepf_head->next;
        }
    }

    printf("OPT: %6.4f ", 1 - (float)diseffect / 320);
}

void LFU(int total_pf) {
    int i, j, min, minpage;
    initialize(total_pf);

    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {  /* 页面失效 */
            diseffect++;
            if (freepf_head == NULL) {  /* 如果没有空闲页面 */
                min = 32767;
                for (j = 0; j < total_vp; j++) {
                    /* 每次都找到最近最少使用的那个页面， 并且暂存起来 */
                    if (min > pl[j].counter && pl[j].pfn != INVALID) {
                        min = pl[j].counter;
                        minpage = j;
                    }
                    pl[j].counter = 0;
                }

                freepf_head = &pfc[pl[minpage].pfn];
                pl[minpage].pfn = INVALID;
            }

            pl[page[i]].pfn = freepf_head->pfn;  /* 有空闲页面则加入内存块 */
            freepf_head = freepf_head->next;  /* 同时减少一个空闲页面 */
        }
    }

    printf("LFU: %6.4f ", 1-( float) diseffect/320);
}

void CLOCK( int total_pf)
{
    int i, j, dp, cont_flag, old_dp;
    pl_type *t;
    initialize( total_pf);
    dp = 0;
    for(i = 0; i < total_instruction; i++)
    {
        if( pl[ page[i]]. pfn == INVALID)  //如果页面失效
        {
            diseffect++;
            if( freepf_head == NULL)  //如果没有空闲页面
            {
                cont_flag = TRUE;
                old_dp = dp;
                while( cont_flag)
                {
                    if( pl[ dp]. counter == 0 && pl[ dp]. pfn != INVALID)cont_flag = FALSE; // 访问位为0时淘汰
                    else{
                        dp++;
                        if( dp == total_vp)dp = 0;
                        if( dp == old_dp) // 访问位为1时将访问位置为0
                            for(j=0; j< total_vp; j++)
                            pl[j]. counter = 0;
                        }
                }
                //依照CLOCK算法置换掉访问位为0被淘汰的页面
                freepf_head = & pfc[ pl[ dp]. pfn];
                pl[ dp]. pfn = INVALID;
                freepf_head-> next = NULL;
            }
            pl[ page[i]]. pfn = freepf_head-> pfn;
            freepf_head = freepf_head-> next;
        }
        else
            pl[ page[i]]. counter = 1;  //命中， 访问次数+1
        if(i % clear_period == 0)
            for(j=0; j< total_vp; j++)
            pl[j]. counter = 0;
    }
    printf("CLOCK: %6.4f", 1-( float) diseffect/320);
}


