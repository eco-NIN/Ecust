#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define INVALID -1

#define total_instruction 320  /* ָ������ */
#define total_vp 32             /* ��ҳ�� */
#define clear_period 50         /* �������� */

/* ҳ��ṹ */
typedef struct {
    int pn, pfn, counter, time;
} pl_type;

pl_type pl[total_vp];  /* ҳ��ṹ���� */

struct pfc_struct {     /* ҳ����ƽṹ */
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
    srand(10 * getpid());  /* ʹ�ý��̺���Ϊ��������� */

    /* ����ָ����� */
    S = (int)(319.0 * rand() / RAND_MAX) + 1;
    for (i = 0; i < total_instruction; i += 4) {
        a[i] = S;                  /* ��ѡһָ����ʵ� */
        a[i + 1] = a[i] + 1;       /* ˳��ִ����һ��ָ�� */
        a[i + 2] = (int)(1.0 * a[i] * rand() / RAND_MAX);  /* ִ��ǰ��ַָ��m' */
        a[i + 3] = a[i + 2] + 1;   /* ִ�к��ַָ�� */
        S = (int)(1.0 * rand() * (318 - a[i + 2]) / RAND_MAX) + a[i + 2] + 2;
    }

    /* ��ָ�����б任��ҳ��ַ�� */
    for (i = 0; i < total_instruction; i++) {
        page[i] = a[i] / 10;
        offset[i] = a[i] % 10;
    }

    /* �û��ڴ湤������4��ҳ�浽32��ҳ�� */
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
        pl[i].pfn = INVALID;    /* ��ҳ����ƽṹ�е�ҳ�ţ�ҳ��Ϊ�� */
        pl[i].counter = 0;
        pl[i].time = -1;         /* ��ҳ����ƽṹ�еķ��ʴ�����ʱ��Ϊ-1 */
    }

    for (i = 1; i < total_pf; i++) {
        pfc[i - 1].next = &pfc[i];
        pfc[i - 1].pfn = i - 1;
    }

    pfc[total_pf - 1].next = NULL;
    pfc[total_pf - 1].pfn = total_pf - 1;
    freepf_head = &pfc[0];  /* ��ҳ����е�ͷָ��Ϊpfc[0] */
}

void FIFO(int total_pf) {
    int i;
    struct pfc_struct *p;
    initialize(total_pf);   /* ��ʼ�����ҳ����������ݽṹ */
    busypf_head = busypf_tail = NULL;  /* æҳ����У�����β���� */

    for (i = 0; i < total_instruction; i++) {
        if (pl[page[i]].pfn == INVALID) {  /* ҳ��ʧЧ */
            diseffect += 1;  /* ʧЧ���� */

            if (freepf_head == NULL) {  /* �޿���ҳ�� */
                p = busypf_head->next;
                pl[busypf_head->pn].pfn = INVALID;
                freepf_head = busypf_head;  /* �ͷ�æҳ������еĵ�һ��ҳ�� */
                freepf_head->next = NULL;
                busypf_head = p;
            }

            p = freepf_head->next;  /* ��FIFO��ʽ����ҳ�����ڴ�ҳ�� */
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
        if (pl[page[i]].pfn == INVALID) {  /* ҳ��ʧЧ */
            diseffect++;
            if (freepf_head == NULL) {  /* ���û�п���ҳ�� */
                iMin = 32765;
                for (j = 0; j < total_vp; j++) {
                    /* ÿ�ζ��ҵ�������δʹ�õ��Ǹ�ҳ�棬 �����ݴ����� */
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

            pl[page[i]].pfn = freepf_head->pfn;  /* �п���ҳ��������ڴ�� */
            pl[page[i]].time = present_time;
            freepf_head = freepf_head->next;  /* ͬʱ����һ������ҳ�� */
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
        if (pl[page[i]].pfn == INVALID) {  /* ҳ��ʧЧ */
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
        if (pl[page[i]].pfn == INVALID) {  /* ҳ��ʧЧ */
            diseffect++;
            if (freepf_head == NULL) {  /* ���û�п���ҳ�� */
                min = 32767;
                for (j = 0; j < total_vp; j++) {
                    /* ÿ�ζ��ҵ��������ʹ�õ��Ǹ�ҳ�棬 �����ݴ����� */
                    if (min > pl[j].counter && pl[j].pfn != INVALID) {
                        min = pl[j].counter;
                        minpage = j;
                    }
                    pl[j].counter = 0;
                }

                freepf_head = &pfc[pl[minpage].pfn];
                pl[minpage].pfn = INVALID;
            }

            pl[page[i]].pfn = freepf_head->pfn;  /* �п���ҳ��������ڴ�� */
            freepf_head = freepf_head->next;  /* ͬʱ����һ������ҳ�� */
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
        if( pl[ page[i]]. pfn == INVALID)  //���ҳ��ʧЧ
        {
            diseffect++;
            if( freepf_head == NULL)  //���û�п���ҳ��
            {
                cont_flag = TRUE;
                old_dp = dp;
                while( cont_flag)
                {
                    if( pl[ dp]. counter == 0 && pl[ dp]. pfn != INVALID)cont_flag = FALSE; // ����λΪ0ʱ��̭
                    else{
                        dp++;
                        if( dp == total_vp)dp = 0;
                        if( dp == old_dp) // ����λΪ1ʱ������λ��Ϊ0
                            for(j=0; j< total_vp; j++)
                            pl[j]. counter = 0;
                        }
                }
                //����CLOCK�㷨�û�������λΪ0����̭��ҳ��
                freepf_head = & pfc[ pl[ dp]. pfn];
                pl[ dp]. pfn = INVALID;
                freepf_head-> next = NULL;
            }
            pl[ page[i]]. pfn = freepf_head-> pfn;
            freepf_head = freepf_head-> next;
        }
        else
            pl[ page[i]]. counter = 1;  //���У� ���ʴ���+1
        if(i % clear_period == 0)
            for(j=0; j< total_vp; j++)
            pl[j]. counter = 0;
    }
    printf("CLOCK: %6.4f", 1-( float) diseffect/320);
}


