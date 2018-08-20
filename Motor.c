#include <reg51.h>
#include <intrins.h>
#include <stdio.h>

typedef unsigned int uint;
typedef unsigned char uchar;

/********************引脚定义********************/
sbit LED_1 = P1^0;			//高温报警LED
sbit LED_2 = P1^3;			//低温报警LED
sbit MOTOR_1 = P1^6;		//电机正转
sbit MOTOR_2 = P1^7;		//电机反转
/************************************************/

/********************全局变量********************/
#define LedTime 200					//200 * 1ms = 200ms
volatile uchar LED_1ms = 0;
volatile uchar Rec_Data = 0;
/************************************************/

/********************函数声明********************/
void Timer0Init(void);
void UartInit(void);
void TempProc(void);
/************************************************/

/*********************主函数*********************/
int main(void)
{
	UartInit();
	Timer0Init();
	while(1)
	{
		TempProc();
	}
}
/************************************************/

/*******************计数器0模块*******************/
void Timer0Init(void)
{
	TMOD |= 0X01;	//选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0 = 0xFC;		//给定时器赋初值，定时1ms
	TL0 = 0x18;	
	ET0 = 1;			//打开定时器0中断允许
	EA = 1;				//打开总中断
	TR0 = 1;			//打开定时器
}

void Timer0(void) interrupt 1
{
	TH0 = 0xFC;		//给定时器赋初值，定时1ms
	TL0 = 0x18;
	LED_1ms ++;
}
/************************************************/

/*******************串口通信模块*******************/
void UartInit(void)
{
	SCON = 0x50;		//设置为工作方式1
	TMOD |= 0x20;		//设置计数器工作方式2
	PCON = 0x80;		//波特率加倍
	TH1=0xF3;				//计数器初始值设置，注意波特率是4800的
	TL1=0xF3;
	ES=1;						//打开接收中断
	EA=1;						//打开总中断
	TR1=1;					//打开计数器
}

void Uart(void) interrupt 4
{
	Rec_Data = SBUF;	//出去接收到的数据
	RI = 0;							//清除接收中断标志位
}
/************************************************/

/*******************温度处理模块*******************/
void TempProc(void)
{
	if (LED_1ms >= LedTime)
	{
		LED_1ms = 0;
		if (Rec_Data == 'A')
		{
			MOTOR_1 = 1;
			MOTOR_2 = 0;
			LED_1 = 1;
			LED_2 = ~LED_2;
		}
		else if (Rec_Data == 'B')
		{
			MOTOR_1 = 0;
			MOTOR_2 = 1;
			LED_1 = ~LED_1;
			LED_2 = 1;
		}
		else
		{
			MOTOR_1 = 0;
			MOTOR_2 = 0;
			LED_1 = 1;
			LED_2 = 1;
		}
	}
}
/************************************************/