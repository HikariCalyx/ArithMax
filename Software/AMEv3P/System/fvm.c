/*******************************************************************************
  ArithMax Open Source Calculator Project
  www.zephray.com
*******************************************************************************/
#include "main.h"
#include "fvm.h"

//Ϊ�˱�������ڴ�й¶�����з�����ڴ汻ͳһ����������һ�ֽ�memblk�Ľṹ�У�
//��ʵ����һ��32*4=128�ֽڴ�С�����飬����ǰ31*4�ֽ��������������ڴ��׵�ַ��
//���4���ֽ�����������һ��memblk�ĵ�ַ������һ��memblk������ʱΪNULL
//ע������memblk��Ϊ��̬���䣬���ܳ��ֲ�����memblk�������

void * *FVM_GlobalBlk;
void * *FVM_CurrBlk;

void FVM_Init()
{
  FVM_GlobalBlk = FVM_Create();
  FVM_SelectBlk(FVM_GlobalBlk);
}

//�����µ�memblk
void * *FVM_Create()
{
        unsigned char i;
        void * * memblk;
        memblk = (void * *)malloc(sizeof(void *)*FVM_BlkSize);
        for (i=0;i<FVM_BlkSize ;i++)
        {
                memblk[i]=NULL;
        }
        return memblk;
}

void FVM_SelectBlk(void * * memblk)
{
        FVM_CurrBlk = memblk;
}
//Ѱ��memblk�ṹ����һ����λ
void * *FVM_FindSlot(void *memblk[])
{
        unsigned char i;
        void *nextblk;
        for (i=0;i<(FVM_BlkSize-1);i++)
                if (memblk[i]==NULL)
                        return &(memblk[i]);
        if (memblk[(FVM_BlkSize-1)]==NULL)
        {
                nextblk = (void *)malloc(sizeof(void *)*FVM_BlkSize);
                memblk[(FVM_BlkSize-1)] = nextblk;
                return FVM_FindSlot((void * *)nextblk);
        }else{
                return FVM_FindSlot((void * *)memblk[(FVM_BlkSize-1)]);
        }
}

void *FVM_Malloc(unsigned int size)
{
        void *mem;
        void * *slot;

        mem = (void *)malloc(size);
        slot = FVM_FindSlot(FVM_CurrBlk);
        *slot = mem;
        return mem;
}

void FVM_FreeBlk(void * *memblk)
{
        unsigned char i;
        for (i=0;i<(FVM_BlkSize-1);i++)
        {
                if (memblk[i]!=NULL)
                {
                        free(memblk[i]);
                        memblk[i]=NULL;
                }
        }
        if (memblk[(FVM_BlkSize-1)]==NULL)
                free((void *)memblk);
        else
                FVM_FreeBlk((void * *)memblk[(FVM_BlkSize-1)]);
}

void FVM_FreeCurr()
{
        FVM_FreeBlk(FVM_CurrBlk);
        FVM_CurrBlk = NULL;
}
