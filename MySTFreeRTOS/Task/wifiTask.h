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
WIFI_NORMAL_OP       //��֤ PID �� KEY �Ժ�ʼ��������
}_eWIFI_PROCESS;





#endif






