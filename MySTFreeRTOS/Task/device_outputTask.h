#ifndef _OUTPUTTASK_H_
#define _OUTPUTTASK_H_
#include "main.h"
#define OUTPUT_QUEUE_LEN  0x06

typedef enum
{
	LIGHT_DARK = 0x00,
	LIGHT_BRIGHT
}_eLIGHT_STATE;
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
typedef enum
{
	TIMINGLED_NONE = 0x00,
	TIMINGLED_TYPE_1,
	TIMINGLED_TYPE_2,
	TIMINGLED_TYPE_3
}_eTIMINGLED;

typedef struct
{
  _eBUZTYPE buzType;
  _eWIFILED wifiLedType;	
	_eTIMINGLED timingLedType;
	uint8_t     aqiLevel;
	uint8_t light;
}_sOUTPUT_VAR;

typedef enum
{
OFF = 0x00,
ON,
}_ON_OFF;

#endif 


