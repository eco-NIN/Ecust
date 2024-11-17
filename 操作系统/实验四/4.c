#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define INVALID -1
#define NULL 0
#define total_instruction 320 /*ָ������*/
#define total_vp 32 /*��ҳ��*/
#define clear_period 50 /*��������*/
typedef struct{ /*ҳ��ṹ*/
 int pn, pfn, counter, time;
 }pl_type;
pl_type pl[total_vp]; /*ҳ��ṹ����*/
struct pfc_struct{ /*ҳ����ƽṹ*/
 int pn, pfn;
 struct pfc_struct *next;
};
struct pfc_struct pfc[total_vp],*freepf_head, *busypf_head, *busypf_tail;
int diseffect, a[total_instruction];
int page[total_instruction], offset[total_instruction];
void initialize();
void FIFO( );
void LRU( );
void OPT( );
void LFU();
void CLOCK( );
int main()
{
 int S,i;
 srand(10*getpid());
 /*����ÿ������ʱ���̺Ų�ͬ���ʿ�������Ϊ��ʼ����������еġ����ӡ�*/
 S=(int)(319.0*rand()/RAND_MAX)+1;
 for(i=0;i<total_instruction;i+=4) /*����ָ�����*/
 {
 a[i]=S; /*��ѡһָ����ʵ�*/
 a[i+1]=a[i]+1; /*˳��ִ����һ��ָ��*/
 a[i+2]=(int)(1.0*a[i]*rand()/RAND_MAX); /*ִ��ǰ��ַָ��m'*/
 a[i+3]=a[i+2]+1; /*ִ�к��ַָ��*/
 S=(int)(1.0*rand()*(318-a[i+2])/RAND_MAX)+a[i+2]+2;
 }
for(i=0;i<total_instruction;i++) /*��ָ�����б任��ҳ��ַ��*/
 {
 page[i]=a[i]/10;
 offset[i]=a[i]%10;
 }
for(i=4;i<=32;i++) /*�û��ڴ湤������4��ҳ�浽32��ҳ��*/
 {
 printf("%2d page frames\t",i);
 FIFO(i);
 LRU(i);
 OPT(i);
 LFU (i);
 CLOCK(i);
 }
 return 0;
}
void initialize(int total_pf) /*��ʼ��������ݽṹ*/
{
 int i;
 diseffect=0;
 for(i=0;i<total_vp;i++)
 {
 pl[i].pn=i;
pl[i].pfn=INVALID; /*��ҳ����ƽṹ�е�ҳ�ţ�ҳ��Ϊ��*/
 pl[i].counter=0;
pl[i].time=-1; /*��ҳ����ƽṹ�еķ��ʴ�����ʱ��Ϊ-1*/
 }
 for(i=1;i<total_pf;i++)
 {
 pfc[i-1].next=&pfc[i];
 pfc[i-1].pfn=i-1;
 } /*����pfc[i-1]��pfc[i]֮�������*/
 pfc[total_pf-1].next=NULL;
 pfc[total_pf-1].pfn=total_pf-1;
 freepf_head=&pfc[0];
 /*��ҳ����е�ͷָ��Ϊpfc[0]*/
}
void FIFO(int total_pf)
{
int i;
struct pfc_struct *p;
initialize(total_pf); /*��ʼ�����ҳ����������ݽṹ*/
busypf_head=busypf_tail=NULL; /*æҳ����У�����β����*/
for(i=0;i<total_instruction;i++)
 {
 if(pl[page[i]].pfn==INVALID) /*ҳ��ʧЧ*/
 {
 diseffect+=1; /*ʧЧ����*/
 if(freepf_head==NULL) /*�޿���ҳ��*/
 {
 p=busypf_head->next;
 pl[busypf_head->pn].pfn=INVALID;
 freepf_head=busypf_head; /*�ͷ�æҳ������еĵ�һ��ҳ��*/
 freepf_head->next=NULL;
 busypf_head=p;
 }
 p=freepf_head->next; /*��FIFO��ʽ����ҳ�����ڴ�ҳ��*/
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
{ }
void OPT(int total_pf)
{ }

void LFU(int total_pf)
{ }
void CLOCK(int total_pf)
{ }
