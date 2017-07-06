#ifndef _HDC1080_H_
#define _HDC1080_H_
#include "stm32F0xx.h"

#define HDC1080_ADDR  40
#define HDC1080_TIMEOUT 0x1000
#define HDC1080_CONGIG_ADDR 0x02
#define HDC1080_TEMP_ADDR  0x00
#define HDC1080_HUMI_ADDR  0x01

#define HDC1080_CONFIG  0x3000


typedef enum
{
 RET_OK = 0x00,
 RET_TIMEOUT,
}_HDC1080_RET;


uint8_t GetTempHumi(uint16_t* temp,uint16_t* hum);

#endif

