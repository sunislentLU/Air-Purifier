#ifndef _BSPWIFI_H_
#define _BSPWIFI_H_

#define WIFI_GPIO_CLK_CMD  RCC_AHBPeriphClockCmd
#define WIFI_GPIO_CLK      RCC_AHBPeriph_GPIOA
#define WIFI_GPIO_PORT     GPIOA
#define WIFI_UART_RX       GPIO_Pin_3
#define WIFI_UART_TX       GPIO_Pin_2

#define WIFI_UART          USART2
#define WIFI_UART_CLK_CMD  RCC_APB1PeriphClockCmd
#define WIFI_UART_CLK      RCC_APB1Periph_USART2
#define WIFI_UART_BAUDRATE 115200


#endif



