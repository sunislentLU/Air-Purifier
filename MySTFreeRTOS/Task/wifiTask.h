#ifndef _WIFITASK_H_
#define _WIFITASK_H_
#include "main.h"


#define WIFI_REC_QUEUE_LEN 0x04
#define WIFI_SND_QUEUE_LEN 0x04


typedef enum
{
WIFI_START = 0x00,
WIFI_CHK_NET , //检查网络窗台
WIFI_VER_PID,        // 连接路由器以后验证产品 ID 和产品 KEY
WIFI_NORMAL_OP       //验证 PID 和 KEY 以后开始正常控制
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






