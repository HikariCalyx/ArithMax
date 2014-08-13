/*2048*/
#include "g2048.h"
#include "stn.h"
#include "key.h"

int code[4][4]={0,0,0,0,
                0,0,0,0,
                0,0,0,0,
                0,0,0,0};/*��Ϸ�е�16������*/
int temp[5];/*�м����*/
int move=0;/*�ƶ�����*/
int score=0;/*����*/

void print(void)/*��ʾ��Ϸ����*/
{
    int i,j;

    LCD_Clear(0);
    for(i=0;i<=3;i++)
    {
        for(j=0;j<=3;j++)
        {
            if(code[i][j]!=0)
            {
                LCD_Fill(24*j,8*i,24*j+22,8*i+7,1);
                LCD_DispNum_6X8(24*j,8*i,code[i][j],4,0);/*��ʾ���ֺͷָ���*/
            }
        }
    }
    LCD_Update();
}

int add(void)/*���м����������д���*/
{
    int i;
    int t=0;
    int change=0;/*�ж������Ƿ��иı䣬0���䣬1�仯*/

    do
    {
        for(i=0;i<=3;i++)
        {
            if(temp[i]==0)
            {
                if(temp[i]!=temp[i+1])
                    change=1;/*��һ��0���治��0ʱ����ı�*/
                temp[i]=temp[i+1];
                temp[i+1]=0;
            }
        }/*ȥ���м��0*/
        t++;
    }while(t<=3);/*�ظ����*/
    for(i=1;i<=3;i++)
    {
        if(temp[i]==temp[i-1])
        {

            if(temp[i]!=0)
            {
                change=1;/*������������ͬ�������ʱ����ı�*/
                score=score+temp[i];/*�ӷ�*/
            }
            temp[i-1]=temp[i-1]*2;
            temp[i]=0;
        }
    }/*���������ڵ���ͬ����������*/
    do
    {
        for(i=0;i<=3;i++)
        {
            if(temp[i]==0)
            {
                temp[i]=temp[i+1];
                temp[i+1]=0;
            }
        }/*ȥ���м��0*/
        t++;
    }while(t<=3);/*�ظ����*/
    return change;
}

int G2048_main(void)
{
    int gameover=0;/*�ж���Ϸ�Ƿ������1������0����*/
    int i,j,k;
    int change=1;/*�жϸ����е����Ƿ�ı䣬0����*/
    char input;

    srand(Timing);/*��������������*/
    for (i=0;i<4;i++)
      for (j=0;j<4;j++)
        code[i][j]=0;
    score=0;
    move=0;
    while(gameover==0)
    {
        if(change>=1)/*�����������ı�ʱ�������*/
        {
            for (k=0;k<10;k++)
            {
                i=((unsigned)rand())%4;
                j=((unsigned)rand())%4;
                if (code[i][j]==0) break;
            }
            if (code[i][j]!=0)
            {
              i=0;
              j=0;
              while(code[i][j]!=0)
              {
                if (j<3) j++;
                else
                {
                  i++;
                  j=0;
                }
              }
            }
            if(((unsigned)rand())%4==0)
            {
                code[i][j]=4;
            }
            else
            {
                code[i][j]=2;/*���ѡһ���ո�����2��4*/
            }
            move++;/*���Ӵ���*/
        }
        print();/*��ʾ*/
        input=GetKey();/*���뷽��*/

        change=0;
        switch(input)
        {
            case KEY_CTRL_AC:/*�˳�*/
                LCD_Clear(0);
                LCD_String_6X8(0,0,"Exit?",1);
                LCD_String_6X8(0,16,"[AC] = Cancel",1);
                LCD_String_6X8(0,24,"[=]  = OK",1);
                LCD_Update();
                input=GetKey();
                if(input==KEY_CTRL_EXE)
                    return 0;
                break;

            case KEY_CTRL_UP:
            case KEY_CHAR_8:/*��*/
                for(j=0;j<=3;j++)
                {
                    for(i=0;i<=3;i++)
                    {
                        temp[i]=code[i][j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(i=0;i<=3;i++)
                    {
                        code[i][j]=temp[i];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }
                break;

            case KEY_CTRL_LEFT:
            case KEY_CHAR_4:/*��*/
                for(i=0;i<=3;i++)
                {
                    for(j=0;j<=3;j++)
                    {
                        temp[j]=code[i][j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(j=0;j<=3;j++)
                    {
                        code[i][j]=temp[j];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }

                break;

            case KEY_CTRL_DOWN:
            case KEY_CHAR_2:/*��*/
                for(j=0;j<=3;j++)
                {
                    for(i=0;i<=3;i++)
                    {
                        temp[i]=code[3-i][j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(i=0;i<=3;i++)
                    {
                        code[3-i][j]=temp[i];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }
                break;

            case KEY_CTRL_RIGHT:
            case KEY_CHAR_6:/*��*/
                for(i=0;i<=3;i++)
                {
                    for(j=0;j<=3;j++)
                    {
                        temp[j]=code[i][3-j];/*��һ�����Ƶ��м����*/
                    }
                    temp[4]=0;
                    change=change+add();
                    for(j=0;j<=3;j++)
                    {
                        code[i][3-j]=temp[j];/*�Ѵ���õ��м�����ƻ���*/
                    }
                }
                break;
        }
        gameover=1;
        for(i=0;i<=3;i++)
            for(j=0;j<=3;j++)
                if(code[i][j]==0)
                    gameover=0;/*���и��Ӷ���������Ϸ����*/

    }
    LCD_Clear(0);
    LCD_String_6X8(0,0,"Game over!",1);
    LCD_String_6X8(0,8,"Score:",1);
    LCD_String_6X8(0,16,"Move:",1);
    LCD_DispNum_6X8(36,8,score,10,1);
    LCD_DispNum_6X8(30,16,move,11,1);
    LCD_String_6X8(0,24,"Press any key",1);
    LCD_Update();
    GetKey();
    return 0;
}
