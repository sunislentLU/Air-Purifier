#include "test.h"
#include "main.h"
#include "mainTask.h"



extern uint8_t isTestMode;
extern uint8_t subTestMode;
extern uint8_t entryTestMode;
extern xQueueHandle inputMsgQueue;
extern xQueueHandle outputMsgQueue;
extern _sINPUT_MSG* mInputMsg;
extern _sWIFI_REC_MSG* mWifiRecMsg;
extern _sWIFI_SND_MSG* mWifiSndMsg;
extern _sOUTPUT_MSG* mOutputMsg;
extern _sRUNNINGVALUE runningValue;
extern _sREFERENCE_VALUE globalParameter;

uint8_t CheckTestMode(void);
extern  void MainSetBuzzer(uint8_t buzType);
extern  void MainSetNetState(void);
extern  void MainSetMode(uint8_t* mode);
extern void MainSetSpeed(uint8_t mode);
extern void MainVariablesInit(void);
extern void SendOutputMsg(_eOUTPUTMSG_TYPE msg,uint8_t paraType,void* paraPointer);

void TestHandle(uint8_t testMode)
{
	if(entryTestMode == 0)
			{
				entryTestMode = 1;
				MainSetBuzzer(OUTPUT_MSG_BUZZ_CONF);
				
				while(CheckTestMode())
				{						
				vTaskDelay(100);
				}
				while(xQueueReceive(inputMsgQueue,mInputMsg,10))
					;				
				if(isTestMode == TEST_MODE_VER)
				{
					
				
				}else if(isTestMode == TEST_MODE_SPD)
				{
					subTestMode +=TEST_LED_SPDLOW;
					MainSetMode(&subTestMode);                                                                                                                                                                                                                                                                                                                                                                                                                                           
					MainSetSpeed((subTestMode-TEST_LED_SPDLOW +1));				
				}else 
				{				
					isTestMode = TEST_MODE_NONE;
					//break;
				}
			
			}				
			if(xQueueReceive(inputMsgQueue,mInputMsg,10) == pdTRUE)
				{
					switch(mInputMsg->inputMsg)						
					{           			
						case KEY_POWER_PRESS:
							isTestMode = TEST_MODE_NONE;
              MainVariablesInit();	             						
						  MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
						vTaskDelay(1000);				 
						NVIC_SystemReset();						
						break;			
						case KEY_MODE_PRESS:
							if(isTestMode == TEST_MODE_SPD)
							{
							subTestMode++;															
								if(subTestMode > TEST_LED_SPDFAST)
									subTestMode = TEST_LED_SPDLOW;
								switch(subTestMode)
								{
									case TEST_LED_SPDLOW:
										runningValue.speed.targetSpd = globalParameter.speedRef.lowSpdRef;
										break;
									case TEST_LED_SPDMED:
										runningValue.speed.targetSpd = globalParameter.speedRef.mediumSpdRef;
										break;
								  case TEST_LED_SPDHIGH:
										runningValue.speed.targetSpd = globalParameter.speedRef.highSpdRef;
										break;
									case TEST_LED_SPDALOW:
										runningValue.speed.targetSpd = globalParameter.atuoSpdRef.autoMedSpdRef;
										break;
									case TEST_LED_SPDAMED:
										runningValue.speed.targetSpd = globalParameter.atuoSpdRef.autoMedSpdRef;
										break;
								  case TEST_LED_SPDAHIGH:
										runningValue.speed.targetSpd = globalParameter.atuoSpdRef.autoHighSpdRef;
										break;
									case TEST_LED_SPDFAST:
										runningValue.speed.targetSpd = globalParameter.speedRef.jetSpdRef;
										break;
									default:
								break;
								}
																
//								mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
//								mOutputMsg->paramType = MSG_PARAM_USHORT;
//								mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
//								xQueueSend(outputMsgQueue,mOutputMsg,0);
								SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);

							MainSetMode(&subTestMode);
							
							MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);		
							}else
							{

							}
						break;
								
						case KEY_VOLUME_PRESS:
							if(isTestMode == TEST_MODE_SPD)
							{
								runningValue.speed.targetSpd += 10;
								if(runningValue.speed.targetSpd>=2400)
									runningValue.speed.targetSpd = 2400;
								//MainSetSpeed((subTestMode-TEST_LED_SPDLOW));
//								mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
//								mOutputMsg->paramType = MSG_PARAM_USHORT;
//								mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
//								xQueueSend(outputMsgQueue,mOutputMsg,0);
								SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);
								MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
							}else
							{

							}
				
						break;
						case KEY_TIMING_PRESS:
							if(isTestMode == TEST_MODE_SPD)
							{
								runningValue.speed.targetSpd -= 10;
								if(runningValue.speed.targetSpd<=200)
									runningValue.speed.targetSpd = 200;
								//MainSetSpeed((subTestMode-TEST_LED_SPDLOW));
//								mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
//								mOutputMsg->paramType = MSG_PARAM_USHORT;
//								mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
//								xQueueSend(outputMsgQueue,mOutputMsg,0);
								SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);								
								MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);

							}else
							{

							}
							break;			
						default:				
						break;				
					}			
				}			
}


uint8_t CheckTestMode(void)
{
	uint8_t ret = 0;
	if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) == 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0))
	{
		vTaskDelay(50);
		if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) == 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) == 0))
		
			ret = TEST_MODE_VER;
		
	}
	else if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) == 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0))
	{
		vTaskDelay(50);
		if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) == 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0))
		
			ret = TEST_MODE_SPD;
	}
	else ret = 0;

 return ret;

}




