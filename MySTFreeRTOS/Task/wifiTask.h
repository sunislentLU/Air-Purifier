#ifndef _WIFITASK_H_
#define _WIFITASK_H_
#include "main.h"


#define WIFI_REC_QUEUE_LEN 0x04
#define WIFI_SND_QUEUE_LEN 0x04


typedef enum
{
WIFI_START = 0x00,
WIFI_CHK_NET , //������細̨
WIFI_VER_PID,        // ����·�����Ժ���֤��Ʒ ID �Ͳ�Ʒ KEY
WIFI_NORMAL_OP,       //��֤ PID �� KEY �Ժ�ʼ��������
WIFI_SMART_LINK
}_eWIFI_PROCESS;



typedef enum
{
	TERM_TYPE_CHAR = 0x00,
	TERM_TYPE_SHORT,
	TERM_TYPE_USHORT,
	TERM_TYPE_INT,
	TERM_TYPE_UINT,
	TERM_TYPE_DW,
	TERM_TYPE_UDW,
	TERM_TYPE_FLOAT,
	TERM_TYPE_DOUBLE,
	TERM_TYPE_STR,
	TERM_TYPE_BIN
}_eTERMI_TYPE;

#endif






