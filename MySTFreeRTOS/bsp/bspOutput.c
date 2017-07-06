#include "bspOutput.h"
#include "stm32f0xx.h"
void BuzzerGpioInit(void);
void BuzzLightTimerInit(void);
void LedGpioInit(void);
void FanGpioInit(void);
void FanTimerInit(void);
void RGBLightTimer_Init(void);
void RGBLightPwrGpio_init(void);
void TimerNvicInit(void);
void EnableRGBLEDLight(uint8_t en);
void RgbLightInit(void);
void SetLed1Off(void);
void FanTimerCounterStop(void);
void FanTimerStart(void);
_sRGBLIGHT rgbLightValue;
void OutputHardwareInit(void)
{

    
	RgbLightInit();
	BuzzerGpioInit();
	LedGpioInit();
	FanGpioInit();
	RGBLightPwrGpio_init();
	RGBLightTimer_Init();
	BuzzLightTimerInit();
	FanTimerInit(); 
	RGBLightTimer_Init();
	TimerNvicInit();


}


void RgbLightInit(void)
{
	rgbLightValue.LuminCompare = 0xffff;
	rgbLightValue.RGB_BCompare = 0xffff;
	rgbLightValue.RGB_GCompare = 0xffff;
	rgbLightValue.RGB_RCompare = 0xffff;
}

void BuzzerGpioInit(void)
{
 GPIO_InitTypeDef GPIO_InitStructre;
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_14;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_10MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructre);
 GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_1);
	
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_9;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_10MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructre);
}



void LedGpioInit(void)
{
 GPIO_InitTypeDef GPIO_InitStructre;
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_10MHz;
 GPIO_Init(GPIOC, &GPIO_InitStructre);

 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|\
 	                             GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_10MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructre);

 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_15;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_10MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructre);
 SetLed1Off();

}


void FanGpioInit(void)
{
 GPIO_InitTypeDef GPIO_InitStructre;
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_8;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructre);

 GPIO_PinAFConfig(GPIOA,GPIO_PinSource8, GPIO_AF_2);
 GPIO_PinAFConfig(GPIOA,GPIO_PinSource11, GPIO_AF_2);

 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_15;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructre);
 GPIO_PinAFConfig(GPIOB,GPIO_PinSource15, GPIO_AF_1);

 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin = GPIO_Pin_12;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_NOPULL;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA,&GPIO_InitStructre);
}


void RGBLightPwrGpio_init(void)
{
 GPIO_InitTypeDef GPIO_InitStructre;
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA,&GPIO_InitStructre);

 GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_1);
 GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_1);

 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB,&GPIO_InitStructre);

 GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_1);
 GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_1);

}

uint16_t fanPeriod = 5000;

void FanTimerInit(void)
{    
	TIM_TimeBaseInitTypeDef TIM_TimebaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitstructure;
	TIM_ICInitTypeDef TIM_ICInitstructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	TIM_DeInit(TIM1);
	TIM_TimeBaseStructInit(&TIM_TimebaseInitStructure);
	
	TIM_TimebaseInitStructure.TIM_ClockDivision = 0;
	TIM_TimebaseInitStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimebaseInitStructure.TIM_Period = FAN_PERIOD_MAXCOUNT - 1;// 1Hz 最小频率
	TIM_TimebaseInitStructure.TIM_Prescaler = FAN_PERIOD_PRESCALE;// 最大频率 10KHz  
	TIM_TimebaseInitStructure.TIM_RepetitionCounter = 0;  	
	TIM_TimeBaseInit(TIM1,&TIM_TimebaseInitStructure);
	/*
	output toggle 
	*/
	TIM_ICStructInit(&TIM_ICInitstructure);
	TIM_OCInitstructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitstructure.TIM_Pulse = 5000-1;
	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitstructure.TIM_OCIdleState = TIM_OCPolarity_High;
	TIM_OC1Init(TIM1, &TIM_OCInitstructure);
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Disable);
    TIM_CCxCmd(TIM1,TIM_Channel_1,DISABLE);
	/*
	input capture 
	*/
	TIM_ICInitstructure.TIM_Channel = TIM_Channel_4;
	TIM_ICInitstructure.TIM_ICFilter = 0x00;
	TIM_ICInitstructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICInitstructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitstructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM1, &TIM_ICInitstructure);
	TIM_CCxCmd(TIM1,TIM_Channel_4,ENABLE);
	TIM_CtrlPWMOutputs(TIM1,ENABLE);
	TIM_ITConfig(TIM1,TIM_IT_CC1|TIM_IT_CC4,ENABLE);
	FanTimerCounterStop();
}

uint16_t buzzPeriod = BUZZER_PERIOD;//48 000/6000 = 8KHz /2 =4KHz
uint16_t lightDuty = 12000;
void BuzzLightTimerInit(void)
{    
	TIM_TimeBaseInitTypeDef TIM_TimebaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitstructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15,ENABLE);
	TIM_TimebaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimebaseInitStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimebaseInitStructure.TIM_Period = BUZZER_MAX_PERIOD;// 732Hz
	TIM_TimebaseInitStructure.TIM_Prescaler = 0;
	TIM_TimebaseInitStructure.TIM_RepetitionCounter = 0;  	
	TIM_TimeBaseInit(TIM15,&TIM_TimebaseInitStructure);

	TIM_OCInitstructure.TIM_OCMode = TIM_OCMode_Toggle;
	TIM_OCInitstructure.TIM_Pulse = buzzPeriod;
	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC1Init(TIM15, &TIM_OCInitstructure);
	TIM_OC1PreloadConfig(TIM15,TIM_OCPreload_Disable);

	TIM_OCInitstructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitstructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitstructure.TIM_Pulse = lightDuty;
	TIM_OCInitstructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC2Init(TIM15, &TIM_OCInitstructure);
	TIM_Cmd(TIM15,ENABLE);
	TIM_CtrlPWMOutputs(TIM15,ENABLE);
	TIM_ITConfig(TIM15,TIM_IT_CC1,ENABLE);
}


void RGBLightTimer_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimebaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitstructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM_TimebaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimebaseInitStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimebaseInitStructure.TIM_Period = LED_RGB_PWM_PERIOD;// 732Hz
	TIM_TimebaseInitStructure.TIM_Prescaler = 0;
	TIM_TimebaseInitStructure.TIM_RepetitionCounter = 0;  	
	TIM_TimeBaseInit(TIM3,&TIM_TimebaseInitStructure);

	TIM_OCInitstructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitstructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitstructure.TIM_Pulse = rgbLightValue.RGB_RCompare;
	TIM_OCInitstructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitstructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitstructure.TIM_OutputNState = TIM_OutputState_Disable;
	
	TIM_OC1Init(TIM3, &TIM_OCInitstructure);
	TIM_OC2Init(TIM3, &TIM_OCInitstructure);
	TIM_OC3Init(TIM3, &TIM_OCInitstructure);
	TIM_OC4Init(TIM3, &TIM_OCInitstructure);
	TIM_Cmd(TIM3,ENABLE);
	EnableRGBLEDLight(DISABLE);
}
void TimerNvicInit(void)
{
 NVIC_InitTypeDef  NVIC_InitStructure;

 NVIC_InitStructure.NVIC_IRQChannel = TIM15_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
 NVIC_Init(&NVIC_InitStructure);

 NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
 NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
 NVIC_Init(&NVIC_InitStructure);
}

void SetLed1On(void)
{
GPIO_ResetBits(GPIOC,GPIO_Pin_13);
}


void SetLed1Off(void)
{
GPIO_SetBits(GPIOC,GPIO_Pin_13);
}





uint16_t fanSpdPeriod = 0;
uint8_t  captureStatus = 0;
uint16_t captureLow;
uint16_t captureHigh;
void TIM1_CC_IRQHandler(void)
{
	uint16_t fanCurCnt;
	if(TIM_GetITStatus(TIM1,TIM_IT_CC1))// output toggle
	{
	   TIM_ClearITPendingBit(TIM1,TIM_IT_CC1);
	   fanCurCnt = TIM_GetCapture1(TIM1);
	   fanCurCnt +=fanPeriod;
	   if(fanCurCnt > (FAN_PERIOD_MAXCOUNT-1))//
	   {
        fanCurCnt -=(FAN_PERIOD_MAXCOUNT-1);
	   }
	   TIM_SetCompare1(TIM1,fanCurCnt);
	}

	if(TIM_GetITStatus(TIM1,TIM_IT_CC4))// input capture
	{
		TIM_ClearITPendingBit(TIM1,TIM_IT_CC4);
		if(captureStatus == 0)
		{
			captureLow = TIM_GetCapture4(TIM1);    
            captureStatus = 1;
		}else
		{ 
			captureHigh = TIM_GetCapture4(TIM1);
			if(captureHigh>captureLow)
				fanSpdPeriod = captureHigh - captureLow;
			else
				fanSpdPeriod = (captureHigh+FAN_PERIOD_MAXCOUNT) - captureLow;
            captureStatus = 0;
		}
	}
}


uint16_t GetFanPeriod(void)
{
	uint16_t ret;
	ret = fanSpdPeriod;
	fanSpdPeriod = 0;
 return ret;
}


void TIM15_IRQHandler(void)
{
	uint16_t buzzerCurCnt;
if(TIM_GetITStatus(TIM15,TIM_IT_CC1) == SET)
{
	TIM_ClearITPendingBit(TIM15,TIM_IT_CC1);
	buzzerCurCnt = TIM_GetCapture1(TIM15);
    buzzerCurCnt += buzzPeriod;
	TIM_SetCompare1(TIM15,buzzerCurCnt);
}

}

void ChangeFanPeriod(uint16_t freq)
{
	fanPeriod = freq;
	if((TIM1->CCER&0x0001) == 0)// the 12th bit is channel 4 output enable control bit 
	{
		FanTimerStart();
		TIM_CCxCmd(TIM1,TIM_Channel_1,ENABLE);
	}
	else if(freq == FAN_PERIOD_MAXCOUNT)
	{
		FanTimerCounterStop();
		TIM_CCxCmd(TIM1,TIM_Channel_1,DISABLE);
	}
}

void FanTimerCounterStop(void)
{
	TIM_SetCompare1(TIM1,fanPeriod);
	TIM_SetCounter(TIM1,0x0000);
	TIM_Cmd(TIM1,DISABLE);
}

void FanTimerStart(void)
{
  TIM_SetCompare1(TIM1,fanPeriod);
	TIM_SetCounter(TIM1,0x0000);
	TIM_Cmd(TIM1,ENABLE);
}


void ChangeLightPwm(uint16_t pwm)
{
 TIM_SetCompare4(TIM3,pwm);
}



void MotorPowerCtrl(uint8_t on)
{
	if(on == ENABLE)
	{
		if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_12) == 0)
		GPIO_SetBits(GPIOA,GPIO_Pin_12);
	}
	else
	{
		if(GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_12) == 1)
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	}
}

void BuzzerOnOff(uint8_t on)
{
	if(on == ENABLE)
		GPIO_SetBits(GPIOA,GPIO_Pin_9);
	else
		GPIO_ResetBits(GPIOA,GPIO_Pin_9);
}


uint8_t GetBuzOnOffStatus(void)
{
  return GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_9);
}

uint8_t GetBuzMotorOnOffStatus(void)
{
  return GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_12);
}


void EnableRGBLEDLight(uint8_t en)
{
	if(en == ENABLE)
	{
		TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Enable);
		TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Enable);
		TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Enable);
		TIM_CCxCmd(TIM3,TIM_Channel_4,TIM_CCx_Enable);	
	}
	else
	{
		TIM_CCxCmd(TIM3,TIM_Channel_1,TIM_CCx_Disable);
		TIM_CCxCmd(TIM3,TIM_Channel_2,TIM_CCx_Disable);
		TIM_CCxCmd(TIM3,TIM_Channel_3,TIM_CCx_Disable);
		TIM_CCxCmd(TIM3,TIM_Channel_4,TIM_CCx_Disable);
	}
}

uint16_t GetRGBCurrentCompare_R(void)
{
	return TIM_GetCapture1(TIM3);
}

uint16_t GetRGBCurrentCompare_G(void)
{
	return TIM_GetCapture2(TIM3);
}

uint16_t GetRGBCurrentCompare_B(void)
{
	return TIM_GetCapture3(TIM3);
}

uint16_t GetLightCurrentCompare(void)
{
	return TIM_GetCapture4(TIM3);
}
