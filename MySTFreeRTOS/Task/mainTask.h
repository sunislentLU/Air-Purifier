#ifndef _MAINTASK_H_
#define _MAINTASK_H_
#include "main.h"
#define DEFALUT_SOFTVER       0x01


#define FAST_MODE_TIME      30//minute


#define DEFAULT_SNZ_SPD_REF    500 // default snooze mode fan speed Unit rpm
#define DEFAULT_JET_SPD_REF    1600 
#define DEFAULT_LOW_SPD_REF    600
#define DEFAULT_MEDIUM_SPD_REF 1000
#define DEFAULT_HIGH_SPD_REF   1500
#define DEFAULT_STANDBY_SPD_REF 0


#define DEFAULT_AUTO_LOW       600
#define DEFAULT_AUTO_MED       1200
#define DEFAULT_AUTO_HIGH      1500


#define DEFAULT_DUST_GOOD_REF  50
#define DEFAULT_DUST_FINE_REF  100
#define DEFAULT_DUST_BAD_REF   300

#define DEFAULT_GAS_GOOD_REF   60
#define DEFAULT_GAS_FINE_REF   30
#define DEFAULT_GAS_BAD_REF   20

#define DEFAULT_LUMIN_DARK    200
#define DEFAULT_LUMIN_LIGHT   400

#define DEFAULT_RESET_HOUR      0
#define DEFAULT_MAX_USE_HOUR   5000

#define DEFAULT_GAS_BASE       1000


#define GAS_WARM_TIME         300//300 //warm up time 180 second
#define GAS_BASE_ADJ_TIME     600 // 10 minute time adjust the base value


#define DEFAULT_MODE           MODE_STANDBY// MODE_AUTO// 
#define DEFAULT_DUSTVALUE      0
#define DEFAULT_DUSTSUBVALUE   0
#define DEFAULT_AQILEVEL       AQI_LEVEL_GOOD
#define DEFAULT_TIMING         TIMING_LEVEL_NONE
#define DEFAULT_LED_STATE      BLUE_LED_ON
#define DEFAULT_FILTER_REMAIN  100
#define DEFAULT_COVER_STATE   COVER_STATE_ON
#define DEFAULT_FAULT          0
#define DEFAULT_DUST_SENS      1
#define DEFAULT_SPEED        {DEFAULT_STANDBY_SPD_REF,DEFAULT_STANDBY_SPD_REF}
#define DEFAULT_GASVALUE      0
#define DEFAULT_LUMIN         0
#define DEFAULT_NETSTATE      NET_IDLE
#define DEFAULT_FILTER_DIS    FILTER_STATE_USEABLE

#define DUST_UP_THRD     20
#define GAS_UP_THRD      80
#define TEMP_UP_THD      2
#define HUMI_UP_THD      10

#define TIMING_LEVEL_0    0
#define TIMING_LEVEL_1    (3600)
#define TIMING_LEVEL_2    (14400)
#define TIMING_LEVEL_3    (28800)

#define FILTER_CNT_PERIOD  1000
#define TIMING_CNT_PERIOD  1000
#define FAULT_CNT_PERIOD   1000

#define MOTOR_FAULT_DEATA 200
#define LEDON_DURATION  10



typedef struct
{
uint16_t currentSpd;// current speed
uint16_t targetSpd;// target speed
}_sSPEED;

typedef enum
{
BLUE_LED_OFF = 0x00,
BLUE_LED_ON
}_eBLUELED;
typedef struct
{
uint8_t second;
uint8_t min;
uint8_t hour;
}_sDATE;

typedef enum
{
FILTER_STATE_USEABLE = 0x00,// filter live time is not expired 
FILTER_STATE_CHANGED,// filter live time is expird need to change
FILTER_STATE_CLEAR,
FILTER_STATE_CONF
}_eFILTER_STATE;


typedef struct
{
uint16_t standbySpdRef;// fan speed of snooze mode 
uint16_t jetSpdRef;// fan speed of jet mode 
uint16_t lowSpdRef;// fan speed of low speed mode 
uint16_t mediumSpdRef;// fan speed of medium speed mode 
uint16_t highSpdRef;// fan speed of high speed mode 
}_sSPEED_REFERENCE;

typedef struct
{
	uint16_t autoLowSpdRef;
	uint16_t autoMedSpdRef;
	uint16_t autoHighSpdRef;
}_sAUTOSPD_REF;

typedef struct
{
uint16_t dustGoodRef;// dust density good reference 
uint16_t dustFineRef;// dust density fine reference not very good but not bad 
uint16_t dustBadRef;// dust density bad reference 
}_sDUST_REFERENCE;

typedef struct
{
uint16_t gasGoodRef;// gas adc good reference 
uint16_t gasFineRef;// gas adc fin reference
uint16_t gasBadRef;// gas adc value bad reference
}_sGAS_REFERENCE;
typedef struct
{
	uint16_t luminDark;
	uint16_t luminLight;
}_sLUMIN_REF;

typedef struct
{
 uint16_t filterHoursCnt;
 uint16_t maxFilterHours;
}_sFILTERLIVE;

typedef struct
{
_sSPEED_REFERENCE speedRef;
_sAUTOSPD_REF  atuoSpdRef;
_sDUST_REFERENCE dustRef;
_sGAS_REFERENCE gasRef;
_sLUMIN_REF lumiRef;
_sFILTERLIVE filterVar;
uint16_t workingTime;
uint16_t dustSen;
uint16_t gasBase;
uint16_t softVersion;
}_sREFERENCE_VALUE;

typedef enum
{
TIMING_LEVEL_NONE = 0x00,
TIMING_1_HOUR,
TIMING_4_HOUR,
TIMING_8_HOUR
}_eTIMING_LEVEL;

typedef enum
{
NET_IDLE = 0x00,
NET_CONNECTABLE,
NET_CONNECTING,
NET_CONNECTED,
NET_CLOUD
}_eNET_STATUS;
typedef enum
{
 AQI_LEVEL_GOOD =0x00,
 AQI_LEVEL_FINE,
 AQI_LEVEL_BAD
}_eAQI_LEVEL;

typedef struct 
{
_eMODE mode;// running mode 
uint16_t dustDensity;// pm2.5
uint16_t dustDensitySub;//pm10
_eAQI_LEVEL  aqiLevel; // aqi levels
_eTIMING_LEVEL timingLevel;// timming value
_eBLUELED blueLed;// blue led on or off
uint8_t filterState;// filter state useable
uint16_t fault;
_eCOVER_STATE topCover;
uint16_t sense;
_sSPEED speed;// running speed
uint16_t gasValue;// current gas value adc value
uint16_t lumin;
uint8_t netStatus;
int16_t temperature;
uint8_t humi;
uint8_t filterDis;
}_sRUNNINGVALUE;

#endif



