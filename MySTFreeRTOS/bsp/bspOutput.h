#ifndef _BSPOUTPUT_H_
#define _BSPOUTPUT_H_


#define BUZZER_GPIO_CLK_CMD 
#define BUZZER_GPIO_CLK  

#define BUZZER_GPIO_PORT
#define BUZZER_GPIO_PIN



#define FAN_GPIOCLK_CMD
#define FAN_GPIO_CLK
#define FAN_GPIO_PORT
#define FAN_GPIO_PIN 




#define BUZZER_TIMER_CLK_CMD 
#define BUZZER_TIMER_CLK
#define BUZZER_TIMER
#define BUZZER_TIMER_PERIOD

#define FAN_TIMER_CLK_CMD
#define FAN_TIMER_CLK
#define FAN_TIMER
#define FAN_TIMER_PERIOD




#define BUZZER_MAX_PERIOD  65536 -1 // 48 000 000 /65536/2 = 366Hz  mini frequency is 366Hz
#define BUZZER_PERIOD      6000-1       // 48 000 000 / 6000 = 8000/2 = 4000Hz  buzzer output frequency is 4KHz 

#define LED_RGB_PWM_PERIOD  65536 -1 //led and rgb pwm frequency is 48 000 000 /65536 = 732Hz


#define FAN_PERIOD_PRESCALE 1000   // 48000000/4800 = 10000    0.1us per counter
#define FAN_PERIOD_MAXCOUNT    48000  //0.1us  * 10000 = 1s   1/1 = 1Hz   MIN Frequency is 1Hz 
#define FAN_PERIOD_COUNT   48000

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

#define FILTER_LED_PORT GPIOC
#define FILTER_LED_PIN  GPIO_Pin_15

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

#define FILTER_LED_ON() GPIO_ResetBits(FILTER_LED_PORT,FILTER_LED_PIN)
#define FILTER_LED_OFF() GPIO_SetBits(FILTER_LED_PORT,FILTER_LED_PIN)

typedef struct
{
unsigned char RGB_RCompare;
unsigned char RGB_GCompare;
unsigned char RGB_BCompare;
unsigned char LuminCompare;
unsigned char FilterCompare;
}_sRGBLIGHT;



unsigned char  GetBuzMotorOnOffStatus(void);
unsigned char  GetBuzOnOffStatus(void);
void BuzzerOnOff(unsigned char on);
void MotorPowerCtrl(unsigned char on);
void OutputHardwareInit(void);
void ChangeLightPwm(unsigned short pwm);

#endif









