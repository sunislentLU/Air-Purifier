//#include "stm32f0xx.h"
#include "hardware.h"


void WifiGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);
}
void WifiUartInit(void)
{
    
	USART_InitTypeDef USART_Initstructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);
	USART_StructInit(&USART_Initstructure);
	USART_Initstructure.USART_BaudRate = 115200;
	USART_Initstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Initstructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Initstructure.USART_Parity = USART_Parity_No;
	USART_Initstructure.USART_StopBits = USART_StopBits_1;
	USART_Init(USART2,&USART_Initstructure);
//	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	//USART_ITConfig(USART2,USART_IT_TC, ENABLE);// enable transmit complete interrupt
	USART2->ICR = 0xffffffff;
	USART_Cmd(USART2,ENABLE);
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
//	NVIC_Init(&NVIC_InitStructure);
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

void InputHardwareInit(void)
{
	WifiGPIOInit();
	WifiUartInit();
	KeySwitchGPIO_Config();
	GasGpioInit();
	LightLumiGpioInit();
	SensorI2CGpio_Init();
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
}


void FanGpioInit(void)
{
 GPIO_InitTypeDef GPIO_InitStructre;
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_11|GPIO_Pin_8;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructre);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_IN;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_11;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA, &GPIO_InitStructre);
	

 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB,ENABLE);
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin  = GPIO_Pin_15;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB, &GPIO_InitStructre);

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
 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOA,&GPIO_InitStructre);

 GPIO_InitStructre.GPIO_Mode = GPIO_Mode_OUT;
 GPIO_InitStructre.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructre.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
 GPIO_InitStructre.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_InitStructre.GPIO_Speed = GPIO_Speed_50MHz;
 GPIO_Init(GPIOB,&GPIO_InitStructre);
}


void OutputHardwareGPIOInit(void)
{
	BuzzerGpioInit();
  FanGpioInit();
	LedGpioInit();
	RGBLightPwrGpio_init();
}


void HardWareInit(void)
{
	SysTick_Config(SystemCoreClock/1000);
	InputHardwareInit();
  OutputHardwareGPIOInit();
}
