
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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"
#include "portable.h"
#include "timers.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "stm32f0xx.h"
#include "main.h"
/*Fucntions Declare*/
extern  void MainTask(void* arg);
extern void DeviceOutputTask(void* arg);
extern void DeviceInputTask(void* arg);
extern void WIFITask(void* arg);
static void RuningEvironmentSetting(void);
static void InterruptVectorRemap(void);
extern void InputVariableInit(void);
extern void OutputVariablesInit(void);
extern void WifiVariablesInit(void);
static void TaskVariablesInit(void);
/*Variables*/
xTaskHandle  mainTask;
xTaskHandle deviceOutputTask;
xTaskHandle deviceInputTask;
xTaskHandle wifiTask;

int main(void)
{
RuningEvironmentSetting();
TaskVariablesInit();	
xTaskCreate(MainTask,"mainTask",configMINIMAL_STACK_SIZE-32,NULL,configMAX_PRIORITIES - 2 ,&mainTask);
xTaskCreate(DeviceOutputTask,"deviceoutputtask",configMINIMAL_STACK_SIZE-32,NULL,configMAX_PRIORITIES - 3 ,&deviceOutputTask);
xTaskCreate(DeviceInputTask,"deviceinputtask",configMINIMAL_STACK_SIZE-32 ,NULL,configMAX_PRIORITIES - 3 ,&deviceInputTask);
xTaskCreate(WIFITask,"wifitask",configMINIMAL_STACK_SIZE-16,NULL,configMAX_PRIORITIES - 3 ,&wifiTask);
vTaskStartScheduler();
}

/**
*Function     Name:RuningEvironmentSetting  
*Description:  setting running enviroment such as use iap 
   application code must set interrupt vector remap from sram
*Parameters:   none 
*return:       none 
*Date:         20170426
*Author:       luxq
**/
	#ifdef UART_DEBUG
	extern void DebugUartInit(void);
	#endif
static void RuningEvironmentSetting(void)
{
  InterruptVectorRemap();
}

/*Function      Name:InterruptVectorRemap  */
/*Description:  copy the vector from flash code to sram and change remap from sram(IAP must) */
/*Parameters:   none */
/*return:       none */
/*Date:         20170426*/
/*Author:       luxq*/
static void InterruptVectorRemap(void)
{
	uint8_t i = 0;      
  for(i = 0; i < 48; i++)
  {
    *((uint32_t*)(0x20000000 + (i << 2)))=*(__IO uint32_t*)(APP_START_ADDR + (i<<2));
	}
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); 
  SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);
}



static void TaskVariablesInit(void)
{
   InputVariableInit();
   OutputVariablesInit();
   WifiVariablesInit();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask,char* pcTaskName)
{
while(1);
}










