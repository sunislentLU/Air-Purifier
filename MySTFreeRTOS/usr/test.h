#ifndef _TEST_H_
#define _TEST_H_

//------------一级目录--------------------
typedef enum
{
	TEST_MODE_NONE = 0x00,
	TEST_MODE_NOR,
	TEST_MODE_SPD
}_eTESTTYPE;

//------------二级目录---------------------
typedef enum
{
	TEST_SOFT_VERSION = 0x00,
	TEST_SENSOR_OP,
	TEST_KEY_LED_DIS,
	TEST_TAKE_FACORY
}_eNORMALTYPE;


typedef enum
{
	DIS_LOW_SPD = 0x00,
	DIS_MED_SPD,
	DIS_HIGH_SPD,
	DIS_ALOW_SPD,
	DIS_AMED_SPD,
	DIS_AHIGH_SPD,
	DIS_FAST_SPD
}_eSPEEDTYPE;

//-------------三级目录---------------------
typedef enum
{
	DIS_VER = 0x00,
	DIS_FILTER_TIME,
	DIS_RUN_TIME,
	DIS_WORKTIME,
}_eVERSION;


typedef enum
{
	SEN_DUST = 0x00,
	SEN_GAS,
	SEN_TEMP,
	SEN_HUMI,
	SEN_LUMI
}_eSENSOR;

typedef enum
{
 LED_DIS_ON = 0x00,
 LED_DIS_OFF,
 LED_DIS_STA1,
 LED_DIS_STA2,
 LED_DIS_STA3,
 LED_DIS_STA4,
 LED_DIS_STA5,
 LED_DIS_STA6
}_eLED_DIS;

//-------------------------------------------
typedef void (*func)(unsigned char on);
#endif



