/**************************************************
**文件名称：通过TC35I接收英文短信
**功能描述:接收英文短信步骤：
**                          1.发送AT指令
**                          2.发送AT+CPMS=\"MT\",\"MT\",\"MT\" 设置所有操作都在MT（模块终端）中进行
**                          3.发送AT+CNMI=2，1设置当有新短信到来时提示
**                          4.发送AT+CMGF=1,设置文本模式
**                          5.发送AT+CMGD=1,删除第一条短信
**                          6.判断接收字符串中是否含有“+CMTI”，含有表示有新的短信
**                          7.发送AT+CMGR=1，设置读取信息指令
**                          8.判断接收信息是否一致，做出相应操作
**                          9.发送AT+CMGD=1，删除读取后的信息
**作者：李松泽
**时间：2014—11-16
**************************************************/
#include <reg52.h>
#include <string.h>
#include "ds18b20.h"
#include "delay.h"
#include "LCD12864.h"
#include "DHT11.h"
#include "GSM.h"
#include "beep.h"
#define uchar unsigned char
#define uint unsigned int
/******************接收短信指令*****************************/
uchar code AT[]="AT\r";
uchar code CPMS[]={"AT+CPMS=\"MT\",\"MT\",\"MT\"\r"};
uchar code CNMI[]={"AT+CNMI=2,1\r"};
uchar code CMGF[]={"AT+CMGF=1\r"};
uchar code CMGR[]={"AT+CMGR=1\r"};
uchar code CMGD[]={"AT+CMGD=1\r"};
/********************发送中文短信指令***********************/
uchar code MSXZ[]={"AT+CMGF=0\r"};//发送模式选择
uchar code MSCS[]={"AT+CSMP=17,167,0,8\r"} ;//设置PDU模式参数，8表示Unicode编码
uchar code CSCS[]={"AT+CSCS=GSM\r"};
uchar code MBSJ[]={"AT+CMGS=54\r"};//输入PDU长度
uchar code DXNR[]={"0891683108904105F011000B818123999721F50008A7"};//18329979125F   8123999721F5              
uchar code MESS[]={"285F53524D68C06D4B73AF58836E295EA64E3AFF1A"};//当前检测环境温度为：25度  的PDU编�
uchar code MESS3[]={"002C6E7F5EA64E3AFF1A"};
/**********************数字0-9的ＰＤＵ编码****************************/
uchar code num0[]={"0030"};
uchar code num1[]={"0031"};
uchar code num2[]={"0032"};
uchar code num3[]={"0033"};
uchar code num4[]={"0034"};
uchar code num5[]={"0035"};
uchar code num6[]={"0036"};
uchar code num7[]={"0037"};
uchar code num8[]={"0038"};
uchar code num9[]={"0039"};
uchar code MESS1[]={"5EA6"};//度的ＰＤＵ编码
uchar Rec_Buf[40];
uchar i=0;
uint T_Value;
sbit led1=P0^0;
sbit led2=P0^1;

/******************************************
**函数名称:void uart_int(void)
**功能描述:串口初始化
**输入:void
**输出:
******************************************/
void serial_int(void)
{
    SCON=0x50;//串行口工作方式1，REN=1，允许串行口接收数据
    PCON=0x00;//SMOD=0；波特率不加倍
    TMOD|=0x20;//T1方式2
    TH1=0xFD;//装初值
    TL1=0xFD;//装初值
    TR1=1;//启动定时器1
	  ES=1;//开启串口中断
	  EA=1;//开启全局中断
}
/******************************************
**函数名称:void send(uchar *tab)
**功能描述:发送指令数据
**输入:uchar *tab
**输出:
******************************************/
void send(uchar *tab)
{
	  ES=0;
    while((*tab)!='\0')
    {
        SBUF=*tab;
        while(TI==0);
        TI=0;
        tab++;
    }
		ES=1;
}
/******************************************
**函数名称:void CLR_Buf()
**功能描述:清除缓存数据
**输入:无
**输出:
******************************************/
void CLR_Buf()
{
    unsigned char k;
    for(k=0;k<40;k++)
    {
        Rec_Buf[k]=0;
    }
    i=0;
}

/******************************************
**函数名称:bit hand(unsigned char *a)
**功能描述:判断缓存中是否含有指定的字符串
**输入:unsigned char *a
**输出:bit 1———含有   0————————不含有
******************************************/
bit hand(unsigned char *a)
{
    if(strstr(Rec_Buf,a)!=NULL)
			  return 1;
		else
			return 0;
}

/******************************************
**函数名称:void Send_Value()
**功能描述:发送采集数据
**输入:
**输出:
******************************************/
void Send_Value()
{
    uchar bai,shi;
	  T_Value=read_ds18b20temp();
	  bai=T_Value%1000/100;
	  shi=T_Value%100/10;
	  write_LCD_command(0x90);
	  write_LCD_data('T');
	  write_LCD_data(0x30+bai);
	  write_LCD_data(0x30+shi);
	  if(bai==0)
		{
			send(num0);
			delayms(1000);
		}
		  
		if(bai==1)
		{
			send(num1);
			delayms(1000);
		}
		if(bai==2)
			{
			send(num2);
			delayms(1000);
		}
		if(bai==3)
			{
			send(num3);
			delayms(1000);
		}
		if(bai==4)
			{
			send(num4);
			delayms(1000);
		}
		if(bai==5)
			{
			send(num5);
			delayms(1000);
		}
		if(bai==6)
			{
			send(num6);
			delayms(1000);
		}
		if(bai==7)
			{
			send(num7);
			delayms(1000);
		}
		if(bai==8)
			{
			send(num8);
			delayms(1000);
		}
		if(bai==9)
			{
			send(num9);
			delayms(1000);
		}
		
		if(shi==0)
			{
			send(num0);
			delayms(1000);
		}
		if(shi==1)
			{
			send(num1);
			delayms(1000);
		}
		if(shi==2)
			{
			send(num2);
			delayms(1000);
		}
		if(shi==3)
			{
			send(num3);
			delayms(1000);
		}
		if(shi==4)
			{
			send(num4);
			delayms(1000);
		}
		if(shi==5)
			{
			send(num5);
			delayms(1000);
		}
		if(shi==6)
			{
			send(num6);
			delayms(1000);
		}
		if(shi==7)
			{
			send(num7);
			delayms(1000);
		}
		if(shi==8)
			{
			send(num8);
			delayms(1000);
		}
		if(shi==9)
			{
			send(num9);
			delayms(1000);
		}
}
/******************************************
**函数名称:void Send_Value()
**功能描述:发送采集湿度数据
**输入:
**输出:
******************************************/
void Send_Wetness_Value()
{
    uchar bai,shi;
	
	  bai=RH_data/10%10;
	  shi=RH_data%10;
	  //bai=5;
	  //shi=7;
	  write_LCD_command(0x90+4);
	  write_LCD_data('R');
	  write_LCD_data(0x30+bai);
	  write_LCD_data(0x30+shi);
	  if(bai==0)
		{
			send(num0);
			delayms(1000);
		}
		  
		if(bai==1)
		{
			send(num1);
			delayms(1000);
		}
		if(bai==2)
			{
			send(num2);
			delayms(1000);
		}
		if(bai==3)
			{
			send(num3);
			delayms(1000);
		}
		if(bai==4)
			{
			send(num4);
			delayms(1000);
		}
		if(bai==5)
			{
			send(num5);
			delayms(1000);
		}
		if(bai==6)
			{
			send(num6);
			delayms(1000);
		}
		if(bai==7)
			{
			send(num7);
			delayms(1000);
		}
		if(bai==8)
			{
			send(num8);
			delayms(1000);
		}
		if(bai==9)
			{
			send(num9);
			delayms(1000);
		}
		
		if(shi==0)
			{
			send(num0);
			delayms(1000);
		}
		if(shi==1)
			{
			send(num1);
			delayms(1000);
		}
		if(shi==2)
			{
			send(num2);
			delayms(1000);
		}
		if(shi==3)
			{
			send(num3);
			delayms(1000);
		}
		if(shi==4)
			{
			send(num4);
			delayms(1000);
		}
		if(shi==5)
			{
			send(num5);
			delayms(1000);
		}
		if(shi==6)
			{
			send(num6);
			delayms(1000);
		}
		if(shi==7)
			{
			send(num7);
			delayms(1000);
		}
		if(shi==8)
			{
			send(num8);
			delayms(1000);
		}
		if(shi==9)
			{
			send(num9);
			delayms(1000);
		}
}
void Receive_Message_init()
{
    serial_int();//串口初始化
	  CLR_Buf();//清除缓存区
	  while(!hand("OK"))
		{
        send(AT);       // 发送AT指令
        delayms(500);
		}
	  CLR_Buf();
		//led1=0;
		//delayms(1000);
    send(CPMS); //设置所有模式在MT（模块终端）中进行      
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led2=0;
		//delayms(1000);
    send(CNMI);  //设置当有新的短信来时提示     
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led1=1;
		//delayms(1000);
	  send(CMGF);//设置为文本模式       
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led2=1;
		//delayms(1000);
	  send(CMGD); //删除第一条短信      
    delayms(500);
		while(!hand("OK"));
	  CLR_Buf();
		//led1=0;
		//delayms(500);
		LCD_display(0x90,"接收短信Waiting");
		di();
		di();
		di();
		di();
}
/******************************************
**函数名称:void Send_Message()
**功能描述:发送中文短信
**输入:
**输出:
******************************************/
void Send_Message()
{
	  LCD_display(0x90,"                ");
	  LCD_display(0x88,"发送短信中...");
    send(AT);       // 1.发送AT指令
    delayms(500);
    send(MSXZ);     //发送短信模式选择，2.发送AT+CMGF=0  设置PDU模式
    delayms(500);
	  led1 =0;
    send(MSCS);     //3.发送AT+CSMP=17、167、0、8设置PDU模式参数
    delayms(500);
    send(CSCS);     //4.发送AT+CSCS=GSM 设置GSM字符集
    delayms(500);
	  led2=0;
    send(MBSJ);// 5.发送AT+CMGS=40 输入PDU长度
    delayms(500);
    send(DXNR);//发送短信正文,6.发送短信内容，此设置下只能发送中文
    send(MESS);
	  Send_Value();
	  led1=1;
	  send(MESS1);
	  delayms(500);
	  send(MESS3);
	  Send_Wetness_Value();
	  led2=1;
    delayms(500);
    SBUF=0x1A;
    //while(TI==0);//如果TI=0；未发送完。循环等待
    //TI=0;//已发送完。TI清零
		di();
		di();
		LCD_display(0x90,"                ");
		LCD_display(0x88,"                ");
		Receive_Message_init();
}

/******************************************
**函数名称:void Receive_Message()
**功能描述:接收信息并进行数据处理
**输入:
**输出:
******************************************/
void Receive_Message()
{
	  uint j;
   // n=read_ds18b20temp();
	  //write_LCD_command(0x88);
	  //write_LCD_data('T');
	  //write_LCD_data(0x30+n%1000/100);
	  //write_LCD_data(0x30+n%100/10);
		if(strstr(Rec_Buf,"+CMTI")!=NULL)//若缓存字符串中含有 “+CMTI“ 就表示有新的短信
    {
        CLR_Buf();
			  delayms(1000);
				send(CMGR);//读取信息
        delayms(1000);
        if(strstr(Rec_Buf,"report")!=NULL)//如果信息内容为open
				{
            //LCD_display(0x90,"发送短信中");
					  for(j=0;j<8;j++)
					  {
					      led1=0;
					      delayms(500);
				        led1=1;
				        delayms(500);
					  }
            Send_Message();
				}
				if(strstr(Rec_Buf,"openled")!=NULL)//如果信息内容为open
				{
            LED_3W_OPEN();
				}
				if(strstr(Rec_Buf,"closeled")!=NULL)//如果信息内容为open
				{
            LED_3W_CLOSE();
				}
        CLR_Buf();
        delayms(500);
        send(CMGD);
			  while(!hand("OK")); 
	      CLR_Buf();
        Receive_Message_init();				
    }					
	
}
/******************************************
**函数名称:void serial()
**功能描述:串口中断处理
**输入:
**输出:
******************************************/
void serial()interrupt 4
{
    ES=0;
	  if(TI)
		{
        TI=0;
    }
		if(RI)
		{
        RI=0;
			  Rec_Buf[i]=SBUF;
		    i++;
			  if(i>40)
			      i=0;
			  ES=1;
    }
}

