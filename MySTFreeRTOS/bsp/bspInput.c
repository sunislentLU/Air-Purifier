#include "bspInput.h"
#include "stm32f0xx.h"
#include "hdc1080.h"
#include "pms7003i.h"


void InputHardwareInit(void)
{
KeySwitchGPIO_Config();
GasGpioInit();
DeviceAdcInit();
LightLumiGpioInit();
SensorI2CGpio_Init();
KeyTimerInit();
}


void KeySwitchGPIO_Config(void)
{
GPIO_InitTypeDef GPIO_InitStructure;
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_Init(GPIOB, &GPIO_InitStructure);
}


void DustGpioInit(void)
{
GPIO_InitTypeDef  GPIO_InitStructure;
RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
GPIO_Init(GPIOA,&GPIO_InitStructure);
GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_1);
GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_1);
}

void GasGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

}


void LightLumiGpioInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
}

void DeviceAdcInit(void)
{
	ADC_InitTypeDef ADC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_ADCCLK_PCLK_Div4);
	ADC_StructInit(&ADC_InitStructure);
	ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_Init(ADC1,&ADC_InitStructure);	
}



uint16_t GetAdcValue(uint32_t channel)
{
	uint16_t adcValue;
	ADC1->CHSELR = 0;
	ADC_ChannelConfig(ADC1,channel,ADC_SampleTime_239_5Cycles);
    ADC_GetCalibrationFactor(ADC1);
	ADC_Cmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_ADRDY));
	ADC_StartOfConversion(ADC1);
    while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC) == RESET);
	adcValue = ADC_GetConversionValue(ADC1)&0x0fff;
	ADC_Cmd(ADC1,DISABLE);
    return adcValue;
}


void KeyTimerInit(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimebaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_TimebaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimebaseInitStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimebaseInitStructure.TIM_Period = 48000;// 10ms
	TIM_TimebaseInitStructure.TIM_Prescaler = 9;
	TIM_TimebaseInitStructure.TIM_RepetitionCounter = 0;  	
	TIM_TimeBaseInit(TIM6,&TIM_TimebaseInitStructure);
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM6,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


extern void KeyProcess(void);
void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update))
	{
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		KeyProcess();
	}
}

uint16_t GetDustSensorRawData(void)
{
	uint16_t pm25,pm100;
	GetParticalDensity(&pm25,&pm100);
 return pm25;
}

uint16_t GetGasAdcValue(void)
{
  uint16_t ret;
  ret = GetAdcValue(ADC_Channel_1);
 return ret;
}



uint16_t GetLumiAdcValue(void)
{
  uint16_t ret;
  ret = GetAdcValue(ADC_Channel_0);
 return ret;
}


//void USART1_IRQHandler(void)
//{
//	if(USART_GetITStatus(USART1,USART_IT_RXNE))
//	{
//    DustSensorGetResult();
//	}
//}

void SensorI2CGpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	I2C_InitTypeDef I2C_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	I2C_InitStructure.I2C_DigitalFilter = 0x00;
	I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
	I2C_InitStructure.I2C_Timing = 0x1045061D;
	I2C_Init(I2C2,&I2C_InitStructure);
	I2C_Cmd(I2C2,ENABLE);
}

uint8_t GetTempHumValue(uint16_t* temp,uint16_t* hum)
{
	return GetTempHumi(temp,hum);
}

uint16_t GetTemperatureValue(void)
{
	uint16_t temp,humi;
	GetTempHumi(&temp,&humi);
	return temp;
}


uint16_t GetHumidityValue(void)
{
	uint16_t temp,humi;
	GetTempHumi(&temp,&humi);
	return humi;
}


