#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

//文件头数据结构
typedef struct
{
	unsigned short Length;
	unsigned char StartColor;
	unsigned char x0;
	unsigned char x1;
	unsigned char x2;
	unsigned char x3;
	unsigned char x4;
} TurnColor_TypeDef;
//文件头BitMap
#define HeadBitMap(bmap) ((TurnColor_TypeDef *)bmap)

HANDLE hCom;
FILE *fp;
unsigned char datas[2048];
unsigned char *bufs = datas + sizeof(TurnColor_TypeDef);
TurnColor_TypeDef *appleHead = (TurnColor_TypeDef *)datas;

int main(void)
{
	char ComName[10];
	char BinName[24] = "badapple.bin";
	int i = 0;
	printf("BIN NAME:");
	scanf("%s", BinName);
	printf("COM NAME:");
	scanf("%s", ComName);
	hCom = CreateFile(TEXT(ComName),				//COM1口
					  GENERIC_READ | GENERIC_WRITE, //允许读
					  0,							//指定共享属性，由于串口不能共享，所以该参数必须为0
					  NULL,
					  OPEN_EXISTING, //打开而不是创建

					  FILE_ATTRIBUTE_NORMAL, //属性描述，该值为FILE_FLAG_OVERLAPPED，表示使用异步I/O，该参数为0，表示同步I/O操作
					  NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		printf("打开COM失败!\n");
		return FALSE;
	}
	else
	{
		printf("COM打开成功！\n");
	}

	SetupComm(hCom, 2048, 2048); //输入缓冲区和输出缓冲区的大小都是1024

	/*********************************超时设置**************************************/
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout = MAXDWORD; //读间隔超时
	TimeOuts.ReadTotalTimeoutMultiplier = 50000; //读时间系数
	TimeOuts.ReadTotalTimeoutConstant = 0;   //读时间常量
	//设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 40; //写时间系数
	TimeOuts.WriteTotalTimeoutConstant = 0;   //写时间常量
	SetCommTimeouts(hCom, &TimeOuts);		  //设置超时

	/*****************************************配置串口***************************/
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 460800;	 //波特率为9600
	dcb.ByteSize = 8;		   //每个字节有8位
	dcb.Parity = NOPARITY;	 //无奇偶校验位
	dcb.StopBits = ONESTOPBIT; //一个停止位
	SetCommState(hCom, &dcb);

	DWORD wCount; //实际读取的字节数
	bool bReadStat;

	unsigned char str = 0;

	/* 打开Bin文件 */
	fp = fopen(BinName, "rb");
	if (fp == NULL)
	{
		printf("Open file badapple.bin error.\n");
		system("PAUSE");
		CloseHandle(hCom);
		return 1;
	}

	while (1)
	{
		/* Read Head */
		fread((unsigned char *)appleHead, sizeof(unsigned char), sizeof(TurnColor_TypeDef), fp);
		if (appleHead->Length)
		{
			/* Read */
			fread(bufs, sizeof(unsigned char), appleHead->Length - sizeof(TurnColor_TypeDef), fp);

			/* 检测串口 */
			//PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //清空缓冲区
			bReadStat = ReadFile(hCom, &str, 1, &wCount, NULL);

			if (!bReadStat)
			{
				printf("读串口失败!");
				return FALSE;
			}
			else
			{
				/* Send */
				WriteFile(hCom, datas, appleHead->Length, &wCount, NULL);
				i++;
				printf("\r%d",str);
			}
		}
		else
		{
			printf("Finish\n");
			system("PAUSE");
			fclose(fp);
			CloseHandle(hCom);
			return 0;
		}
	}

	fclose(fp);
	CloseHandle(hCom);
}