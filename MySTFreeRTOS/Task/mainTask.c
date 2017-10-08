#include "mainTask.h"
#include "stdio.h"
#include "string.h"
#include "main.h"
/*function declare*/
void MainVariablesInit(void);
void MainSetMode(uint8_t* mode);
void MainSetSpeed(uint8_t mode);
void MainSetTiming(void);
static void FilterLiveCount(void);
static void FaultDetection(void);
static void TimingCount(void);
void MainSetBuzzer(uint8_t buzType);
void MainSetNetState(void);
static void SecondLoopProcess(void);
extern void LoopTimerInit(_sLOOPTIMER* timer,uint16_t interval);
extern uint8_t CheckTickExpired(_sLOOPTIMER* timer);
extern uint8_t SetPms7003iStandby(uint8_t onoff);
extern uint8_t CheckTestMode(void);
extern void TestHandle(uint8_t testMode);
void ReadGlobalParameter(void);
void SaveGlobalParameter(void);
void SystemUpdateMark(uint16_t ver);
void GasBaseAdj(void);
void PrintRunningValue(void);
void AqiCaculation(void);
void SendOutputMsg(_eOUTPUTMSG_TYPE msg,uint8_t paraType,void* paraPointer);
void DustSensorSleepProcess(void);
_eBOOL GetLedOnOffFlag(void);
void FilterLiveCheck(void);
extern uint16_t GetLumiAdcValue(void);
/*********************variables****************/
_sINPUT_MSG* mInputMsg;
_sOUTPUT_MSG* mOutputMsg;
_sRUNNINGVALUE runningValue;
_sREFERENCE_VALUE globalParameter;
static uint8_t aqi_tmp = 0;
static uint8_t gasJude;
uint8_t warmUp = 0;
uint16_t gasAdjCnt = 0;
uint8_t fastTime = 0;
float gasRatio = 1.0;
extern xQueueHandle inputMsgQueue;
extern xQueueHandle outputMsgQueue;
extern xTaskHandle mainTask;
extern uint8_t uartSendBuf[];
_sDATE filterCnt;
uint16_t timingCnt = 0;
uint8_t filterClrCnt = 0;
uint32_t filterLiveCnt = 0;
_sLOOPTIMER* secondLoop;
_sLOOPTIMER* mFilterCnt;

uint8_t isTestMode = 0;
uint8_t subTestMode = 0;

uint8_t testPsw = 0;
uint8_t pswCnt = 0;
static uint8_t faultCnt = 0;
static uint8_t gasJudeCnt[3] = {0};


#ifndef FOR_JP
uint8_t conTimeOut = 0;
extern xQueueHandle wifiRecQueue;
extern xQueueHandle wifiSndQueue;
uint8_t dustDelayCnt = 0;
_eBOOL updateFlag = FALSE;
_eBOOL wifiRstFlag = FALSE;
_eBOOL manulSpeedFlag = FALSE;
uint8_t wifiRstCnt = 0;
static uint8_t dustJude;
xSemaphoreHandle semWifiVariable;
_sWIFI_REC_MSG* mWifiRecMsg;
_sWIFI_SND_MSG* mWifiSndMsg;
static uint8_t dustJudeCnt[3] = {0};
#endif

const _sRUNNINGVALUE runningValueDefault = {DEFAULT_MODE,DEFAULT_DUSTVALUE,DEFAULT_DUSTSUBVALUE,DEFAULT_AQILEVEL,
	                                          DEFAULT_TIMING,DEFAULT_LED_STATE,DEFAULT_FILTER_REMAIN,DEFAULT_FAULT,
	                                          DEFAULT_COVER_STATE,DEFAULT_DUST_SENS,DEFAULT_SPEED,DEFAULT_GASVALUE,
                                            DEFAULT_LUMIN,DEFAULT_NETSTATE,DEFAULT_TEMP,DEFAULT_HUMI,DEFAULT_FILTER_DIS};

const _sSPEED_REFERENCE spdRefDefault = {DEFAULT_STANDBY_SPD_REF,DEFAULT_JET_SPD_REF,DEFAULT_LOW_SPD_REF,\
	                                     DEFAULT_MEDIUM_SPD_REF,DEFAULT_HIGH_SPD_REF};

const _sAUTOSPD_REF autoSpdRefDefault={DEFAULT_AUTO_LOW,DEFAULT_AUTO_MED,DEFAULT_AUTO_HIGH};																			 

const _sDUST_REFERENCE dustRefDefault={DEFAULT_DUST_GOOD_REF,DEFAULT_DUST_FINE_REF,DEFAULT_DUST_BAD_REF};
const _sGAS_REFERENCE  gasRefDefault = {DEFAULT_GAS_GOOD_REF,DEFAULT_GAS_FINE_REF,DEFAULT_GAS_BAD_REF};
const uint16_t timingTable[4]={TIMING_LEVEL_0,TIMING_LEVEL_1,TIMING_LEVEL_2,TIMING_LEVEL_3};
const _sLUMIN_REF luminDefaultRef={DEFAULT_LUMIN_DARK,DEFAULT_LUMIN_LIGHT};
const _sFILTERLIVE filterRef={DEFAULT_RESET_HOUR,DEFAULT_MAX_USE_HOUR};
const _sLIVERATE filterLiveRate={DEFAULT_STBRATE,DEFAULT_FASTRATE,DEFAULT_LOWRATE,DEFAULT_MEDRATE,DEFAULT_HIGHRATE};

/******************************************/

/****************
* Function Name:      MainTask
* Description:        all user interface logic and operation in this task
                      
* Parameter:          arg
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
extern uint8_t CheckTestMode(void);

void MainTask(void* arg)
{
  uint8_t* dataPointer;
  uint16_t dataTemp; 
  uint32_t dataCalu;
  uint16_t* p_data;
  MainVariablesInit();
  ReadGlobalParameter();
	#ifdef CADR_TEST
	runningValue.mode = MODE_JET;
	MainSetMode((uint8_t*)&runningValue.mode);
	MainSetSpeed(runningValue.mode);
	MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
	#endif
	vTaskDelay(1000);
	xQueueReset(inputMsgQueue);
  for(;;)
	{
		if(isTestMode != 0)//test Mode
		{	
			TestHandle(isTestMode);     
		}else// normal  mode 
		{
	
			if(xQueueReceive(inputMsgQueue,mInputMsg,10) == pdTRUE)// input task messages	
			{		
				switch(mInputMsg->inputMsg)		
				{
					//------------------wifi pair and set default---------------------------------------

	          case KEY_POWER_HOLD:
#ifndef FOR_JP
				if(runningValue.mode != MODE_STANDBY)// 待机模式下
				{				
					mWifiSndMsg->propMsg = WIFI_SET_CONN;//进入配网模式			
					xQueueSend(wifiSndQueue, mWifiSndMsg,1);			  
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);			  
					runningValue.netStatus = 4;			  
					MainSetNetState();			  
					conTimeOut = 60;			  	
				}else				
				{
					if(wifiRstFlag == FALSE)
					{
						wifiRstFlag = TRUE;
						wifiRstCnt = 60;
						SendOutputMsg(OUTPUT_MSG_LEDSON,MSG_PARAM_NONE,NULL);
						
					}else
					{
						wifiRstFlag = FALSE;
						wifiRstCnt = 0;
						SendOutputMsg(OUTPUT_MSG_LEDSOFF,MSG_PARAM_NONE,NULL);
						mWifiSndMsg->propMsg = WIFI_SET_DEFAULT;
						xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
						MainSetBuzzer(OUTPUT_MSG_BUZZ_CONF);
					}			
				}
				break;
#endif
//--------------------------------------
					case KEY_POWER_PRESS:			
					case KEY_POWER_LPRESS:				
					if(runningValue.mode == MODE_STANDBY)				
					{					
						if(testPsw == 4)					
						{						
							testPsw = 0;						
							isTestMode = 1;						
							MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);						
							break;					
						}else if(testPsw == 40)					
						{						
							testPsw = 0;						
							isTestMode = 2;							
							MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);						
							break;				
						}						
						testPsw = 0;					
						isTestMode = 0;	
#ifndef FOR_JP
						if(wifiRstFlag == TRUE)//  short long press to cancle wifi set default 
						{
							wifiRstFlag = FALSE;
							wifiRstCnt = 0;
							MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);	
							SendOutputMsg(OUTPUT_MSG_LEDSOFF,MSG_PARAM_NONE,NULL);
							break;	
						}
						manulSpeedFlag = FALSE;
#endif
						runningValue.mode = MODE_AUTO;					
						runningValue.blueLed = BLUE_LED_ON;					
						runningValue.lumin = 500;					
						SendOutputMsg(OUTPUT_MSG_LIGHT,MSG_PARAM_USHORT,&runningValue.lumin);
						SendOutputMsg(OUTPUT_MSG_BLUELED,MSG_PARAM_NONE, &runningValue.blueLed);
						FilterLiveCheck();
#ifndef FOR_JP
						mWifiSndMsg->propMsg = WIFI_UP_LED;
						xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
#endif
					}				
					else				
					{					
						testPsw = 0;					
						isTestMode = 0;					
						runningValue.mode = MODE_STANDBY;															
						if(runningValue.timingLevel != TIMING_LEVEL_NONE)					
						{						
							runningValue.timingLevel = TIMING_LEVEL_NONE;					
						}	
					}	
					if(timingCnt !=0)						
						timingCnt = 0;
					faultCnt = 0;
					MainSetTiming();
					MainSetMode((uint8_t*)&runningValue.mode);								
					MainSetSpeed(runningValue.mode);				
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);				
					break;
			case KEY_MODE_PRESS:
			case KEY_MODE_LPRESS:
			case KEY_MODE_HOLD:				
				if(runningValue.mode != MODE_STANDBY)// in standby mode no action
				{				
					if(runningValue.mode == MODE_AUTO)	
					{
						runningValue.mode = MODE_JET;	
						fastTime = FAST_MODE_TIME;
					}
					else				 
					runningValue.mode = MODE_AUTO;				
					MainSetMode((uint8_t*)&runningValue.mode);											
					if(runningValue.mode == MODE_AUTO)
					{					
						p_data = (uint16_t*)&globalParameter.atuoSpdRef;					        
						runningValue.speed.targetSpd =*(p_data+aqi_tmp);							
						SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);					
						faultCnt = 0;										
					}else
					MainSetSpeed(runningValue.mode);									
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);							
				}
				else//password process
				{
					pswCnt = 5;
					if(testPsw == 2)
						testPsw = 3;
					else if(testPsw == 3)
						testPsw = 4;
					else if(testPsw == 20)
						testPsw = 30;
					else
					{
						testPsw = 0;
						pswCnt = 0;
					}
				}
#ifndef FOR_JP
             manulSpeedFlag = FALSE;
#endif
				break;
			case KEY_VOLUME_PRESS:
			case KEY_VOLUME_LPRESS:
			case KEY_VOLUME_HOLD:
				if(runningValue.mode != MODE_STANDBY)// in standby mode no action
				{
				if((runningValue.mode >=MODE_LOW)&&(runningValue.mode <= MODE_HIGH))
				{
					runningValue.mode++;
					if(runningValue.mode >MODE_HIGH)
						runningValue.mode = MODE_LOW;
					MainSetMode((uint8_t*)&runningValue.mode);				    
					MainSetSpeed(runningValue.mode);
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
				}else// error
				{
					if(runningValue.mode <MODE_LOW)
					{
						runningValue.mode = MODE_LOW;
						MainSetMode((uint8_t*)&runningValue.mode);				    
						MainSetSpeed(runningValue.mode);
						MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
					}
				}
				}
				else//password process
				{
					pswCnt = 5;
					if(testPsw == 0)
						testPsw = 10;
					else if(testPsw == 10)
						testPsw = 20;
					else
					{
						testPsw = 0;
						pswCnt = 0;
					}
				}
#ifndef FOR_JP
               manulSpeedFlag = FALSE;
#endif
				break;
			case KEY_TIMING_PRESS:
			case KEY_TIMING_LPRESS:			
				if(runningValue.mode != MODE_STANDBY)// in standby mode no action
				{	
					if(runningValue.filterDis == FILTER_STATE_CLEAR)
				    {
						runningValue.filterDis = FILTER_STATE_USEABLE;
						runningValue.filterState = DEFAULT_FILTER_REMAIN;
						globalParameter.filterVar.filterHoursCnt = 0;
						SaveGlobalParameter();
						SendOutputMsg(OUTPUT_MSG_FDIS_NRL,MSG_PARAM_NONE,NULL);
#ifndef FOR_JP
						mWifiSndMsg->propMsg = WIFI_UP_FILTER;
						xQueueSend(wifiSndQueue,mWifiSndMsg,1);
#endif
						MainSetBuzzer(OUTPUT_MSG_BUZZ_CONF);
						break;
				    }
				else{

					switch(runningValue.timingLevel)
					{
						case TIMING_LEVEL_NONE:
						runningValue.timingLevel = TIMING_1_HOUR;
						timingCnt = timingTable[1];
						break;
						case TIMING_1_HOUR:
						runningValue.timingLevel = TIMING_4_HOUR;
						timingCnt = timingTable[2];							
						break;
						case TIMING_4_HOUR:
						runningValue.timingLevel = TIMING_8_HOUR;
						timingCnt = timingTable[3];							
						break;
					    case TIMING_8_HOUR:
						runningValue.timingLevel = TIMING_LEVEL_NONE;
						timingCnt = timingTable[0];							
						break;
					default:
						runningValue.timingLevel = TIMING_LEVEL_NONE;
						timingCnt = 0;
						break;
						}
					MainSetTiming();
					
					#if RGB_TEST
					runningValue.aqiLevel++;
					if(runningValue.aqiLevel>AQI_LEVEL_BAD)
						runningValue.aqiLevel = AQI_LEVEL_GOOD;
					SendOutputMsg(OUTPUT_MSG_RGB,MSG_PARAM_UCHAR,&runningValue.aqiLevel);
					#endif
				}
					 MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
					
			}
			else//password process
				{
					pswCnt = 5;
					if(testPsw == 0)
						testPsw = 1;
					else if(testPsw == 1)
						testPsw = 2;
					else if(testPsw == 30)
						testPsw = 40;
					else 
					{
						testPsw = 0;
						pswCnt = 0;
					}
				}
				break;

			case KEY_TIMING_HOLD:
				if(runningValue.mode!=MODE_STANDBY)
				{
					if((runningValue.filterDis == FILTER_STATE_USEABLE)||(runningValue.filterDis == FILTER_STATE_CHANGED))
					{
						runningValue.filterDis = FILTER_STATE_CLEAR;
						filterClrCnt = 5;                    
						SendOutputMsg(OUTPUT_MSG_FDIS_CLR, MSG_PARAM_NONE,NULL);
						MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
					}
				}
				
				break;
#ifndef FOR_JP
			case INPUT_MSG_DUST:
				if((mInputMsg->paramType == MSG_PARAM_SHORT)&&(dustDelayCnt>=10))
				{
					dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);
					if(((dataTemp>runningValue.dustDensity)&&(dataTemp - runningValue.dustDensity>=DUST_UP_THRD))||
						((dataTemp<runningValue.dustDensity)&&(runningValue.dustDensity-dataTemp>=DUST_UP_THRD)))
					{
						runningValue.dustDensity= dataTemp;// *((uint16_t*)mInputMsg->inputMsgParam);
						mWifiSndMsg->propMsg = WIFI_UP_DUST;
						xQueueSend(wifiSndQueue,mWifiSndMsg,0);
					}
					runningValue.dustDensity = dataTemp;
				}
				if((runningValue.fault&DUST_FAULT_BIT)== DUST_FAULT_BIT)
				{
					runningValue.fault&=~DUST_FAULT_BIT;
					mWifiSndMsg->propMsg = WIFI_UP_FAULT;
					xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
				}
				break;
			case INPUT_MSG_DUST_SUB:// 1s period 
				dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);
				if((mInputMsg->paramType == MSG_PARAM_SHORT)&&(dustDelayCnt>=10))
				{
					if(((dataTemp>runningValue.dustDensitySub)&&((dataTemp - runningValue.dustDensitySub)>=DUST_UP_THRD))||
						((dataTemp<runningValue.dustDensitySub)&&((runningValue.dustDensitySub-dataTemp)>=DUST_UP_THRD)))
					{
												
						if((dataTemp>runningValue.dustDensitySub)&&(dataTemp-runningValue.dustDensitySub>=20))//?????êá????μê±?íé??D??AQI ?????êá?o?×aê±????10???D??ò?′?
						{
							runningValue.dustDensitySub= dataTemp;// *((uint16_t*)mInputMsg->inputMsgParam);
							//AqiCaculation();
							dustJudeCnt[AQI_LEVEL_BAD] = AQI_JUDE_CNT;
							dustJudeCnt[AQI_LEVEL_FINE] = AQI_JUDE_CNT;
							dustJudeCnt[AQI_LEVEL_GOOD] = AQI_JUDE_CNT;
						}else
						runningValue.dustDensitySub= dataTemp;// *((uint16_t*)mInputMsg->inputMsgParam);
						mWifiSndMsg->propMsg = WIFI_UP_DUST_SUB;
						xQueueSend(wifiSndQueue,mWifiSndMsg,0);															
					}
					runningValue.dustDensitySub= dataTemp;
				}
				break;
		 case INPUT_MSG_TEMP:
				dataTemp = *((int8_t*)mInputMsg->inputMsgParam);
				if(((dataTemp > runningValue.temperature)&&(dataTemp - runningValue.temperature >= TEMP_UP_THD))||
					((dataTemp < runningValue.temperature)&&(runningValue.temperature - dataTemp >= TEMP_UP_THD)))
				{
					mWifiSndMsg->propMsg = WIFI_UP_TEMP;
					xQueueSend(wifiSndQueue,mWifiSndMsg,0);				
				}
				runningValue.temperature = dataTemp;
				if((runningValue.fault&HT_FAULT_BIT)== HT_FAULT_BIT)
				{
					runningValue.fault&=~HT_FAULT_BIT;
					mWifiSndMsg->propMsg = WIFI_UP_FAULT;
					xQueueSend(wifiSndQueue, mWifiSndMsg, 1);

				}
				break;
			case INPUT_MSG_HUMI:
				dataTemp = *((int8_t*)mInputMsg->inputMsgParam);	
				if(((dataTemp > runningValue.humi)&&(dataTemp - runningValue.humi >= HUMI_UP_THD))||
					((dataTemp < runningValue.humi)&&(runningValue.humi - dataTemp >= HUMI_UP_THD)))
				{
					mWifiSndMsg->propMsg = WIFI_UP_HUMI;
					xQueueSend(wifiSndQueue,mWifiSndMsg,0);				
				}
				runningValue.humi = dataTemp;
				break;
			case INPUT_MSG_DFAULT:
				if((runningValue.fault&DUST_FAULT_BIT)!= DUST_FAULT_BIT)
				{
					runningValue.fault |= DUST_FAULT_BIT;
					 mWifiSndMsg->propMsg = WIFI_UP_FAULT;
				    xQueueSend(wifiSndQueue,mWifiSndMsg,1);
				}
				break;
			case INPUT_MSG_HTFAULT:
				if((runningValue.fault&HT_FAULT_BIT)!= HT_FAULT_BIT)
				{
					runningValue.fault |= HT_FAULT_BIT;
					 mWifiSndMsg->propMsg = WIFI_UP_FAULT;
				    xQueueSend(wifiSndQueue,mWifiSndMsg,1);
				}
				break;
#endif
			case INPUT_MSG_GAS:
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
				{								
					dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);				
					dataCalu = dataTemp;
					dataCalu = dataCalu*117/100;
					if(dataCalu == 0)
						break;
					dataCalu = ((5000 - dataCalu)*9700)/dataCalu;
					if((warmUp != 0x00)&&(globalParameter.gasBase))//20170929 如果基准为0则不进行判断
					  gasRatio = 1.0*dataCalu/globalParameter.gasBase;
					else
						gasRatio = 1.0;
					
					if(((dataCalu>runningValue.gasValue)&&((dataCalu - runningValue.gasValue)>=GAS_UP_THRD))||
						((dataCalu<runningValue.gasValue)&&((runningValue.gasValue-dataCalu)>=GAS_UP_THRD)))
					{
												
						if(dataTemp<runningValue.gasValue)
						{
							runningValue.gasValue= dataTemp;
							gasJudeCnt[AQI_LEVEL_BAD] = AQI_JUDE_CNT;
							gasJudeCnt[AQI_LEVEL_FINE] = AQI_JUDE_CNT;
							gasJudeCnt[AQI_LEVEL_GOOD] = AQI_JUDE_CNT;
						}else
						runningValue.gasValue= dataCalu;// *((uint16_t*)mInputMsg->inputMsgParam);
#ifndef FOR_JP
						mWifiSndMsg->propMsg = WIFI_UP_GAS;
						xQueueSend(wifiSndQueue,mWifiSndMsg,0);		
#endif
					}
					runningValue.gasValue= dataCalu;// *((uint16_t*)mInputMsg->inputMsgParam);
					
				}
				break;
			case INPUT_MSG_LUMIN:
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
				{
					dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);				
					dataTemp = (dataTemp*3300)/4095;
					if((dataTemp>80)&&((runningValue.mode!=MODE_STANDBY)||(runningValue.blueLed == 1)))
						dataTemp-=80;
						 runningValue.lumin= dataTemp;
					if((runningValue.mode!=MODE_STANDBY)/*&&(runningValue.blueLed == BLUE_LED_ON)*/)
					{
						SendOutputMsg(OUTPUT_MSG_LIGHT,MSG_PARAM_USHORT,&runningValue.lumin);
					}
				}
				break;
			case INPUT_MSG_TOP:
				if(mInputMsg->paramType == MSG_PARAM_UCHAR)
				{
					runningValue.topCover = (_eCOVER_STATE)*((uint8_t*)mInputMsg->inputMsgParam);
				}
				break;
			case INPUT_MSG_SPEED:
				if(mInputMsg->paramType == MSG_PARAM_USHORT)
				{
					dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);
					dataTemp <<=2;
					runningValue.speed.currentSpd = dataTemp;
				}
				break;
		default:
			break;
			}         
	}
#ifndef FOR_JP
   if(xQueueReceive(wifiRecQueue, mWifiRecMsg, 10) == pdTRUE)
   {
   	    switch(mWifiRecMsg->wifiMsg)
   	    {            
					case WIFI_MSG_MODE:
						dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;
					if(*dataPointer>=MODE_TEST)
						break;
					if(*dataPointer != runningValue.mode)
					{
						if(*dataPointer == MODE_JET)
							fastTime = FAST_MODE_TIME;
						if(runningValue.mode == MODE_STANDBY)
						{
							runningValue.blueLed = BLUE_LED_ON;
							SendOutputMsg(OUTPUT_MSG_BLUELED,MSG_PARAM_UCHAR,&runningValue.blueLed);
							mWifiSndMsg->propMsg = WIFI_UP_LED;
							xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
							MainSetTiming();
							FilterLiveCheck();
						}
						runningValue.mode = (_eMODE)*dataPointer;
						if(runningValue.mode == MODE_STANDBY)
						{
							runningValue.timingLevel = TIMING_LEVEL_0;
							timingCnt = 0;
							MainSetTiming();
							
						}
						MainSetMode((uint8_t*)&runningValue.mode);
						MainSetSpeed(runningValue.mode);
						MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
						
					}
					break;			   
					case WIFI_MSG_LED:
					if(runningValue.mode!=MODE_STANDBY)
					{
					dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;	
					if((*dataPointer) == 1)
					{
			 	       runningValue.blueLed = BLUE_LED_ON;
					}
					else 
						runningValue.blueLed = BLUE_LED_OFF;					
					SendOutputMsg(OUTPUT_MSG_BLUELED,MSG_PARAM_NONE, &runningValue.blueLed);
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
					}
					mWifiSndMsg->propMsg = WIFI_UP_LED;
					xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
					break;
				 case WIFI_MSG_DUST:
			 	
				 break;
			  
				 case WIFI_MSG_GAS:
			 	
				 break;			 
			  case WIFI_MSG_LUMI:
					break;

					case WIFI_MSG_AQI:
	
					break;

					case WIFI_MSG_TIMING:
						if(runningValue.mode != MODE_STANDBY)
						{
						dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;					
							timingCnt = *dataPointer*SECOND_PER_HOUR;
							if(*dataPointer <=12)
							 runningValue.timingLevel = *dataPointer;	
							MainSetTiming();
							MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
						
						}
					break;
				case WIFI_MSG_VER:
					dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;
				  dataPointer++;
					if(*dataPointer >globalParameter.softVersion)
						updateFlag = TRUE;
					else
						updateFlag = FALSE; 

					break;
					case WIFI_MSG_FILTER:
						dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;
						if(*dataPointer<=100)
						{
						runningValue.filterState = *(uint8_t*)mWifiRecMsg->wifiMsgParam;
						globalParameter.filterVar.filterHoursCnt = globalParameter.filterVar.maxFilterHours - globalParameter.filterVar.maxFilterHours/100*runningValue.filterState;
						mWifiSndMsg->propMsg = WIFI_UP_FILTER;
						xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
						}
					break;
					case WIFI_MSG_SPEED:
						dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;
						dataTemp = 0;
						dataTemp = *dataPointer;
						dataTemp<<=8;
						dataPointer++;
						dataTemp|=*dataPointer;
						if(dataTemp<=4000)
						{
						runningValue.speed.targetSpd= dataTemp;
						SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT, &runningValue.speed.targetSpd);
						mWifiSndMsg->propMsg = WIFI_UP_SPEED;
						xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
						manulSpeedFlag = TRUE;
						}
					break;
					case WIFI_MSG_DEV_FAULT:
	
					break;
					case WIFI_MSG_SPD_REF:
	
					break;
					case WIFI_MSG_DUST_REF:
	
					break;
					case WIFI_MSG_AUTOSPD_REF:
	
					break;
					case WIFI_MSG_LUMI_REF:
	
					break;
					case WIFI_DUST_SEN:
						dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;
						dataTemp = *dataPointer++;
						dataTemp <<= 8;
						dataTemp |=*dataPointer;
						globalParameter.dustSen = dataTemp;
						SaveGlobalParameter();
						vTaskDelay(10);
						NVIC_SystemReset();
					break;
					case WIFI_TVOC_REF:
	
					break;			 
				 case WIFI_MSG_NET:
				 	if(runningValue.netStatus != *((uint8_t*)mWifiRecMsg->wifiMsgParam))
				 	{
				      runningValue.netStatus = *((uint8_t*)mWifiRecMsg->wifiMsgParam);
					  MainSetNetState();
				 	}
				 break;
				 case WIFI_MSG_UPDATE:
				 	dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;
					dataPointer++;				    
				  dataTemp = *dataPointer;
				  dataTemp&=0x00ff;
					dataPointer++;
					dataTemp |= ((*dataPointer)<<8);				    
				 SystemUpdateMark(dataTemp);
				    vTaskDelay(10);
				 NVIC_SystemReset();
				 break;
		default:
				
			break;
		}
				xSemaphoreGive(semWifiVariable);
   }	
  #endif
/********* to do gas and dust level parse**
*********  filter live count  timing count 
*********  and fan fault detection       ***/
	 SecondLoopProcess();
	 FilterLiveCount();

	}
}
}


void FactoryInit(void)
{
   memcpy((char*)&runningValue,(const char*)&runningValueDefault,sizeof(runningValueDefault));
   memcpy((char*)&globalParameter.speedRef,(const char*)&spdRefDefault,sizeof(spdRefDefault));
   memcpy((char*)&globalParameter.dustRef,(const char*)&dustRefDefault,sizeof(dustRefDefault));
   memcpy((char*)&globalParameter.gasRef,(const char*)&gasRefDefault,sizeof(gasRefDefault));
   memcpy((char*)&globalParameter.lumiRef,(const char*)&luminDefaultRef,sizeof(luminDefaultRef));
   memcpy((char*)&globalParameter.atuoSpdRef,(const char*)&autoSpdRefDefault,sizeof(autoSpdRefDefault));
}

/****************
* Function Name:      MainVariablesInit
* Description:        initionlize message queue and timers 
                      
* Parameter:          none
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
void MainVariablesInit(void)
{
   mInputMsg = pvPortMalloc(sizeof(_sINPUT_MSG));
   mOutputMsg = pvPortMalloc(sizeof(_sOUTPUT_MSG));   
   secondLoop = pvPortMalloc(sizeof(_sLOOPTIMER));
   mFilterCnt = pvPortMalloc(sizeof(_sLOOPTIMER));
 #ifndef FOR_JP
   semWifiVariable = xSemaphoreCreateBinary();
   mWifiRecMsg = pvPortMalloc(sizeof(_sWIFI_REC_MSG));
   mWifiSndMsg = pvPortMalloc(sizeof(_sWIFI_SND_MSG));
#endif
   LoopTimerInit(secondLoop,MS_PER_SECOND);//
   LoopTimerInit(mFilterCnt,MS_PER_MINUTE);// one minute per loop 
   FactoryInit();
}




/****************
* Function Name:      MainSetMode
* Description:        send current mode and send to ouput task make it action
                      
* Parameter:          mode
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
 void MainSetMode(uint8_t* mode)
{

  SendOutputMsg(OUTPUT_MSG_MODE,MSG_PARAM_UCHAR,mode);
 #ifndef FOR_JP
  mWifiSndMsg->propMsg = WIFI_UP_MODE;
  xQueueSend(wifiSndQueue,mWifiSndMsg,1);
#endif

}

/****************
* Function Name:      MainSetBuzzer
* Description:        send buzzer type to outputtask make hardware execute
                      
* Parameter:          buzzer type
* return:             none
* Date:               20170630
*author:              CTK  luxq
***************/

 void MainSetBuzzer(uint8_t buzType)
{
	SendOutputMsg((_eOUTPUTMSG_TYPE)buzType,MSG_PARAM_NONE, NULL);
}
 void MainSetNetState(void)
{
	SendOutputMsg(OUTPUT_MSG_NET,MSG_PARAM_UCHAR, (uint8_t*)(&runningValue.netStatus));
}

/****************
* Function Name:      MainSetSpeed
* Description:        send current speed and send to ouput task make it action
                      
* Parameter:          speed
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
 void MainSetSpeed(uint8_t mode)
{
	switch(mode)
	{
		case MODE_STANDBY:
			runningValue.speed.targetSpd = globalParameter.speedRef.standbySpdRef;
			break;
		case MODE_JET:
			runningValue.speed.targetSpd = globalParameter.speedRef.jetSpdRef;
			break;
		case MODE_LOW:
			runningValue.speed.targetSpd = globalParameter.speedRef.lowSpdRef;
			break;
		case MODE_AUTO:
			runningValue.speed.targetSpd = globalParameter.atuoSpdRef.autoLowSpdRef;
			break;
		case MODE_MEDIUM:
			runningValue.speed.targetSpd = globalParameter.speedRef.mediumSpdRef;
			break;
		case MODE_HIGH:
			runningValue.speed.targetSpd = globalParameter.speedRef.highSpdRef;
			break;
		default:
		break;
	}
	faultCnt = 0;
	SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);
}

void MainSetTiming(void)
{
	SendOutputMsg(OUTPUT_MSG_TIMING,MSG_PARAM_UCHAR,&runningValue.timingLevel);
#ifndef FOR_JP
	mWifiSndMsg->propMsg = WIFI_UP_TIMING;
	xQueueSend(wifiSndQueue,mWifiSndMsg,1);
#endif
}

/****************
* Function Name:      FilterLiveCount
* Description:        filterCountTmr soft timer callback function 
                      Count the time of when not in standby mode 
* Parameter:          timer 
* return:             none
* Date:               20170504
*author:              CTK  luxq
***************/
static void FilterLiveCount(void)
{
	uint8_t i = 0;
	uint16_t* p_data;
	uint8_t length;
	uint8_t* u8Pointer;
    if(CheckTickExpired(mFilterCnt))
		{
			
			filterCnt.min++;
#ifndef FOR_JP
			if(filterCnt.min%UPDATE_PERIOD == 0 )
			{
				mWifiSndMsg->propMsg = WIFI_UP_ALL;
				xQueueSend(wifiSndQueue,mWifiSndMsg,0);	
			}	
#endif
			length = sizeof(_sLIVERATE)/(sizeof(filterLiveRate.stanbyRate));
			p_data = (uint16_t*)&globalParameter.speedRef;
			for(i=0;i<length;i++)
			{
				if(*p_data == runningValue.speed.targetSpd)
					break;
				p_data++;
			}
			if(i<5)
			{
			u8Pointer = (uint8_t*)&filterLiveRate;
			filterLiveCnt+=*(u8Pointer+i);
			}
	    if(filterCnt.min == MINUTES_PER_HOUR)
	    {
			filterCnt.min = 0;
			filterCnt.hour++;
			if(filterLiveCnt >= DEFAULT_HOUR_UNIT)
			{
				globalParameter.filterVar.filterHoursCnt+= filterLiveCnt/DEFAULT_HOUR_UNIT;
				filterLiveCnt%=DEFAULT_HOUR_UNIT;	
				if(globalParameter.filterVar.filterHoursCnt>= globalParameter.filterVar.maxFilterHours)// not above 
					globalParameter.filterVar.filterHoursCnt = globalParameter.filterVar.maxFilterHours;
			}
			if(runningValue.mode>MODE_STANDBY)
			{
				globalParameter.runningTime++;
				if(globalParameter.runningTime>=0xFFFE)
					globalParameter.runningTime = 0xFFFE;
			}
			globalParameter.workingTime++;
			if(globalParameter.workingTime >= 0xFFFE)
				globalParameter.workingTime = 0xfffE;
			   runningValue.filterState = 100-((globalParameter.filterVar.filterHoursCnt*100)/globalParameter.filterVar.maxFilterHours);
			    SaveGlobalParameter();
#ifndef FOR_JP
			   	mWifiSndMsg->propMsg = WIFI_UP_FILTER;
				xQueueSend(wifiSndQueue, mWifiSndMsg, 1);
#endif
				if(globalParameter.filterVar.filterHoursCnt>= globalParameter.filterVar.maxFilterHours)
				{
					runningValue.filterDis= FILTER_STATE_CHANGED;
					SendOutputMsg(OUTPUT_MSG_FDIS_WRN,MSG_PARAM_NONE,NULL);
					
				}
	    }				
			if(fastTime >0)
			{		
				fastTime--;	
				
				if(runningValue.mode!= MODE_JET)
				{					
				fastTime = 0;		
				return;					
				}			
				if(fastTime == 0)		
				{			
					runningValue.mode = MODE_AUTO;				
					MainSetMode((uint8_t*)&runningValue.mode);				
					MainSetSpeed(runningValue.mode);				
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
				}		
	}

		}
}

/****************
* Function Name:      FaultDetection
* Description:        faultDetectTmr soft timer callback function 
                      check hardware status (.eg fan ) in period
* Parameter:          timer 
* return:             none
* Date:               20170504
*author:              CTK  luxq
***************/
static void FaultDetection(void)
{
	uint8_t faultTemp;
     //if(runningValue.mode!= MODE_STANDBY)
    // {
	 	if(runningValue.speed.currentSpd != runningValue.speed.targetSpd)
	 	{
			if(((runningValue.speed.currentSpd>runningValue.speed.targetSpd)&&((runningValue.speed.currentSpd - runningValue.speed.targetSpd)>=MOTOR_FAULT_DEATA))||
				((runningValue.speed.currentSpd<runningValue.speed.targetSpd)&&((runningValue.speed.targetSpd - runningValue.speed.currentSpd)>=MOTOR_FAULT_DEATA)))
			{
				faultCnt++;
				if(faultCnt >=10)
				{
				  faultCnt = 0;
				faultTemp |= MOTOR_FAULT_BIT;
				if(faultTemp!= runningValue.fault)
				{										
					runningValue.fault |= MOTOR_FAULT_BIT;
					if(runningValue.mode != MODE_STANDBY)
					{
					mInputMsg->inputMsg = KEY_POWER_PRESS;
					mInputMsg->paramType = MSG_PARAM_NONE;
					xQueueSend(inputMsgQueue,mInputMsg, 1);	
					MainSetBuzzer(OUTPUT_MSG_BUZZ_WARN);
					}
					vTaskDelay(500);
#ifndef FOR_JP
					mWifiSndMsg->propMsg = WIFI_UP_FAULT;
					xQueueSend(wifiSndQueue,mWifiSndMsg,1);
#endif
					vTaskDelay(500);
				}
				}
			}
			else
			{
				faultCnt = 0;
				faultTemp &= (~MOTOR_FAULT_BIT);
				if((faultTemp&MOTOR_FAULT_BIT)!= (runningValue.fault&MOTOR_FAULT_BIT))
				{
				  runningValue.fault &=(~MOTOR_FAULT_BIT);
#ifndef FOR_JP
				  mWifiSndMsg->propMsg = WIFI_UP_FAULT;
				  xQueueSend(wifiSndQueue,mWifiSndMsg,1);
#endif
				}
			}
		}else
			{
				faultCnt = 0;
				faultTemp &= (~MOTOR_FAULT_BIT);
				if((faultTemp&MOTOR_FAULT_BIT)!= (runningValue.fault&MOTOR_FAULT_BIT))
				{
				  runningValue.fault &=(~MOTOR_FAULT_BIT);
#ifndef FOR_JP
				  mWifiSndMsg->propMsg = WIFI_UP_FAULT;
				  xQueueSend(wifiSndQueue,mWifiSndMsg,1);
#endif
				}
			}
}


void GasBaseAdj(void)
{
//	static uint16_t baseTemp;
	static uint8_t gasJudeTemp = AQI_LEVEL_GOOD;
	uint16_t valueTemp;
	gasAdjCnt++;
	if(warmUp == 0)//3 minute to warm up
	{
		
		if(gasAdjCnt >= GAS_WARM_TIME)
		{
		//	baseTemp = runningValue.gasValue;
			//if(globalParameter.gasBase < baseTemp)
			globalParameter.gasBase = runningValue.gasValue;
			warmUp = 1;	  
			gasAdjCnt = 0;
		}
	}else// ten minute to adjust the gas base resistor value
	{
					
		if(gasJudeTemp!=gasJude)		
			{			
				gasJudeTemp=gasJude;
				if(gasJudeTemp == AQI_LEVEL_GOOD)			
					globalParameter.gasBase = runningValue.gasValue;				
			}
		if(gasJudeTemp == AQI_LEVEL_GOOD)
		{    
			valueTemp = runningValue.gasValue;
//			if(valueTemp>baseTemp)
//				baseTemp = valueTemp;
			if(valueTemp >globalParameter.gasBase)
				globalParameter.gasBase = valueTemp;
			if(gasAdjCnt>= GAS_BASE_ADJ_TIME)
			{
				gasAdjCnt = 0;
				globalParameter.gasBase = valueTemp;		
				valueTemp = 0;				
			}					

		}else
		gasAdjCnt = 0;		
	}
#ifndef FOR_JP
	if(dustDelayCnt <15)
	{
		dustDelayCnt++;
		if(dustDelayCnt == 10)
		{
			mWifiSndMsg->propMsg = WIFI_UP_DUST;
			xQueueSend(wifiSndQueue,mWifiSndMsg,0);
			mWifiSndMsg->propMsg = WIFI_UP_DUST_SUB;
			xQueueSend(wifiSndQueue,mWifiSndMsg,0);
		}
	}
#endif
}

void AqiCaculation(void)
{
	uint16_t* p_data; 
	uint16_t gRatio;
	gRatio = gasRatio*100;
#ifndef FOR_JP
	if(runningValue.dustDensitySub<=(globalParameter.dustRef.dustGoodRef-5))
	{
		dustJudeCnt[AQI_LEVEL_FINE] = 0;
		dustJudeCnt[AQI_LEVEL_BAD] = 0;
		dustJudeCnt[AQI_LEVEL_GOOD]++;
		if(dustJudeCnt[AQI_LEVEL_GOOD]>=AQI_JUDE_CNT)
		{
			dustJudeCnt[AQI_LEVEL_GOOD] = 0;
		   dustJude = AQI_LEVEL_GOOD;
		}
	}
  else if((runningValue.dustDensitySub>(globalParameter.dustRef.dustGoodRef+5))&&(runningValue.dustDensitySub<=(globalParameter.dustRef.dustFineRef-5)))
  	{	
		dustJudeCnt[AQI_LEVEL_GOOD] = 0;
		dustJudeCnt[AQI_LEVEL_BAD] = 0;
		dustJudeCnt[AQI_LEVEL_FINE]++;
		if(dustJudeCnt[AQI_LEVEL_FINE]>=AQI_JUDE_CNT)
		{
			dustJudeCnt[AQI_LEVEL_FINE] = 0;
		   dustJude = AQI_LEVEL_FINE;
		}
   }
	else if((runningValue.dustDensitySub>(globalParameter.dustRef.dustFineRef+5)))
    {
		dustJudeCnt[AQI_LEVEL_GOOD] = 0;
		dustJudeCnt[AQI_LEVEL_FINE] = 0;
		dustJudeCnt[AQI_LEVEL_BAD]++;
		if(dustJudeCnt[AQI_LEVEL_BAD]>=AQI_JUDE_CNT)
		{
			dustJudeCnt[AQI_LEVEL_BAD] = 0;
		   dustJude = AQI_LEVEL_BAD;
		}
     }else
     {
	if(runningValue.dustDensitySub<=(globalParameter.dustRef.dustGoodRef))
	{
		dustJudeCnt[AQI_LEVEL_FINE] = 0;
		dustJudeCnt[AQI_LEVEL_BAD] = 0;
		dustJudeCnt[AQI_LEVEL_GOOD]++;
		if(dustJudeCnt[AQI_LEVEL_GOOD]>=AQI_CRITZONT_JUDE_CNT)
		{
			dustJudeCnt[AQI_LEVEL_GOOD] = 0;
		   dustJude = AQI_LEVEL_GOOD;
		}
	}
  else if((runningValue.dustDensitySub>(globalParameter.dustRef.dustGoodRef))&&(runningValue.dustDensitySub<=(globalParameter.dustRef.dustFineRef)))
  	{	
		dustJudeCnt[AQI_LEVEL_GOOD] = 0;
		dustJudeCnt[AQI_LEVEL_BAD] = 0;
		dustJudeCnt[AQI_LEVEL_FINE]++;
		if(dustJudeCnt[AQI_LEVEL_FINE]>=AQI_CRITZONT_JUDE_CNT)
		{
			dustJudeCnt[AQI_LEVEL_FINE] = 0;
		   dustJude = AQI_LEVEL_FINE;
		}
   }
	else if((runningValue.dustDensitySub>(globalParameter.dustRef.dustFineRef)))
    {
		dustJudeCnt[AQI_LEVEL_GOOD] = 0;
		dustJudeCnt[AQI_LEVEL_FINE] = 0;
		dustJudeCnt[AQI_LEVEL_BAD]++;
		if(dustJudeCnt[AQI_LEVEL_BAD]>=AQI_CRITZONT_JUDE_CNT)
		{
			dustJudeCnt[AQI_LEVEL_BAD] = 0;
		   dustJude = AQI_LEVEL_BAD;
		}
     }
	 }
#endif
	if(gRatio >=globalParameter.gasRef.gasGoodRef)
	    {
			gasJudeCnt[AQI_LEVEL_FINE]=0;
			gasJudeCnt[AQI_LEVEL_BAD] = 0;
		gasJudeCnt[AQI_LEVEL_GOOD]++;
		if(gasJudeCnt[AQI_LEVEL_GOOD]>=AQI_JUDE_CNT)
		{
			gasJudeCnt[AQI_LEVEL_GOOD] = 0;
		    gasJude = AQI_LEVEL_GOOD;
		}
      }
  else if((gRatio>(globalParameter.gasRef.gasFineRef))&&(gRatio<=(globalParameter.gasRef.gasGoodRef)))
  	{

		gasJudeCnt[AQI_LEVEL_GOOD]=0;
		gasJudeCnt[AQI_LEVEL_BAD] = 0;
		gasJudeCnt[AQI_LEVEL_FINE]++;
		if(gasJudeCnt[AQI_LEVEL_FINE]>=AQI_JUDE_CNT)
		{
			gasJudeCnt[AQI_LEVEL_FINE] = 0;
		    gasJude = AQI_LEVEL_FINE;
		}
      }
	else if((gRatio<=(globalParameter.gasRef.gasBadRef)))
		 {
		 gasJudeCnt[AQI_LEVEL_FINE]=0;
		gasJudeCnt[AQI_LEVEL_GOOD] = 0;
		gasJudeCnt[AQI_LEVEL_BAD]++;
		if(gasJudeCnt[AQI_LEVEL_BAD]>=AQI_JUDE_CNT)
		{
			gasJudeCnt[AQI_LEVEL_BAD] = 0;
		    gasJude = AQI_LEVEL_BAD;
		}
      }
#ifndef FOR_JP
	if((gasJude==AQI_LEVEL_GOOD)&&(dustJude ==  AQI_LEVEL_GOOD))
		aqi_tmp = AQI_LEVEL_GOOD;
	else
		aqi_tmp = gasJude>dustJude?gasJude:dustJude;
#else
        aqi_tmp = gasJude;

#endif
	
	if(runningValue.mode !=MODE_STANDBY)// 运行模式
	{
		if(aqi_tmp != runningValue.aqiLevel)// aqi is changed 
		{
			runningValue.aqiLevel = (_eAQI_LEVEL)aqi_tmp;
#ifndef FOR_JP
		   mWifiSndMsg->propMsg = WIFI_UP_AQI;
		   xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
		   mWifiSndMsg->propMsg = WIFI_UP_DUST;
		   xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
		   mWifiSndMsg->propMsg = WIFI_UP_DUST_SUB;
		   xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
#endif
		   faultCnt = 0;
		}
		if(runningValue.blueLed == BLUE_LED_ON)//开灯的情况下
		{
			SendOutputMsg(OUTPUT_MSG_RGB, MSG_PARAM_USHORT, &runningValue.aqiLevel);
		}
	  if(runningValue.mode == MODE_AUTO)// if in the auto mode
	  {				
#ifndef FOR_JP
         if(manulSpeedFlag == TRUE)
		 	return;
#endif
			p_data = (uint16_t*)&globalParameter.atuoSpdRef;		   
			runningValue.speed.targetSpd =*(p_data+aqi_tmp);
	     if(runningValue.speed.currentSpd != runningValue.speed.targetSpd)
	     {
			SendOutputMsg(OUTPUT_MSG_SPEED,MSG_PARAM_USHORT,&runningValue.speed.targetSpd);
		  }					
	  }
	}	
	if(aqi_tmp != runningValue.aqiLevel)
	{
		runningValue.aqiLevel = aqi_tmp;
#ifndef FOR_JP
		mWifiSndMsg->propMsg = WIFI_UP_AQI;
		xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
		mWifiSndMsg->propMsg = WIFI_UP_DUST;
		xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
		mWifiSndMsg->propMsg = WIFI_UP_DUST_SUB;
		xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
#endif
	}
}

uint8_t t10sCounter = 0;
#ifndef FOR_JP
extern uint8_t SetPms7003iStandby(uint8_t onoff);
uint8_t dustSenSlp = 0;
void DustSensorSleepProcess(void)
{
	static uint8_t sleepCnt = 0;
	if(runningValue.mode == MODE_STANDBY)
	{
		if(sleepCnt == 20)
		{
			dustSenSlp = 1;
			SetPms7003iStandby(0);
		}
		if(sleepCnt == 0)
		{
			SetPms7003iStandby(1);
			dustSenSlp = 0;
			dustDelayCnt = 0;
		}
			sleepCnt++;	
	}else
	{
	  sleepCnt = 0;
	  if(dustSenSlp == 1)
	  {
	  	SetPms7003iStandby(1);
		dustSenSlp = 0;
		dustDelayCnt = 0;
	  }
	}
}
#endif
static void SecondLoopProcess(void)
{
	if(CheckTickExpired(secondLoop))
	{
	TimingCount();
	GasBaseAdj();
	t10sCounter++;
	FaultDetection();	
	if(t10sCounter>=10)
	{
#ifndef FOR_JP
	   mWifiSndMsg->propMsg = WIFI_UP_LUMI;
       xQueueSend(wifiSndQueue,mWifiSndMsg,0);
#endif
		t10sCounter = 0;			
	}
	if(filterClrCnt >0)
		{		
			filterClrCnt --;				
			if(filterClrCnt == 0)				
			{	
				if(runningValue.filterState>0)
				runningValue.filterDis = FILTER_STATE_USEABLE;
				else
				runningValue.filterDis = FILTER_STATE_CHANGED;
				SendOutputMsg(OUTPUT_MSG_FDIS_NRL,MSG_PARAM_NONE,NULL);				
			}
		}
	if(pswCnt)
	{
		pswCnt --;
		if(pswCnt == 0)
			testPsw = 0;		
	}
#ifndef FOR_JP
	if(wifiRstCnt)
	{
		wifiRstCnt--;
		if(wifiRstCnt == 0)
		{
			wifiRstFlag = FALSE;
			SendOutputMsg(OUTPUT_MSG_LEDSOFF,MSG_PARAM_NONE,NULL);	
		}
	}
#endif
		AqiCaculation();
	PrintRunningValue();
#ifndef FOR_JP
	DustSensorSleepProcess();
#endif
	}
}
/****************
* Function Name:      TimingCount
* Description:        timingCountTmr soft timer callback function 
                      timing  decount
* Parameter:          timer 
* return:             none
* Date:               20170504
*author:              CTK  luxq
***************/
static void TimingCount(void)
{
	if(timingCnt!=0)
	{
  timingCnt--;
  if(timingCnt == 0)
  {
//  	 if(runningValue.mode != MODE_STANDBY)
//  	 {
//		 runningValue.mode = MODE_STANDBY;
//		 MainSetMode((uint8_t*)&runningValue.mode);
//		 MainSetSpeed(runningValue.mode);
//		 MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
//		 runningValue.timingLevel = TIMING_LEVEL_NONE;
//	 }
        runningValue.timingLevel = TIMING_LEVEL_NONE;
		mInputMsg->inputMsg = KEY_POWER_PRESS;
		xQueueSend(inputMsgQueue,mInputMsg,0);
  }
	}
#ifndef FOR_JP
	if(conTimeOut>0)
	{
		conTimeOut--;
		if(conTimeOut == 0)
		{
			if(runningValue.netStatus == 4)
			{
			   mWifiSndMsg->propMsg = WIFI_REBOOT;
			   xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
			   runningValue.netStatus = 0;
			}						
		}
	}
#endif
	/*if(ledOnCnt!=0)			
	{			
		ledOnCnt--;				
		if(ledOnCnt == 0)				
		{
			runningValue.blueLed = BLUE_LED_OFF;	
//			mOutputMsg->outputMsgParam = &runningValue.blueLed;
//			mOutputMsg->outputMsg = OUTPUT_MSG_BLUELED;
//			mOutputMsg->paramType = MSG_PARAM_UCHAR;
//			xQueueSend(outputMsgQueue,mOutputMsg,1);
            SendOutputMsg(OUTPUT_MSG_BLUELED,MSG_PARAM_UCHAR, &runningValue.blueLed);

		}
	}*/
}



void SaveGlobalParameter(void)
{
  uint16_t* dataPointer;
  uint32_t adddress;
  uint8_t ret;
  uint8_t i;
  uint8_t length;
  FLASH_Unlock();
  dataPointer = (uint16_t*)&globalParameter;
  portENTER_CRITICAL();
  ret = FLASH_ErasePage(DATA_START_ADDR);
  length = sizeof(_sREFERENCE_VALUE);
  adddress = DATA_START_ADDR;
  if(ret == FLASH_COMPLETE)
  {
  	for(i=0;i<length;i=i+2)
  	{
  	ret = FLASH_ProgramHalfWord(adddress,*dataPointer);
	if(ret == FLASH_COMPLETE)
	{
	dataPointer++;
	adddress+=2;
	}else
	break;
  	}
  }
  FLASH_Lock();
  portEXIT_CRITICAL();
}


void GetFlashSpdRef(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(_sSPEED_REFERENCE);
    memcpy(&globalParameter.speedRef,(uint32_t*)address,length);
	if(globalParameter.speedRef.highSpdRef== 0xffff)
		globalParameter.speedRef.highSpdRef = spdRefDefault.highSpdRef;
	if(globalParameter.speedRef.jetSpdRef == 0xffff)
		globalParameter.speedRef.jetSpdRef= spdRefDefault.jetSpdRef;
	if(globalParameter.speedRef.lowSpdRef== 0xffff)
		globalParameter.speedRef.lowSpdRef= spdRefDefault.lowSpdRef;
	if(globalParameter.speedRef.mediumSpdRef== 0xffff)
		globalParameter.speedRef.mediumSpdRef= spdRefDefault.mediumSpdRef;
	if(globalParameter.speedRef.standbySpdRef== 0xffff)
		globalParameter.speedRef.standbySpdRef= spdRefDefault.standbySpdRef;
}

void GetFlashAutoSpdRef(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(_sAUTOSPD_REF);
    memcpy(&globalParameter.atuoSpdRef,(uint32_t*)address,length);
	if(globalParameter.atuoSpdRef.autoHighSpdRef== 0xffff)
		globalParameter.atuoSpdRef.autoHighSpdRef= autoSpdRefDefault.autoHighSpdRef;
	if(globalParameter.atuoSpdRef.autoLowSpdRef== 0xffff)
		globalParameter.atuoSpdRef.autoLowSpdRef= autoSpdRefDefault.autoLowSpdRef;
	if(globalParameter.atuoSpdRef.autoMedSpdRef== 0xffff)
		globalParameter.atuoSpdRef.autoMedSpdRef= autoSpdRefDefault.autoMedSpdRef;
}


void GetFlashDustRef(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(_sDUST_REFERENCE);
    memcpy(&globalParameter.dustRef,(uint32_t*)address,length);
	if(globalParameter.dustRef.dustBadRef== 0xffff)
		globalParameter.dustRef.dustBadRef= dustRefDefault.dustBadRef;
	if(globalParameter.dustRef.dustFineRef== 0xffff)
		globalParameter.dustRef.dustFineRef= dustRefDefault.dustFineRef;
	if(globalParameter.dustRef.dustGoodRef== 0xffff)
		globalParameter.dustRef.dustGoodRef= dustRefDefault.dustGoodRef;
}

void GetFlashLumiRef(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(_sLUMIN_REF);
    memcpy(&globalParameter.lumiRef,(uint32_t*)address,length);
	if(globalParameter.lumiRef.luminDark== 0xffff)
		globalParameter.lumiRef.luminDark = luminDefaultRef.luminDark;
	if(globalParameter.lumiRef.luminLight== 0xffff)
		globalParameter.lumiRef.luminLight= luminDefaultRef.luminLight;
}


void GetFlashFilterVar(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(_sFILTERLIVE);
    memcpy(&globalParameter.filterVar,(uint32_t*)address,length);
	if(globalParameter.filterVar.filterHoursCnt== 0xffff)
		globalParameter.filterVar.filterHoursCnt= filterRef.filterHoursCnt;	
	if(globalParameter.filterVar.maxFilterHours== 0xffff)
		globalParameter.filterVar.maxFilterHours= filterRef.maxFilterHours;	
	 runningValue.filterState = 100-((globalParameter.filterVar.filterHoursCnt*100)/globalParameter.filterVar.maxFilterHours);
	// runningValue.filterState = 0;

}

void GetFlashSen(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(uint16_t);
    memcpy(&globalParameter.dustSen,(uint32_t*)address,length);
	if(globalParameter.dustSen== 0xffff)
		globalParameter.dustSen = DEFAULT_DUST_SENS;
}

void GetFlashGasBase(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(uint16_t);
    memcpy(&globalParameter.gasBase,(uint32_t*)(address),length);
	if(globalParameter.gasBase== 0xffff)
		globalParameter.gasBase = DEFAULT_GAS_BASE;
}


void GetFlashWorkingTime(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(uint16_t);
    memcpy(&globalParameter.workingTime,(uint32_t*)(address),length);
	if(globalParameter.workingTime == 0xFFFF)
		globalParameter.workingTime= 0x01;

}

void GetFlashSoftVer(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(uint16_t);
    memcpy(&globalParameter.softVersion,(uint32_t*)(address),length);
	if(globalParameter.softVersion != DEFALUT_SOFTVER)
		globalParameter.softVersion= DEFALUT_SOFTVER;
}
void GetFlashRunningTime(uint32_t addr)
{
	uint8_t length;
	uint32_t address;
	address = addr;
	length = sizeof(uint16_t);
    memcpy(&globalParameter.runningTime,(uint32_t*)(address),length);
	if(globalParameter.runningTime == 0xffff)
		globalParameter.runningTime= DEFAULT_RUNING_TIME;



}

void ReadGlobalParameter(void)
{
	GetFlashSpdRef(SPDREF_ADDR);
	GetFlashAutoSpdRef(AUTOSPDREF_ADDR);

	GetFlashLumiRef(LUMIREF_ADDR);
	GetFlashFilterVar(FILTER_ADDR);
	GetFlashWorkingTime(WORKTIME_ADDR);
#ifndef FOR_JP
	GetFlashSen(DUSTSEN_ADDR);
	GetFlashDustRef(DUSTREF_ADDR);
#endif
	//GetFlashGasBase(GASBASE_ADDR);
	GetFlashSoftVer(VERSION_ADDR);
	GetFlashRunningTime(RUNTIME_ADDR);
}

void SystemUpdateMark(uint16_t ver)
{
  uint8_t ret;	
  FLASH_Unlock();
  portENTER_CRITICAL();
  ret = FLASH_ErasePage(BOOT_DATA_ADDR);
  if(ret == FLASH_COMPLETE)
  {
  	ret = FLASH_ProgramHalfWord(BOOT_DATA_ADDR,ver);
  	while(ret != FLASH_COMPLETE)
		;
	ret = ret = FLASH_ProgramHalfWord(VERSION_ADDR,globalParameter.softVersion);
	while(ret != FLASH_COMPLETE);
  }
  FLASH_Lock();
  portEXIT_CRITICAL();
}



void SendOutputMsg(_eOUTPUTMSG_TYPE msg,uint8_t paraType,void* paraPointer)
{
	mOutputMsg->outputMsg = msg;
	mOutputMsg->paramType = (_eMSG_PARAM_TYPE)paraType;
	mOutputMsg->outputMsgParam = paraPointer;
	xQueueSend(outputMsgQueue,mOutputMsg,1);
}




void FilterLiveCheck(void)
{
	if(runningValue.filterState >0)						
		{						  						
			runningValue.filterDis = FILTER_STATE_USEABLE; 						   				
			SendOutputMsg(OUTPUT_MSG_FDIS_NRL, MSG_PARAM_NONE,NULL);										
		}											
		else										
		{						   					
			runningValue.filterDis = FILTER_STATE_CHANGED; 						   								
			SendOutputMsg(OUTPUT_MSG_FDIS_WRN, MSG_PARAM_NONE,NULL);											
		}											
}
void PrintRunningValue(void)
{
//	DEBUG("mode: %d\r",runningValue.mode);
//	DEBUG("pm2.5: %dug/m3\r",runningValue.dustDensity);
//	DEBUG("pm10: %dug/m3\r",runningValue.dustDensitySub);
//	DEBUG("aqi: %d\r",runningValue.aqiLevel);
//	DEBUG("gas: %dR\r",runningValue.gasValue);
//	DEBUG("gasbase: %dR\r",globalParameter.gasBase);
//	DEBUG("temp: %d'C\r",runningValue.temperature);
//	DEBUG("humi: %d\r",runningValue.humi);
//	DEBUG("timing: %d\r",runningValue.timingLevel);
//	DEBUG("target speed : %d rpm\r",runningValue.speed.targetSpd);
//	DEBUG("current speed : %d rpm\r",runningValue.speed.currentSpd);
//	DEBUG("lumin : %dmV\r",runningValue.lumin);
//	DEBUG("net : %d\r",runningValue.netStatus);
	uint8_t buffer[128];
	uint16_t length = 0,length1= 0;
	uint16_t i;
	uint8_t* pointer,*pointer1;
	uint8_t chksum;
	pointer = (uint8_t*)&runningValue;
	length = sizeof(_sRUNNINGVALUE);
	length1 = sizeof(_sREFERENCE_VALUE);
	memset(buffer,0,128);
	pointer1 = buffer;
	pointer1++;
	memcpy(pointer1,pointer,length);
	pointer1+=length;
	pointer = (uint8_t*)&globalParameter;
	memcpy(pointer1,pointer,length1);
	pointer1 = buffer;
	*pointer1++ = 0xFF;
	length += length1;
	chksum = 0;
	for(i=0;i<length;i++)		
	{
		chksum += *(pointer1++);
	}
	*pointer1++ = chksum;
	*pointer1 = 0x55;
	pointer1 = buffer;
	length+=3;
	for(i=0;i<length;i++)
	{
		USART_SendData(USART1,*pointer1);
		pointer1++;
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
  {}
	}
}


uint8_t* GetModeState(void)
{
 return (uint8_t*)(&runningValue.mode);
}

uint8_t* GetDustValue(void)
{
 return (uint8_t*)(&runningValue.dustDensity);
}

uint8_t* GetDustSubValue(void)
{
 return (uint8_t*)(&runningValue.dustDensitySub);
}

uint8_t* GetAqiValue(void)
{
 return (uint8_t*)(&runningValue.aqiLevel);
}

uint8_t* GetTimingValue(void)
{
 return (uint8_t*)(&runningValue.timingLevel);
}

uint8_t* GetFilterRemain(void)
{
 return (uint8_t*)(&runningValue.filterState);
}

uint8_t* GetLedState(void)
{
 return (uint8_t*)(&runningValue.blueLed);
}

uint8_t* GetDevFault(void)
{
 return (uint8_t*)(&runningValue.fault);
}

uint8_t* GetTemp(void)
{
  return (uint8_t*)(&runningValue.temperature);
}


uint8_t* GetHumi(void)
{
  return (uint8_t*)(&runningValue.humi);
}
uint8_t* GetFirmwareVersion(void)
{
	
 return (uint8_t*)(&globalParameter.softVersion);
}

uint8_t* GetCoverState(void)
{
 return (uint8_t*)(&runningValue.topCover);
}

uint8_t* GetTVOCValue(void)
{
  return (uint8_t*)(&runningValue.gasValue);
}

uint8_t* GetLuminValue(void)
{
  return (uint8_t*)(&runningValue.lumin);
}

uint8_t* GetFanSpeed(void)
{
  return (uint8_t*)(&runningValue.speed.currentSpd);
}

uint8_t* GetFanSpdRef(void)
{
 return (uint8_t*)(&globalParameter.speedRef);
}

uint8_t* GetDustRef(void)
{
 return (uint8_t*)(&globalParameter.dustRef);
}

uint8_t* GetAutoSpdRef(void)
{
 return (uint8_t*)(&globalParameter.atuoSpdRef);
}
uint8_t* GetLuminRef(void)
{
  return (uint8_t*)(&globalParameter.lumiRef);
}
uint8_t* GetDustSen(void)
{
 return (uint8_t*)(&globalParameter.dustSen);
}
uint8_t* GetTVOCRef(void)
{
 return (uint8_t*)(&globalParameter.gasRef);
}

uint16_t GetFirmVersion(void)
{
  return globalParameter.softVersion;
}

uint16_t GetWorkingTime(void)
{
	return globalParameter.workingTime;
}
uint16_t GetFilterTime(void)
{
	return globalParameter.filterVar.filterHoursCnt;
}
uint16_t GetRunningTime(void)
{
  return globalParameter.runningTime;
}
#ifndef FOR_JP
uint8_t GetUpdateFlag(void)
{
return updateFlag;
}


uint8_t GetWifiRstFlag(void)
{
	return wifiRstFlag;
}
#endif











