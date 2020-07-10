#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

//�ļ�ͷ���ݽṹ
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
//�ļ�ͷBitMap
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
	hCom = CreateFile(TEXT(ComName),				//COM1��
					  GENERIC_READ | GENERIC_WRITE, //�����
					  0,							//ָ���������ԣ����ڴ��ڲ��ܹ������Ըò�������Ϊ0
					  NULL,
					  OPEN_EXISTING, //�򿪶����Ǵ���

					  FILE_ATTRIBUTE_NORMAL, //������������ֵΪFILE_FLAG_OVERLAPPED����ʾʹ���첽I/O���ò���Ϊ0����ʾͬ��I/O����
					  NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		printf("��COMʧ��!\n");
		return FALSE;
	}
	else
	{
		printf("COM�򿪳ɹ���\n");
	}

	SetupComm(hCom, 2048, 2048); //���뻺����������������Ĵ�С����1024

	/*********************************��ʱ����**************************************/
	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout = MAXDWORD; //�������ʱ
	TimeOuts.ReadTotalTimeoutMultiplier = 50000; //��ʱ��ϵ��
	TimeOuts.ReadTotalTimeoutConstant = 0;   //��ʱ�䳣��
	//�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier = 40; //дʱ��ϵ��
	TimeOuts.WriteTotalTimeoutConstant = 0;   //дʱ�䳣��
	SetCommTimeouts(hCom, &TimeOuts);		  //���ó�ʱ

	/*****************************************���ô���***************************/
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 460800;	 //������Ϊ9600
	dcb.ByteSize = 8;		   //ÿ���ֽ���8λ
	dcb.Parity = NOPARITY;	 //����żУ��λ
	dcb.StopBits = ONESTOPBIT; //һ��ֹͣλ
	SetCommState(hCom, &dcb);

	DWORD wCount; //ʵ�ʶ�ȡ���ֽ���
	bool bReadStat;

	unsigned char str = 0;

	/* ��Bin�ļ� */
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

			/* ��⴮�� */
			//PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR); //��ջ�����
			bReadStat = ReadFile(hCom, &str, 1, &wCount, NULL);

			if (!bReadStat)
			{
				printf("������ʧ��!");
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