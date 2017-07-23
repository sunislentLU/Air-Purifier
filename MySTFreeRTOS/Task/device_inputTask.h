#ifndef _INPUTTASK_H_
#define _INPUTTASK_H_
#include "main.h"

#define INPUT_QUEUE_LEN  4
#define INPUT_KEY_NUM   0x05
#define INPUT_KEY_TYPE_MAX 0x03
#define INPUT_DUST_BUF_SIZE 10
#define INPUT_DUST_IGNORE_SIZE  2
#define INPUT_GAS_BUF_SIZE 12
#define INPUT_GAS_IGNORE_SIZE   4
#define INPUT_LUMI_BUF_SIZE 12
#define INPUT_LUMI_IGNORE_SIZE   4
#define INPUT_SPEED_BUF_SIZE 10
#define INPUT_SPEED_IGNORE_SIZE   2
#define INPUT_TEMP_BUF_SIZE 10
#define INPUT_TEMP_IGNORE_SIZE   2
#define INPUT_HUMI_BUF_SIZE 10
#define INPUT_HUMI_IGNORE_SIZE   2

#define INPUT_GASLOOPPERIOD   200  // 100 microsecond   loop period
#define INPUT_LIMILOOPPERIOD  100
#define INPUT_SPEEDLOOPPERIOD 100
#define INPUT_DUSTLOOPPERIOD  100

typedef struct
{
uint8_t keyValue;
uint16_t dustValue;
uint16_t gasValue;
uint16_t lumin;
uint16_t speed;
int8_t temp;
int8_t humi;
_eCOVER_STATE coverState;
}_sINPUTVALUE;

extern void InputHardwareInit(void);
extern uint16_t GetDustSensorRawData(void);
extern uint16_t GetGasAdcValue(void);
extern uint16_t GetLumiAdcValue(void);
extern uint16_t KeyScan(void);
extern uint8_t SwitchScan(void);
uint16_t GetAverPayloadFromBuffer(uint16_t* buffer,uint8_t maxLen,uint8_t ignoreLen);

#endif

