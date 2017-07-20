
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "FreeRTOSConfig.h"
#include "timers.h"
#include "semphr.h"

/* Exported types ------------------------------------------------------------*/
//---------------------------input message define area--------------------------------------
/**********************************
** IMPORTANT 
***********************************/

#define BOOT_START_ADDR  0x08000000
#define BOOT_END_ADDR    0x080027FF
#define BOOT_AREA_SIZE   0x2800

#define APP_START_ADDR   0x08002800
#define APP_END_ADDR     0x0800F7FF
#define APP_AREA_SIZE    0xD000


#define DATA_START_ADDR   0x0800F800
#define DATA_END_ADDR     0x0800FFFF
#define DATA_AREA_SIZE    0x800

#define BOOT_DATA_ADDR    0x0800FC00
#define BOOT_DATA_END_ADDR 0x0800FFFF
#define BOOT_DATA_SIZE    0x400

#define FLASH_VOLUME      0x10000
#define FLASH_START_ADDR  0x08000000
#define FLASH_END_ADDR    0x0800FFFF


#define SPDREF_ADDR     DATA_START_ADDR
#define DUSTREF_ADDR    DATA_START_ADDR + 10
#define GASREF_ADDR     DUSTREF_ADDR + 6
#define AUTOSPDREF_ADDR GASREF_ADDR + 6
#define LUMIREF_ADDR    AUTOSPDREF_ADDR + 6
#define FILTER_ADDR     LUMIREF_ADDR + 4
#define WORKTIME_ADDR   FILTER_ADDR + 4
#define DUSTSEN_ADDR    WORKTIME_ADDR + 2
#define GASBASE_ADDR    DUSTSEN_ADDR + 2
#define FIRMWARE_ADDR   GASBASE_ADDR + 2

#define BOOT_OPTION_ADDR BOOT_DATA_ADDR
#define VERSION_ADDR     BOOT_OPTION_ADDR+4
#define CHECK_RESULT     VERSION_ADDR+4
#define FIRMLEN_ADDR     CHECK_RESULT+16







typedef enum
{
INPUT_MSG_NONE = 0x00,
//---KEY MESSAGE---------
INPUT_MSG_KEY1_PRESS,
INPUT_MSG_KEY2_PRESS,
INPUT_MSG_KEY3_PRESS,
INPUT_MSG_KEY4_PRESS,
INPUT_MSG_KEY5_PRESS,

INPUT_MSG_KEY1_LPRESS,
INPUT_MSG_KEY2_LPRESS,
INPUT_MSG_KEY3_LPRESS,
INPUT_MSG_KEY4_LPRESS,
INPUT_MSG_KEY5_LPRESS,

INPUT_MSG_KEY1_HOLD,
INPUT_MSG_KEY2_HOLD,
INPUT_MSG_KEY3_HOLD,
INPUT_MSG_KEY4_HOLD,
INPUT_MSG_KEY5_HOLD,
//----------------------
INPUT_MSG_DUST,
INPUT_MSG_DUST_SUB,
INPUT_MSG_GAS,
INPUT_MSG_SPEED,
INPUT_MSG_TOP,
INPUT_MSG_LUMIN,
INPUT_MSG_TEMP,
INPUT_MSG_HUMI
}_eINPUTMSG_TYPE;

#define KEY_POWER_PRESS  INPUT_MSG_KEY1_PRESS 
#define KEY_MODE_PRESS  INPUT_MSG_KEY2_PRESS 
#define KEY_VOLUME_PRESS  INPUT_MSG_KEY3_PRESS 
#define KEY_TIMING_PRESS  INPUT_MSG_KEY4_PRESS 
#define KEY_LED_PRESS  INPUT_MSG_KEY5_PRESS 


#define KEY_POWER_LPRESS  INPUT_MSG_KEY1_LPRESS 
#define KEY_MODE_LPRESS  INPUT_MSG_KEY2_LPRESS 
#define KEY_VOLUME_LPRESS  INPUT_MSG_KEY3_LPRESS 
#define KEY_TIMING_LPRESS  INPUT_MSG_KEY4_LPRESS 
#define KEY_LED_LPRESS  INPUT_MSG_KEY5_LPRESS 

#define KEY_POWER_HOLD  INPUT_MSG_KEY1_HOLD
#define KEY_MODE_HOLD  INPUT_MSG_KEY2_HOLD
#define KEY_VOLUME_HOLD  INPUT_MSG_KEY3_HOLD
#define KEY_TIMING_HOLD  INPUT_MSG_KEY4_HOLD
#define KEY_LED_HOLD  INPUT_MSG_KEY5_HOLD





typedef enum
{
MSG_PARAM_NONE = 0x00,// none parameter
MSG_PARAM_CHAR,
MSG_PARAM_UCHAR,
MSG_PARAM_SHORT,
MSG_PARAM_USHORT,
MSG_PARAM_INT,
MSG_PARAM_UINT
}_eMSG_PARAM_TYPE;

typedef struct
{
_eINPUTMSG_TYPE inputMsg;
_eMSG_PARAM_TYPE paramType;
void* inputMsgParam;
}_sINPUT_MSG;

//--------------------------------------------------------
//----------------output message define area--------------
typedef enum
{
 MODE_STANDBY = 0x00,
 MODE_AUTO ,
 MODE_JET,
 MODE_LOW,
 MODE_MEDIUM,
 MODE_HIGH,
 MODE_TEST
}_eMODE;

typedef enum
{
BUZZER_TYPE_NONE = 0x00,
BUZZER_ONE_SHORT,
BUZZER_ONE_LONG,
BUZZER_TWO_SHORT,
BUZZER_TWO_LONG
}_eBUZZER_TYPE;

typedef enum
{
OUTPUT_MSG_NONE = 0x00,
OUTPUT_MSG_MODE,
OUTPUT_MSG_BLUELED,
OUTPUT_MSG_BUZZ_KEY,
OUTPUT_MSG_BUZZ_CONF,
OUTPUT_MSG_BUZZ_WARN,
OUTPUT_MSG_LIGHT,
OUTPUT_MSG_SPEED,
OUTPUT_MSG_RGB
}_eOUTPUTMSG_TYPE;

typedef struct
{
_eOUTPUTMSG_TYPE outputMsg;
_eMSG_PARAM_TYPE paramType;
void* outputMsgParam;
}_sOUTPUT_MSG;
//------------------------------------------------------
//-------------wifi message ----------------
typedef enum
{
WIFI_MSG_NONE = 0x00,
WIFI_MSG_MODE,
WIFI_MSG_DUST,
WIFI_DUST_SUB,
WIFI_MSG_AQI,
WIFI_MSG_TIMING,
WIFI_MSG_FILTER,
WIFI_MSG_LED,
WIFI_MSG_DEV_FAULT,
WIFI_MSG_VER,
WIFI_COVER_STA,
WIFI_MSG_GAS,
WIFI_MSG_LUMI,
WIFI_MSG_SPEED,
WIFI_MSG_SPD_REF,
WIFI_MSG_DUST_REF,
WIFI_MSG_AUTOSPD_REF,
WIFI_MSG_LUMI_REF,
WIFI_DUST_SEN,
WIFI_TVOC_REF,

WIFI_MSG_NET,
WIFI_MSG_UPDATE
}_eWIFIMSG_TYPE;

typedef struct
{
_eWIFIMSG_TYPE wifiMsg;
unsigned char  length;
void* wifiMsgParam;
}_sWIFI_REC_MSG;


typedef enum
{
WIFI_UP_NONE = 0x00,
WIFI_UP_MODE,
WIFI_UP_DUST,
WIFI_UP_DUST_SUB,
WIFI_UP_GAS,
WIFI_UP_BLUE,
WIFI_UP_LOCK,
WIFI_UP_VERSION,
WIFI_UP_SPEED,
WIFI_UP_LED,
WIFI_UP_ALL,
WIFI_SET_CONN,
WIFI_REBOOT,
}_eWIFI_PROP;

typedef struct
{
_eWIFI_PROP propMsg;
//_eMSG_PARAM_TYPE paramType;
//void* wifiPropParam;
}_sWIFI_SND_MSG;



typedef struct
{
uint32_t nextTick;
uint32_t intervalTick;
}_sLOOPTIMER;


typedef enum
{
COVER_STATE_ON = 0x00,
COVER_STATE_OFF
}_eCOVER_STATE;



// 与网页上的顺序要一致
typedef enum
{
 TERM_MODE = 0x00,
 TERM_DUST,
 TERM_DUST_SUB,
 TERM_AQI,
 TERM_TIMING,
 TERM_FILTERLIVE,
 TERM_LED,
 TERM_FAULT, 
 TERM_VERSION,
 TERM_COVER, 
 TERM_TVOC,
 TERM_LUMI,
 TERM_SPEED,
 TERM_SPD_REF,
 TERM_DUST_REF,
 TERM_AUTO_SPD_REF,
 TERM_LUMI_REF,
 TERM_SENSE,
 TERM_TVOC_REF
}_eTERMINATION;

typedef uint8_t* (*getdata) (void);
typedef struct 
{
uint16_t length;
_eMSG_PARAM_TYPE datatype;
getdata  getdatafunc;
}_sTERMI_FORMAT;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

