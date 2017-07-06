#ifndef _KEY_H_
#define _KEY_H_
#include "main.h"

#define KEY1_PORT  GPIOB
#define KEY1_PIN   GPIO_Pin_2

#define KEY2_PORT  GPIOB
#define KEY2_PIN   GPIO_Pin_10

#define KEY3_PORT  GPIOB
#define KEY3_PIN   GPIO_Pin_11

#define KEY4_PORT  GPIOB
#define KEY4_PIN   GPIO_Pin_12

#define KEY5_PORT  GPIOB
#define KEY5_PIN   GPIO_Pin_13


#define KEY_POWER_PORT KEY1_PORT
#define KEY_POWER_PIN  KEY1_PIN

#define KEY_MODE_PORT KEY2_PORT
#define KEY_MODE_PIN  KEY2_PIN

#define KEY_VOLUME_PORT KEY3_PORT
#define KEY_VOLUME_PIN  KEY3_PIN

#define KEY_TIMING_PORT KEY4_PORT
#define KEY_TIMING_PIN  KEY4_PIN

#define KEY_LED_PORT KEY5_PORT
#define KEY_LED_PIN  KEY5_PIN


 typedef enum  
{
KEY_STATE_IDLE=0x00,
KEY_STATE_DEBOUNCE,
KEY_STATE_DOWN,
KEY_STATE_HOLD,
KEY_STATE_LONG_HOLD
  }KEY_STATE;


 enum   KEY_TYPE
 {
   KEY_TYPE_NONE=0x00,
   KEY_TYPE_PRESS=0x01,
   KEY_TYPE_LONG_PRESS,
   KEY_TYPE_HOLD,
   KEY_TYPE_LONG_LONG_PRESS,
   KEY_TYPE_LONG_HOLD,
 };
enum  KEY_NUMBER
{
  KEY_NUMBER_NONE=0x00,
  KEY_NUMBER_1,
  KEY_NUMBER_2,
  KEY_NUMBER_3,
  KEY_NUMBER_4,
  KEY_NUMBER_5,
  KEY_NUMBER_6,
  KEY_NUMBER_7,
  KEY_NUMBER_8,
  
};

enum  SW_KEY_STATE
{
SW_KEY_STATE_IDLE,
SW_KEY_STATE_DEBOUNCE,
SW_KEY_STATE_DOWN,
SW_KEY_STATE_UP
};

enum SW_EVENT
{
SW_EVENT_FILTER_DOWN=0x01,
SW_EVENT_FILTER_UP,	
SW_EVENT_DOOR_DOWN,
SW_EVENT_DOOR_UP,
SW_EVENT_BOTH_DOWN,
SW_EVENT_BOTH_UP
};


uint16_t KeyScan(void);
uint8_t  SWKeyDetect(void);
void KeySwitchGPIO_Config(void);
#endif

