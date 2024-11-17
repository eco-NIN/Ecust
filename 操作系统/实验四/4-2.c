#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>  // For getpid()


#define TRUE 1
#define FALSE 0
#define INVALID -1
//#define NULL 0

#define total_instruction 320 /*指令流长*/
#define total_vp 32 /*虚页长*/
#define clear_period 50 /*清零周期*/


//页面类型
typedef struct{ /*页面结构*/
    int pn, pfn, counter, time;//pn为页号，pfn为页面号，counter为一个周期内访问该页面次数，time为访问时间
}pl_type;
pl_type pl[total_vp]; /*页面结构数组*/


struct pfc_struct{ /*页面控制结构*/
    int pn, pfn;
    struct pfc_struct *next;
};
//pfc[total_head]定义用户进程虚页控制结构
//*freepf_head为空页面头的指针
//*busypf_head为忙页面头的指针
//*busypf_tail为忙页面尾的指针
struct pfc_struct pfc[total_vp],*freepf_head, *busypf_head, *busypf_tail;


int diseffect;//页面失效次数
int a[total_instruction];//指令流数据组
int page[total_instruction];//每条指令所属页号
int offset[total_instruction];//每页装入10条指令后取模运算页号偏移值
//int total_pf; 用户进程的内存页面数

void initialize(int total_pf);
void FIFO(int total_pf);
void LRU(int total_pf);
void OPT(int total_pf);
void LFU(int total_pf);
void CLOCK(int total_pf);

int main()
{
    int S,i;
    srand(10*getpid());
    /*由于每次运行时进程号不同，故可用来作为初始化随机数队列的“种子”*/
    S=(int)(319.0*rand()/RAND_MAX)+1;
    for(i=0;i<total_instruction;i+=4) /*产生指令队列*/
    {
        a[i]=S; /*任选一指令访问点*/
        a[i+1]=a[i]+1; /*顺序执行下一条指令*/
        a[i+2]=(int)(1.0*a[i]*rand()/RAND_MAX); /*执行前地址指令m'*/
        a[i+3]=a[i+2]+1; /*执行后地址指令*/
        S=(int)(1.0*rand()*(318-a[i+2])/RAND_MAX)+a[i+2]+2;
    }
    for(i=0;i<total_instruction;i++) /*将指令序列变换成页地址流*/
    {
        page[i]=a[i]/10;
        offset[i]=a[i]%10;
    }
    for(i=4;i<=32;i++) /*用户内存工作区从4个页面到32个页面*/
    {
        //int total_pf; 用户进程的内存页面数
        //此处i为不同的用户进程页面数，计算其命中率
        printf("%2d page frames\t",i);
        FIFO(i);//先进先出
        LRU(i);//最近最少使用算法
        OPT(i);//最优算法
        LFU (i);//最少使用页面算法
        CLOCK(i);//时钟算法
        printf("\n");
     }
     return 0;
}

void initialize(int total_pf) /*初始化相关数据结构*/
{
    int i;
    diseffect=0;
    for(i=0;i<total_vp;i++)
    {
        pl[i].pn=i;
        pl[i].pfn=INVALID; /*置页面控制结构中的页号，页面为空*/
        pl[i].counter=0;
        pl[i].time=-1; /*置页面控制结构中的访问次数，时间为-1*/
    }
    for(i=1;i<total_pf;i++)
    {
        pfc[i-1].next=&pfc[i];
        pfc[i-1].pfn=i-1;
    } /*建立pfc[i-1]和pfc[i]之间的链接*/

    pfc[total_pf-1].next=NULL;
    pfc[total_pf-1].pfn=total_pf-1;
    freepf_head=&pfc[0];
    /*空页面队列的头指针为pfc[0]*/
}

void FIFO(int total_pf)
{
    int i;
    struct pfc_struct *p;
    initialize(total_pf); /*初始化相关页面控制用数据结构*/
    busypf_head=busypf_tail=NULL; /*忙页面队列，队列尾链接*/
    for(i=0;i<total_instruction;i++)
    {
        if(pl[page[i]].pfn==INVALID) /*页面失效*/
        {
            diseffect+=1; /*失效次数*/
            if(freepf_head==NULL) /*无空闲页面*/
            {
                p=busypf_head->next;
                pl[busypf_head->pn].pfn=INVALID;
                freepf_head=busypf_head; /*释放忙页面队列中的第一个页面*/
                freepf_head->next=NULL;
                busypf_head=p;
            }
            p=freepf_head->next; /*按FIFO方式调新页面入内存页面*/
            freepf_head->next=NULL;
            freepf_head->pn=page[i];
            pl[page[i]].pfn=freepf_head->pfn;
            if(busypf_tail==NULL) busypf_head=busypf_tail=freepf_head;
            else
            {
                busypf_tail->next=freepf_head;
                busypf_tail=freepf_head;
            }
            freepf_head=p;
        }
    }

    printf("FIFO:%6.4f ",1-(float)diseffect/320);

}

void LRU(int total_pf)
{
    int i, j, least_used, lru_time;
    initialize(total_pf);  // 初始化
    for(i = 0; i < total_instruction; i++)
    {
        if(pl[page[i]].pfn == INVALID)  // 页面失效
        {
            diseffect++;
            // 查找最少最近使用的页面
            least_used = -1;
            lru_time = 999999;
            for(j = 0; j < total_pf; j++)
            {
                if(pl[j].time < lru_time && pl[j].time != -1)  // 只考虑已加载的页面
                {
                    lru_time = pl[j].time;
                    least_used = j;
                }
            }
            // 替换页面
            pl[least_used].pn = page[i];
            pl[least_used].time = i;  // 更新访问时间
        }
        else
        {
            // 更新当前页面的访问时间
            pl[page[i]].time = i;
        }
    }
    printf("LRU:%6.4f ", 1 - (float)diseffect / 320);
}

void OPT(int total_pf)
{
    int i, j, farthest, opt_index;
    initialize(total_pf);  // 初始化
    for(i = 0; i < total_instruction; i++)
    {
        if(pl[page[i]].pfn == INVALID)  // 页面失效
        {
            diseffect++;
            farthest = -1;
            opt_index = -1;
            // 找出将来最久不再使用的页面
            for(j = 0; j < total_pf; j++)
            {
                int next_use = 999999;
                for(int k = i + 1; k < total_instruction; k++)
                {
                    if(page[k] == pl[j].pn)
                    {
                        next_use = k;
                        break;
                    }
                }
                if(next_use > farthest)
                {
                    farthest = next_use;
                    opt_index = j;
                }
            }
            // 替换最远的页面
            pl[opt_index].pn = page[i];
        }
    }
    printf("OPT:%6.4f ", 1 - (float)diseffect / 320);
}

void LFU(int total_pf)
{
    int i, j, least_used, min_counter;
    initialize(total_pf);  // 初始化
    for(i = 0; i < total_instruction; i++)
    {
        if(pl[page[i]].pfn == INVALID)  // 页面失效
        {
            diseffect++;
            least_used = -1;
            min_counter = 999999;
            for(j = 0; j < total_pf; j++)
            {
                if(pl[j].counter < min_counter)
                {
                    min_counter = pl[j].counter;
                    least_used = j;
                }
            }
            // 替换最少访问的页面
            pl[least_used].pn = page[i];
            pl[least_used].counter = 1;  // 新页面的计数器初始化为1
        }
        else
        {
            // 更新当前页面的访问计数
            pl[page[i]].counter++;
        }
    }
    printf("LFU:%6.4f ", 1 - (float)diseffect / 320);
}

void CLOCK(int total_pf)
{
    int i, hand = 0;
    initialize(total_pf);  // 初始化
    for(i = 0; i < total_instruction; i++)
    {
        if(pl[page[i]].pfn == INVALID)  // 页面失效
        {
            diseffect++;
            while(pl[hand].time != 0)  // 查找时间为0的页面
            {
                pl[hand].time = 0;
                hand = (hand + 1) % total_pf;  // 圆形扫描
            }
            // 替换页面
            pl[hand].pn = page[i];
            pl[hand].time = 1;  // 设置访问位为1
            hand = (hand + 1) % total_pf;  // 移动手指
        }
        else
        {
            // 如果访问该页面，更新访问时间
            pl[page[i]].time = 1;  // 设置访问位为1
        }
    }
    printf("CLOCK:%6.4f ", 1 - (float)diseffect / 320);
}

