#ifndef _HARDWARE_H_
#define _HARDWARE_H_
#include "stm32f0xx.h"
#define LED1_PORT    GPIOA
#define LED1_PIN     GPIO_Pin_15

#define LED2_PORT    GPIOB
#define LED2_PIN     GPIO_Pin_3


#define LED3_PORT    GPIOB
#define LED3_PIN     GPIO_Pin_4

#define LED4_PORT    GPIOB
#define LED4_PIN     GPIO_Pin_5

#define LED5_PORT    GPIOB
#define LED5_PIN     GPIO_Pin_6

#define LED6_PORT    GPIOB
#define LED6_PIN     GPIO_Pin_7

#define LED7_PORT    GPIOB
#define LED7_PIN     GPIO_Pin_8

#define LED8_PORT    GPIOB
#define LED8_PIN     GPIO_Pin_9

#define LED9_PORT    GPIOC
#define LED9_PIN     GPIO_Pin_13

#define LED10_PORT    GPIOC
#define LED10_PIN     GPIO_Pin_14

#define POWER_LED_OFF()    GPIO_SetBits(LED1_PORT,LED1_PIN)
#define POWER_LED_ON()   GPIO_ResetBits(LED1_PORT,LED1_PIN)

#define AUTO_LED_OFF()    GPIO_SetBits(LED2_PORT,LED2_PIN)
#define AUTO_LED_ON()   GPIO_ResetBits(LED2_PORT,LED2_PIN)

#define FAST_LED_OFF()    GPIO_SetBits(LED3_PORT,LED3_PIN)
#define FAST_LED_ON()   GPIO_ResetBits(LED3_PORT,LED3_PIN)

#define LOW_LED_OFF()    GPIO_SetBits(LED4_PORT,LED4_PIN)
#define LOW_LED_ON()   GPIO_ResetBits(LED4_PORT,LED4_PIN)

#define MEDIUM_LED_OFF()    GPIO_SetBits(LED5_PORT,LED5_PIN)
#define MEDIUM_LED_ON()   GPIO_ResetBits(LED5_PORT,LED5_PIN)

#define HIGH_LED_OFF()    GPIO_SetBits(LED6_PORT,LED6_PIN)
#define HIGH_LED_ON()   GPIO_ResetBits(LED6_PORT,LED6_PIN)

#define TIM1_LED_OFF()    GPIO_SetBits(LED7_PORT,LED7_PIN)
#define TIM1_LED_ON()   GPIO_ResetBits(LED7_PORT,LED7_PIN)

#define TIM2_LED_OFF()    GPIO_SetBits(LED8_PORT,LED8_PIN)
#define TIM2_LED_ON()   GPIO_ResetBits(LED8_PORT,LED8_PIN)

#define TIM3_LED_OFF()    GPIO_SetBits(LED9_PORT,LED9_PIN)
#define TIM3_LED_ON()   GPIO_ResetBits(LED9_PORT,LED9_PIN)

#define WIFI_LED_ON()    GPIO_ResetBits(LED10_PORT,LED10_PIN)
#define WIFI_LED_OFF()   GPIO_SetBits(LED10_PORT,LED10_PIN)

void InputHardwareInit(void);
void WifiUartInit(void);
void WifiGPIOInit(void);
void HardWareInit(void);

#endif








