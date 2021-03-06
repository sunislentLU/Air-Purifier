
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/* Exported types ------------------------------------------------------------*/
//---------------------------input message define area--------------------------------------
/**********************************
** IMPORTANT 
***********************************/

#define BOOT_START_ADDR  0x08000000
#define BOOT_END_ADDR    0x080027FF
#define BOOT_AREA_SIZE   0x2800

#define APP_START_ADDR   0x08002800
#define APP_END_ADDR     0x0800F7FF
#define APP_AREA_SIZE    0xD000


#define DATA_START_ADDR   0x0800F800
#define DATA_END_ADDR     0x0800FFFF
#define DATA_AREA_SIZE    0x800

#define BOOT_DATA_ADDR    0x0800FC00
#define BOOT_DATA_END_ADDR 0x0800FFFF
#define BOOT_DATA_SIZE    0x400

#define FLASH_VOLUME      0x10000
#define FLASH_START_ADDR  0x08000000
#define FLASH_END_ADDR    0x0800FFFF




#define BOOT_OPTION_ADDR BOOT_DATA_ADDR
#define VERSION_ADDR     BOOT_OPTION_ADDR+4
#define CHECK_RESULT     VERSION_ADDR+4
#define FIRMLEN_ADDR     CHECK_RESULT+16


#define PAGE_SIZE  1024
#define APP_PAGE_NUM   53

typedef enum
{
	BOOT_WAIT2APP = 0x00,
	BOOT_WAIT_REQ,
	BOOT_WAIT_DATA,
	BOOT_CHK,
  BOOT_END
}_eBOOT_PRC;

//typedef unsigned char *POINTER; 
//typedef unsigned short int UINT2; 
//typedef unsigned long int UINT4; 

//typedef struct 
//{ 
// UINT4 state[4]; 
// UINT4 count[2]; 
// unsigned char buffer[64]; 
//} MD5_CTX; 



//typedef struct
//{
//unsigned char  head;
//unsigned short length;
//unsigned char cmd;
//unsigned char data[262];
//unsigned char checksum;//cmd + length + sn +ip +data
//unsigned char end;
//}_sWIFI_FORMAT;

typedef  void (*pFunction)(void);
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

