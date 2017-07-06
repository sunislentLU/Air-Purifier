#ifndef _OUTPUTTASK_H_
#define _OUTPUTTASK_H_
#include "main.h"
#define OUTPUT_QUEUE_LEN  0x04

typedef enum
{
 BUZ_TYPE_NONE = 0x00,
 BUZ_TYPE_KEY,
 BUZ_TYPE_CONF,
 BUZ_TYPE_WARN
}_eBUZTYPE;

typedef enum
{
	WIFILED_UCON = 0x00,
	WIFILED_ROUTER,
	WIFILED_CLOUD,
	WIFILED_CONFIG
}_eWIFILED;

typedef struct
{
  _eBUZTYPE buzType;
  _eWIFILED wifiLedType;	
}_sOUTPUT_VAR;

typedef enum
{
OFF = 0x00,
ON,
}_ON_OFF;

#endif 


