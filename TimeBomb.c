//可设定定时炸弹程序
//功能：
//按S6~13选择时间，分别为30、60、90、120、150、180、210、240秒
//只需按一次时间选择键，随后按S2以启动，将自动倒计时
//倒计时结束后爆炸，请在安全区域操作，否则会有生命危险


#include <reg52.h>
#include <intrins.h>
#define uint unsigned int
#define uchar unsigned char
sbit BEEP = P2^3;//蜂鸣器低电平有效
sbit WE = P2^7;//数码管位选
sbit DU = P2^6;//数码管段选
uchar KeyValue = 0;//按键值


//共阴数码管段选表0-9
uchar  code tabel[]= {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F,};


void delay(uint z)//毫秒级软件延时
{
	uint x,y;
	for(x = z; x > 0; x--)
		for(y = 114; y > 0 ; y--); 		
} 


//数码管动态显示函数，最高显示255
void display(uchar i)
{
	uchar bai, shi, ge;
	bai = i / 100; //236 / 100  = 2
	shi = i % 100 / 10;	//236 % 100 / 10 = 3
	ge  = i % 10;//236 % 10 =6
	
	//第一位数码管  		
	P0 = 0XFF;//清除断码
	WE = 1;//打开位选锁存器
	P0 = 0XFE; //1111 1110
	WE = 0;//锁存位选数据
	
	DU = 1;//打开段选锁存器
	P0 = tabel[bai];//
	DU = 0;//锁存段选数据
	delay(5);

	//第二位数码管
	P0 = 0XFF;//清除断码
	WE = 1;//打开位选锁存器
	P0 = 0XFD; //1111 1101
	WE = 0;//锁存位选数据
	
	DU = 1;//打开段选锁存器
	P0 = tabel[shi];//
	DU = 0;//锁存段选数据
	delay(5);

	//第三位数码管
	P0 = 0XFF;//清除断码
	WE = 1;//打开位选锁存器
	P0 = 0XFB; //1111 1011
	WE = 0;//锁存位选数据
	
	DU = 1;//打开段选锁存器
	P0 = tabel[ge];//
	DU = 0;//锁存段选数据
	delay(5);
}

//键盘扫描函数。键值从左到右从上到下从0开始以1递增
void KeyScan()
{
	//4*4矩阵键盘扫描
	P3 = 0XF0;//列扫描
	if(P3 != 0XF0)//判断按键是否被按下
	{
		delay(10);//软件消抖10ms
		if(P3 != 0XF0)//判断按键是否被按下
		{
			switch(P3) //判断那一列被按下
			{
				case 0xe0:	KeyValue = 0;	break;//第一列被按下
				case 0xd0:	KeyValue = 1;	break;//第二列被按下
				case 0xb0:	KeyValue = 2;	break;//第三列被按下
				case 0x70:	KeyValue = 3;	break;//第四列被按下 
			}
			P3 = 0X0F;//行扫描
			switch(P3) //判断那一行被按下
			{
				case 0x0e:	KeyValue = KeyValue;	break;//第一行被按下
				case 0x0d:	KeyValue = KeyValue + 4;	break;//第二行被按下
				case 0x0b:	KeyValue = KeyValue + 8;	break;//第三行被按下
				case 0x07:	KeyValue = KeyValue + 12;	break;//第四行被按下 
			}
			while(P3 != 0X0F);//松手检测	
		}
	}
	P3 = 0XFF;//独立按键扫描
	if(P3 != 0XFF)
	{
		delay(10);//软件消抖10ms
		if(P3 != 0XFF)
		{
			switch(P3) //判断那一行被按下
			{
				case 0xfe:	KeyValue = 16;	break;//S2被按下
				case 0xfd:	KeyValue = 17;	break;//S3被按下
				case 0xfb:	KeyValue = 18;	break;//S4被按下
				case 0xf7:	KeyValue = 19;	break;//S5被按下 
			}
			while(P3 != 0XFF);//松手检测			
		}	
	}
}


//定时器0初始化
void timer0Init()
{
	TR0 = 1;	 //启动定时器0
	TMOD = 0X01; //定时器工作模式1，16位定时器计数模式
	TH0 = 0x4b;//(65535-46082)/256 高八位
	TL0 = 0xfd; //(65535-46082)%256 定时50ms,50ms=50000μs 50000/1.085=46082 低八位
}

void main()//主函数，负责倒计时、数码管显示、蜂鸣器提示
{	
	uint TimeInput = 0;//键盘输入按键值
	uchar mSec, Sec;//毫秒和秒储存变量
		while(KeyValue!=16)
		{
				
					KeyScan();
					switch(KeyValue)
					{
						case 0:TimeInput = 30; break;//半分钟
						case 1:TimeInput = 60; break;//一分钟
						case 2:TimeInput = 90; break;//一分半
						case 3:TimeInput = 120; break;//两分钟
						case 4:TimeInput = 150; break;//两分半
						case 5:TimeInput = 180; break;//三分钟
						case 6:TimeInput = 210; break;//三分半
						case 7:TimeInput = 240; break;//四分钟
						default:KeyValue = 16;//强制S2以开始倒计时
					}
					display(TimeInput);
				
		}
	KeyValue = 16;
	


	
	
	/*主要过程，倒计时*/
				
	timer0Init();//定时器0初始化
	while(1)
	{
		if(TF0 == 1)//判断是否溢出
		{
			TF0 = 0;//软件清零溢出标志位
			TH0 = 0x4b;
			TL0 = 0xfd; //定时50ms
			mSec++;//50ms到
			if(mSec == 20)
			{
				mSec = 0;
				Sec++;//1秒时间到
				BEEP = 0;
				delay(50);
				BEEP = ~BEEP;
			}					
		}
		display(TimeInput-Sec); //数码管显示函数

		if(Sec > TimeInput)//倒计时结束
		{
			Sec = 0;//秒清零 
			BEEP = 0;//长鸣
						
			WE = 1;//打开位选锁存器
			P0 = 0X00; //选通所有数码管
			WE = 0;//锁存位选数据

			DU = 1;//打开段选锁存器
			P0 = 0Xff;//全亮
			DU = 0;//锁存段选数据
						
			while(1);
		}
	}	
}  