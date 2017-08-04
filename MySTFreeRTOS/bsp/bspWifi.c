#include "bspWifi.h"
#include "stm32f0xx.h"
#include "fifo.h"
void WifiGPIOInit(void);
void WifiUartInit(void);
void WifiHardwareInit(void)
{
   WifiGPIOInit();
   WifiUartInit();
}


void WifiGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	WIFI_GPIO_CLK_CMD(WIFI_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = WIFI_UART_TX;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WIFI_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = WIFI_UART_RX;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WIFI_GPIO_PORT, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = WIFI_RST_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(WIFI_GPIO_PORT, &GPIO_InitStructure);
 
	
}
void WifiUartInit(void)
{
    
	USART_InitTypeDef USART_Initstructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	USART_DeInit(USART2);
	USART_StructInit(&USART_Initstructure);
	USART_Initstructure.USART_BaudRate = WIFI_UART_BAUDRATE;
	USART_Initstructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Initstructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_Initstructure.USART_Parity = USART_Parity_No;
	USART_Initstructure.USART_StopBits = USART_StopBits_1;
	USART_Initstructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(WIFI_UART,&USART_Initstructure);
	USART_ITConfig(WIFI_UART,USART_IT_RXNE,ENABLE);
	USART_ITConfig(WIFI_UART,USART_IT_TC, ENABLE);// enable transmit complete interrupt
	USART2->ICR = 0xffffffff;
	USART_Cmd(WIFI_UART,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
	NVIC_Init(&NVIC_InitStructure);
}



