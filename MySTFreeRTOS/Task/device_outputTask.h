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
 BUZ_TYPE_WARN,
 BUZ_TYPE_TEST1,
 BUZ_TYPE_TEST2,
 BUZ_TYPE_TEST3,
 BUZ_TYPE_TEST4,
 BUZ_TYPE_TEST5
}_eBUZTYPE;

typedef enum
{
	WIFILED_UCON = 0x00,
	WIFILED_ROUTER,
	WIFI_OFFLINE,
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


typedef enum
{
	FILTER_LED_NORMAL = 0x00,
	FILTER_LED_CLR,
	FILTER_LED_WARN
}_eFILTERLED;

typedef struct
{
  _eBUZTYPE buzType;
  _eWIFILED wifiLedType;	
	_eTIMINGLED timingLedType;
	uint8_t     aqiLevel;
	uint8_t light;
	uint8_t filter;
	uint8_t mode;
}_sOUTPUT_VAR;

typedef enum
{
OFF = 0x00,
ON,
}_ON_OFF;

#endif 


