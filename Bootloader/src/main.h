
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
#define BOOT_END_ADDR    0x08001FFF
#define BOOT_AREA_SIZE   0x2000

#define APP_START_ADDR   0x08002000
#define APP_END_ADDR     0x0800F7FF
#define APP_AREA_SIZE    0xD800


#define DATA_START_ADDR   0x0800F800
#define DATA_END_ADDR     0x0800FFFF
#define DATA_AREA_SIZE    0x800

#define FLASH_VOLUME      0x10000
#define FLASH_START_ADDR  0x08000000
#define FLASH_END_ADDR    0x0800FFFF



typedef enum
{
	BOOT_GET_STATUS = 0x00,
	BOOT_WAIT_REQ,
	BOOT_WAIT_DATA,
  BOOT_END
}_eBOOT_PRC;


typedef  void (*pFunction)(void);
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __MAIN_H */

