#include "test.h"
#include "main.h"
#include "mainTask.h"
#include "bspOutput.h"


extern uint8_t isTestMode;
extern uint8_t subTestMode;
extern uint8_t entryTestMode;
extern xQueueHandle inputMsgQueue;
extern xQueueHandle outputMsgQueue;
extern _sINPUT_MSG* mInputMsg;
extern _sOUTPUT_MSG* outputMsg;
extern _sWIFI_REC_MSG* mWifiRecMsg;
extern _sWIFI_SND_MSG* mWifiSndMsg;
extern _sOUTPUT_MSG* mOutputMsg;
extern _sRUNNINGVALUE runningValue;
extern _sREFERENCE_VALUE globalParameter;
extern _sRGBLIGHT rgbLightValue;

extern  void MainSetBuzzer(uint8_t buzType);
extern  void MainSetNetState(void);
extern  void MainSetMode(uint8_t* mode);
extern void MainSetSpeed(uint8_t mode);
extern void MainVariablesInit(void);
extern void SendOutputMsg(_eOUTPUTMSG_TYPE msg,uint8_t paraType,void* paraPointer);
extern   void SetFanSpeed(uint16_t spd);
extern  void SetBuzzer(uint8_t op);
extern uint16_t GetFirmVersion(void);
extern uint16_t* GetDustValue(void);
extern uint8_t* GetHumi(void);
extern uint8_t* GetTemp(void);
extern uint16_t GetWorkingTime(void);
extern uint16_t GetFilterTime(void);
extern uint16_t GetRunningTime(void);
extern void ReadGlobalParameter(void);
extern void SaveGlobalParameter(void);
extern uint8_t* GetTVOCValue(void);
extern uint8_t* GetLuminValue(void);

uint8_t CheckTestMode(void);
void TestSetSpeed(uint8_t testType);
void SetTestSpdType(uint8_t tstspdtype);
void TestModeLedIndication(void);
void LedDataDisplay(uint8_t dat);
void DisLedBit7(uint8_t on);
void DisLedBit6(uint8_t on);
void DisLedBit5(uint8_t on);
void DisLedBit4(uint8_t on);
void DisLedBit3(uint8_t on);
void DisLedBit2(uint8_t on);
void DisLedBit1(uint8_t on);
void DisLedBit0(uint8_t on);

uint8_t thirdTestMode;
const func ledFunction[8]={DisLedBit0,DisLedBit1,DisLedBit2,DisLedBit3,\
	                         DisLedBit4,DisLedBit5,DisLedBit6,DisLedBit7};
uint8_t lowHighFlag = 0;

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
				if(isTestMode == TEST_MODE_NOR)
				{
					subTestMode = TEST_SOFT_VERSION;
					MainSetMode(&subTestMode); 
				}else if(isTestMode == TEST_MODE_SPD)
				{
					subTestMode +=DIS_LOW_SPD;
					MainSetMode(&subTestMode);                                                                                                                                                                                                                                                                                                                                                                                                                                           
					TestSetSpeed((subTestMode));				
				}else 
				{				
					isTestMode = TEST_MODE_NONE;
				}
			
			}				
			if(xQueueReceive(inputMsgQueue,mInputMsg,10) == pdTRUE)
				{
					switch(mInputMsg->inputMsg)						
					{           			
						case KEY_POWER_PRESS:
						  isTestMode = TEST_MODE_NONE;
						  MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
						  vTaskDelay(1000);				 
						  NVIC_SystemReset();						
						break;	

						
						case KEY_MODE_PRESS:
							if(isTestMode == TEST_MODE_SPD)
							{
							  subTestMode++;															
								if(subTestMode > DIS_FAST_SPD)
									subTestMode = DIS_LOW_SPD;
							  TestSetSpeed(subTestMode);
							  MainSetMode(&subTestMode);
                              MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);									
							  	
							}else
							{
								subTestMode++;
								lowHighFlag = 0;
								if(subTestMode == TEST_TAKE_FACORY)
								{
									ReadGlobalParameter();
					                SaveGlobalParameter();
									rgbLightValue.RGB_RCompare = 0x00;
									rgbLightValue.RGB_GCompare = 0xff;
									rgbLightValue.RGB_BCompare = 0xff;
								}
								if(subTestMode > TEST_TAKE_FACORY)
									subTestMode = TEST_SOFT_VERSION;
								thirdTestMode = 0;
								MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);	
							}
						break;
								
						case KEY_VOLUME_PRESS:
							if(isTestMode == TEST_MODE_SPD)
							{
								runningValue.speed.targetSpd += 20;
								if(runningValue.speed.targetSpd>=4000)
									runningValue.speed.targetSpd = 4000;
								SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);
								MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
							}else
							{
								thirdTestMode++;
								POWER_LED_OFF();
								lowHighFlag = 0;
								switch(subTestMode)
								{
								case TEST_SOFT_VERSION:
									if(thirdTestMode>DIS_WORKTIME)
										thirdTestMode = DIS_VER;
								break;
								case TEST_SENSOR_OP:
									if(thirdTestMode>SEN_LUMI)
										thirdTestMode = SEN_DUST;
								break;
							    case TEST_LED_KEY:
									if(thirdTestMode >LED_DIS_STA6)
										thirdTestMode = LED_DIS_ON;
								break;
								case TEST_TAKE_FACORY:



								break;					
								default:
								break;
								}			
								MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);								
							}
				
						break;
						case KEY_TIMING_PRESS:
							if(isTestMode == TEST_MODE_SPD)
							{
								runningValue.speed.targetSpd -= 20;
								if(runningValue.speed.targetSpd<=200)
									runningValue.speed.targetSpd = 200;
								SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);								
								MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);

							}else
							{
								
								lowHighFlag^=1;
								MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);

							}
							break;	
				     case INPUT_MSG_DUST:			       
					     runningValue.dustDensity = *((uint16_t*)mInputMsg->inputMsgParam);				        
				      break;
					 case INPUT_MSG_GAS:
					   runningValue.gasValue = *((uint16_t*)mInputMsg->inputMsgParam);
					  break;
					 case INPUT_MSG_LUMIN:
					 	runningValue.lumin= *((uint16_t*)mInputMsg->inputMsgParam);
					 	break;
					case INPUT_MSG_TEMP:
						runningValue.temperature= *((int16_t*)mInputMsg->inputMsgParam);
						break;
					case INPUT_MSG_HUMI:
						runningValue.humi= *((uint8_t*)mInputMsg->inputMsgParam);
						break;
						default:				
						break;				
					}			
				}			
}



void OutputTestHandle(void)
{
	uint16_t dataTmp;
	uint8_t mode;
	if(xQueueReceive(outputMsgQueue, outputMsg, 10))
		{
			     
			switch(outputMsg->outputMsg)
				{
				case OUTPUT_MSG_MODE:
				   mode = *((uint8_t*)outputMsg->outputMsgParam);
				   rgbLightValue.LuminCompare = 0x7f;				   	
					break;
				case OUTPUT_MSG_LIGHT:
					break;	
				case OUTPUT_MSG_SPEED:
				    dataTmp = *((uint16_t*)outputMsg->outputMsgParam);
					 SetFanSpeed(dataTmp);
					if(mode >= TEST_LED_SPDLOW)
					{
						dataTmp/=20;
						if( mode >= TEST_LED_SPDLOW)
							LedDataDisplay((uint8_t)dataTmp);
					}
					break;
			case OUTPUT_MSG_BUZZ_KEY:
		   case OUTPUT_MSG_BUZZ_CONF:
		   case OUTPUT_MSG_BUZZ_WARN:
				 SetBuzzer(outputMsg->outputMsg - OUTPUT_MSG_BUZZ_KEY +1);		
				default:
			 	    break;
				}	
			}
		TestModeLedIndication();
}


uint8_t CheckTestMode(void)
{
	uint8_t ret = 0;
	if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) == 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))
	{
		vTaskDelay(50);
		if((GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2) == 0)&&(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0))
		
			ret = TEST_MODE_NOR;
		
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


uint8_t GetTestModeFlag(void)
{
	return isTestMode;
}


void TestSetSpeed(uint8_t testType)
{
	switch(testType)
		{
			case DIS_LOW_SPD:
				runningValue.speed.targetSpd = globalParameter.speedRef.lowSpdRef;
				break;
				case DIS_MED_SPD:
					runningValue.speed.targetSpd = globalParameter.speedRef.mediumSpdRef;
				break;
				case DIS_HIGH_SPD:
				runningValue.speed.targetSpd = globalParameter.speedRef.highSpdRef;
				break;
				case DIS_ALOW_SPD:
				runningValue.speed.targetSpd = globalParameter.atuoSpdRef.autoLowSpdRef;
				break;
				case DIS_AMED_SPD:
				runningValue.speed.targetSpd = globalParameter.atuoSpdRef.autoMedSpdRef;
				break;
				case DIS_AHIGH_SPD:
				runningValue.speed.targetSpd = globalParameter.atuoSpdRef.autoHighSpdRef;
				break;
				case DIS_FAST_SPD:
				runningValue.speed.targetSpd = globalParameter.speedRef.jetSpdRef;
				break;
				default:
				break;			
	}
    SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);
}

void LedDataDisplay(uint8_t dat)
{
	uint8_t i=0;
	uint8_t opBit = dat;
	ledFunction[0]((opBit&0x01));
	for(i=1;i<8;i++)
	{
		opBit >>=1; 
		ledFunction[i]((opBit&0x01));
	}


}

void TestModeLedIndication(void)
{
	uint16_t dataTemp;
	switch(isTestMode)
	{
		case TEST_MODE_NONE:
			break;
		case TEST_MODE_NOR:
		switch(subTestMode)
			{
				case TEST_SOFT_VERSION:
					switch(thirdTestMode)
					{
					 case DIS_VER:
					 	dataTemp = GetFirmVersion();
					 	LedDataDisplay((uint8_t)dataTemp);
						rgbLightValue.RGB_RCompare = 0;
						rgbLightValue.RGB_GCompare = 0;
						rgbLightValue.RGB_BCompare = 0;
					 	break;
					 case DIS_FILTER_TIME:
					 	dataTemp = GetFilterTime();
						if(lowHighFlag)
						{
							POWER_LED_ON();
							LedDataDisplay((uint8_t)(dataTemp>>8));
							
						}
						else
						{
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						}
						rgbLightValue.RGB_RCompare = 0xff;
						rgbLightValue.RGB_GCompare = 0;
						rgbLightValue.RGB_BCompare = 0;
					 	break;
					 case DIS_WORKTIME:
					 	dataTemp = GetWorkingTime();
						if(lowHighFlag)
						{
							POWER_LED_ON();
							LedDataDisplay((uint8_t)(dataTemp>>8));
						}
						else
						{
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						}

						rgbLightValue.RGB_RCompare = 0;
						rgbLightValue.RGB_GCompare = 0xff;
						rgbLightValue.RGB_BCompare = 0;
					 	break;
					 case DIS_RUN_TIME:
					 	dataTemp = GetRunningTime();
						if(lowHighFlag)
						{
							POWER_LED_ON();
							LedDataDisplay((uint8_t)(dataTemp>>8));
						}
						else
						{
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						}
						rgbLightValue.RGB_RCompare = 0;
						rgbLightValue.RGB_GCompare = 0;
						rgbLightValue.RGB_BCompare = 0xff;
					 	break;
					 default:
					 	break;
					}
					FILTER_LED_OFF();
					WIFI_LED_OFF();
				break;
				case TEST_SENSOR_OP:
					switch(thirdTestMode)
					{
						case SEN_DUST:
							dataTemp = *(uint16_t*)GetDustValue();
						if(lowHighFlag)
						{
							POWER_LED_ON();
							LedDataDisplay((uint8_t)(dataTemp>>8));
						}
						else
						{
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						}
						rgbLightValue.RGB_RCompare = 0;
						rgbLightValue.RGB_GCompare = 0;
						rgbLightValue.RGB_BCompare = 0;
						break;
						case SEN_GAS:
							dataTemp = *(uint16_t*)GetTVOCValue();
						if(lowHighFlag)
						{
							POWER_LED_ON();
							LedDataDisplay((uint8_t)(dataTemp>>8));
						}
						else
						{
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						}	
						rgbLightValue.RGB_RCompare = 0xff;
						rgbLightValue.RGB_GCompare = 0;
						rgbLightValue.RGB_BCompare = 0;
						break;
						case SEN_TEMP:
							dataTemp = *GetTemp();
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						rgbLightValue.RGB_RCompare = 0;
						rgbLightValue.RGB_GCompare = 0xff;
						rgbLightValue.RGB_BCompare = 0;
						break;
						case SEN_HUMI:
							dataTemp = *GetHumi();
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						rgbLightValue.RGB_RCompare = 0;
						rgbLightValue.RGB_GCompare = 0;
						rgbLightValue.RGB_BCompare = 0xff;
						break;
						case SEN_LUMI:
						dataTemp = *(uint16_t*)GetLuminValue();
						if(lowHighFlag)
						{
							POWER_LED_ON();
							LedDataDisplay((uint8_t)(dataTemp>>8));
						}
						else
						{
							POWER_LED_OFF();
							LedDataDisplay((uint8_t)dataTemp);
						}	

						rgbLightValue.RGB_RCompare = 0xff;
						rgbLightValue.RGB_GCompare = 0xff;
						rgbLightValue.RGB_BCompare = 0;
						break;
					default:
					break;
					}

					FILTER_LED_ON();
					WIFI_LED_OFF();
				break;
				case TEST_KEY_LED_DIS: 
				switch(thirdTestMode)
					{
						case LED_DIS_ON:
							dataTemp = 0xff;
							LedDataDisplay((uint8_t)dataTemp);
						break;
						case LED_DIS_OFF:
							dataTemp = 0x00;
							LedDataDisplay((uint8_t)dataTemp);
						break;
						case LED_DIS_STA1:
					    rgbLightValue.RGB_BCompare = 0x00;
					    rgbLightValue.RGB_GCompare = 0x00;
					    rgbLightValue.RGB_RCompare = 0xff;
					    rgbLightValue.FilterCompare = 0xff;							
						break;
						case LED_DIS_STA2:
					    rgbLightValue.RGB_BCompare = 0x00;
					    rgbLightValue.RGB_GCompare = 0xff;
					    rgbLightValue.RGB_RCompare = 0x00;
					    rgbLightValue.FilterCompare = 0x00;							
						break;
						case LED_DIS_STA3:
					    rgbLightValue.RGB_BCompare = 0xff;
					    rgbLightValue.RGB_GCompare = 0x00;
					    rgbLightValue.RGB_RCompare = 0x00;
					    rgbLightValue.FilterCompare = 0xff;
						break;
						case LED_DIS_STA4:
						rgbLightValue.RGB_BCompare = 0x00;
					    rgbLightValue.RGB_GCompare = 0x7f;
					    rgbLightValue.RGB_RCompare = 0xff;
					    rgbLightValue.FilterCompare = 0xff;
						break;
					    case LED_DIS_STA5:
						rgbLightValue.RGB_BCompare = 0x3f;
					    rgbLightValue.RGB_GCompare = 0x7f;
					    rgbLightValue.RGB_RCompare = 0x00;
					    rgbLightValue.FilterCompare = 0x00;
						break;
						case LED_DIS_STA6:
						rgbLightValue.RGB_BCompare = 0xff;
					    rgbLightValue.RGB_GCompare = 0x1f;
					    rgbLightValue.RGB_RCompare = 0x00;
					    rgbLightValue.FilterCompare = 0xff;							
						break;
					default:
					break;
					}
					FILTER_LED_OFF();
					WIFI_LED_ON();
				break;
				case TEST_TAKE_FACORY:
					//ReadGlobalParameter();
					//SaveGlobalParameter();
					FILTER_LED_ON();
					WIFI_LED_ON();
				break;
				default:
				break;
		    }
			break;
		case TEST_MODE_SPD:
			switch(subTestMode)
				{
					case DIS_LOW_SPD:						
					POWER_LED_OFF();						
					WIFI_LED_OFF();						
					//rgbLightValue.RGB_BCompare = 0x7f;
					FILTER_LED_ON();
					break;
					case DIS_MED_SPD:			
					POWER_LED_OFF();			
					WIFI_LED_ON();
					FILTER_LED_OFF();
					//rgbLightValue.RGB_BCompare = 0x00;			
					break;		
					case DIS_HIGH_SPD:			
					POWER_LED_OFF();			
					WIFI_LED_ON();	
					FILTER_LED_ON();
					//rgbLightValue.RGB_BCompare = 0x7f;			
					break;		
					case DIS_ALOW_SPD:			
					POWER_LED_ON();			
					WIFI_LED_OFF();	
					FILTER_LED_OFF();
					//rgbLightValue.RGB_BCompare = 0x00;			
					break;	
					case DIS_AMED_SPD:			
					POWER_LED_ON();			
					WIFI_LED_OFF();	
					FILTER_LED_ON();
					//rgbLightValue.RGB_BCompare = 0x00;			
					break;	
					case DIS_AHIGH_SPD:			
					POWER_LED_ON();			
					WIFI_LED_ON();	
					FILTER_LED_OFF();
					//rgbLightValue.RGB_BCompare = 0x7f;		
					break;		
					case DIS_FAST_SPD:		
					POWER_LED_ON();		
					WIFI_LED_ON();	
					FILTER_LED_ON();
					//rgbLightValue.RGB_BCompare = 0x00;	
					break;
					default:	
					break;
	            }		
			break;
			default:
			break;
	}
}


void DisLedBit7(uint8_t on)
{
	if(on == 0)
		AUTO_LED_OFF();
	else
		 AUTO_LED_ON();
}


void DisLedBit6(uint8_t on)
{
	if(on == 0)
		FAST_LED_OFF();
	else
	    FAST_LED_ON();
}

void DisLedBit5(uint8_t on)
{
	if(on == 0)
		LOW_LED_OFF();
	else
		LOW_LED_ON();
}

void DisLedBit4(uint8_t on)
{
	if(on == 0)
		MEDIUM_LED_OFF();
	else
		MEDIUM_LED_ON();
}

void DisLedBit3(uint8_t on)
{
	if(on == 0)
		HIGH_LED_OFF();
	else
		HIGH_LED_ON();
}

void DisLedBit2(uint8_t on)
{
	if(on == 0)
		TIM1_LED_OFF();
	else
		TIM1_LED_ON();
}

void DisLedBit1(uint8_t on)
{
	if(on == 0)
		TIM2_LED_OFF();
	else
		TIM2_LED_ON();
}

void DisLedBit0(uint8_t on)
{
	if(on == 0)
		TIM3_LED_OFF();
	else
		TIM3_LED_ON();
}





