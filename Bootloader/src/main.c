
/**
  ******************************************************************************
  * @file    AP-C120
  * @author  CTK 
  * @version V1.0.0
  * @date    26-April-2017
  * @brief    main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT CTK</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "main.h"
/*Fucntions Declare*/
void HardWareInit(void);




_eBOOT_PRC bootState;

pFunction Jump_To_Application;
 uint32_t JumpAddress;

int main(void)
{
	HardWareInit();
	bootState = BOOT_END;
	while(1)
	{
	
	switch(bootState)
	{
		case BOOT_GET_STATUS:
			
			break;
		
		case BOOT_WAIT_REQ:
			
			break;
		
	 case BOOT_WAIT_DATA:
		 
			break;
	 
	case BOOT_END:
    if (((*(__IO uint32_t*)APP_START_ADDR) & 0x2FFE0000 ) == 0x20000000)
    { 
      JumpAddress = *(__IO uint32_t*) (APP_START_ADDR + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      __set_MSP(*(__IO uint32_t*) APP_START_ADDR);  
      Jump_To_Application();
		}
			break;
		default:
			break;
	}
	}
}


void HardWareInit(void)
{
SysTick_Config(SystemCoreClock/1000);
}


