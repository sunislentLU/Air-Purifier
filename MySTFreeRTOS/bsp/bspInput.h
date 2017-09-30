#ifndef _BSPINPUT_H_
#define _BSPINPUT_H_
#include "stm32f0xx.h"

typedef struct
{
uint8_t start1;
uint8_t start2;

uint8_t pm1_0H;
uint8_t pm1_0L;

uint8_t pm2_5H;
uint8_t pm2_5L;

uint8_t pm10H;
uint8_t pm10L;

uint8_t airpm1_0H;
uint8_t airpm1_0L;

uint8_t airpm2_5H;
uint8_t airpm2_5L;

uint8_t airpm10H;
uint8_t airpm10L;

uint8_t part0_3H;
uint8_t part0_3L;

uint8_t part0_5H;
uint8_t part0_5L;

uint8_t part1_0H;
uint8_t part01_0L;

uint8_t part2_5H;
uint8_t part2_5L;

uint8_t part5_0H;
uint8_t part5_0L;

uint8_t part10H;
uint8_t part10L;

uint8_t version;
uint8_t error;
uint8_t chksumH;
uint8_t chksumL;
}_sDUSTVALUE;



void KeySwitchGPIO_Config(void);
void DustGpioInit(void);
void GasGpioInit(void);
void DustUartInit(void);
void LightLumiGpioInit(void);
void DeviceAdcInit(void);
uint16_t GetDustSensorRawData(void);
uint16_t GetLumiAdcValue(void);
uint16_t GetGasAdcValue(void);
void SensorI2CGpio_Init(void);
void KeyTimerInit(void);
void FanSpdScanGpioInit(void);
uint16_t GetFanFreq(void);
#endif







