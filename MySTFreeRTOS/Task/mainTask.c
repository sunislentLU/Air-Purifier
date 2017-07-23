#include "mainTask.h"
#include "stdio.h"
#include "string.h"
/*function declare*/
void MainVariablesInit(void);
static void MainSetMode(uint8_t* mode);
static void MainSetSpeed(uint8_t mode);
void MainSetTiming(void);
static void FilterLiveCount(void);
static void FaultDetection(void);
static void TimingCount(void);
static void MainSetBuzzer(uint8_t buzType);
static void MainSetNetState(void);
static void SecondLoopProcess(void);
extern void LoopTimerInit(_sLOOPTIMER* timer,uint16_t interval);
extern uint8_t CheckTickExpired(_sLOOPTIMER* timer);\
void ReadGlobalParameter(void);
void SaveGlobalParameter(void);
void SystemUpdateMark(uint16_t ver);
/*********************variables****************/
_sINPUT_MSG* mInputMsg;
_sWIFI_REC_MSG* mWifiRecMsg;
_sWIFI_SND_MSG* mWifiSndMsg;
_sOUTPUT_MSG* mOutputMsg;
_sRUNNINGVALUE runningValue;
_sREFERENCE_VALUE globalParameter;

extern xQueueHandle inputMsgQueue;
extern xQueueHandle outputMsgQueue;
extern xQueueHandle wifiRecQueue;
extern xQueueHandle wifiSndQueue;
extern xTaskHandle mainTask;
extern uint8_t uartSendBuf[];

xSemaphoreHandle semWifiVariable;

const _sRUNNINGVALUE runningValueDefault = {DEFAULT_MODE,DEFAULT_DUSTVALUE,DEFAULT_DUSTSUBVALUE,DEFAULT_AQILEVEL,
	                                          DEFAULT_TIMING,DEFAULT_LED_STATE,DEFAULT_FILTER_REMAIN,DEFAULT_FAULT,
	                                          DEFAULT_COVER_STATE,DEFAULT_DUST_SENS,DEFAULT_SPEED,DEFAULT_GASVALUE,
                                            DEFAULT_LUMIN,DEFAULT_NETSTATE};

const _sSPEED_REFERENCE spdRefDefault = {DEFAULT_STANDBY_SPD_REF,DEFAULT_JET_SPD_REF,DEFAULT_LOW_SPD_REF,\
	                                     DEFAULT_MEDIUM_SPD_REF,DEFAULT_HIGH_SPD_REF};

const _sAUTOSPD_REF autoSpdRefDefault={DEFAULT_AUTO_LOW,DEFAULT_AUTO_MED,DEFAULT_AUTO_HIGH};																			 

const _sDUST_REFERENCE dustRefDefault={DEFAULT_DUST_GOOD_REF,DEFAULT_DUST_FINE_REF,DEFAULT_DUST_BAD_REF};
const _sGAS_REFERENCE  gasRefDefault = {DEFAULT_GAS_GOOD_REF,DEFAULT_GAS_FINE_REF,DEFAULT_GAS_BAD_REF};
const uint16_t timingTable[4]={TIMING_LEVEL_0,TIMING_LEVEL_1,TIMING_LEVEL_2,TIMING_LEVEL_3};
const _sLUMIN_REF luminDefaultRef={DEFAULT_LUMIN_DARK,DEFAULT_LUMIN_LIGHT};
const _sFILTERLIVE filterRef={DEFAULT_RESET_HOUR,DEFAULT_MAX_USE_HOUR};

_sDATE filterCnt;
uint16_t timingCnt = 0;
uint8_t conTimeOut = 0;
_sLOOPTIMER* secondLoop;
_sLOOPTIMER* mFilterCnt;
/******************************************/

/****************
* Function Name:      MainTask
* Description:        all user interface logic and operation in this task
                      
* Parameter:          arg
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
void MainTask(void* arg)
{
  uint8_t* dataPointer;
  uint16_t dataTemp;
  MainVariablesInit();
  ReadGlobalParameter();
  for(;;)
	{
	if(xQueueReceive(inputMsgQueue,mInputMsg,10) == pdTRUE)// input task messages
	{
		switch(mInputMsg->inputMsg)
		{           
			case KEY_POWER_PRESS:				
				if(runningValue.mode == MODE_STANDBY)
				{
					runningValue.mode = MODE_AUTO;
					MainSetTiming();
				}
				else
				{
					runningValue.mode = MODE_STANDBY;
					if(timingCnt !=0)
						timingCnt = 0;
					if(runningValue.timingLevel != TIMING_LEVEL_NONE)
					{
						runningValue.timingLevel = TIMING_LEVEL_NONE;
					}
				}
				MainSetMode((uint8_t*)&runningValue.mode);
				MainSetSpeed(runningValue.mode);
				MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);

				break;

			case KEY_POWER_LPRESS:
			case KEY_POWER_HOLD:
			break;
			case KEY_MODE_PRESS:
			case KEY_MODE_LPRESS:
				if(runningValue.mode != MODE_STANDBY)// in standby mode no action
				{				
					if(runningValue.mode == MODE_AUTO)					 
						runningValue.mode = MODE_JET;				
					else				 
					runningValue.mode = MODE_AUTO;				
					MainSetMode((uint8_t*)&runningValue.mode);				
					MainSetSpeed(runningValue.mode);				
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
					
				}
				break;
     
			case KEY_MODE_HOLD:
				if(runningValue.mode != MODE_STANDBY)
				{
				mWifiSndMsg->propMsg = WIFI_SET_CONN;
				xQueueSend(wifiSndQueue, mWifiSndMsg,1);
			  MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
			  runningValue.netStatus = 3;
			  MainSetNetState();
			  conTimeOut = 30;				
				}
				break;
							
			case KEY_VOLUME_PRESS:
				//p_uint16 = (uint16_t*)&spdRef;
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
				break;
			case KEY_VOLUME_LPRESS:
			case KEY_VOLUME_HOLD://	
			break;
			
			case KEY_TIMING_PRESS:
				if(runningValue.mode != MODE_STANDBY)// in standby mode no action
				{				
					runningValue.timingLevel++;
				  if(runningValue.timingLevel>TIMING_8_HOUR)
						runningValue.timingLevel = TIMING_LEVEL_NONE;
				    timingCnt  = timingTable[runningValue.timingLevel];
					MainSetTiming();
					MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
				}
				break;

			case KEY_TIMING_LPRESS:
			case KEY_TIMING_HOLD:
				break;
			case INPUT_MSG_DUST:
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
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
				break;
			case INPUT_MSG_DUST_SUB:
				dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
				{
					if(((dataTemp>runningValue.dustDensitySub)&&(dataTemp - runningValue.dustDensitySub>=DUST_UP_THRD))||
						((dataTemp<runningValue.dustDensitySub)&&(runningValue.dustDensitySub-dataTemp>=DUST_UP_THRD)))
					{
						runningValue.dustDensitySub= dataTemp;// *((uint16_t*)mInputMsg->inputMsgParam);
						mWifiSndMsg->propMsg = WIFI_UP_DUST_SUB;
						xQueueSend(wifiSndQueue,mWifiSndMsg,0);
					}
					runningValue.dustDensitySub= dataTemp;
				}

				
				break;
			case INPUT_MSG_GAS:
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
				{								
					dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);				
					dataTemp = dataTemp*4095/3300;	
					if(((dataTemp>runningValue.gasValue)&&(dataTemp - runningValue.gasValue>=GAS_UP_THRD))||
						((dataTemp<runningValue.gasValue)&&(runningValue.gasValue-dataTemp>=GAS_UP_THRD)))
					{
						runningValue.gasValue= dataTemp;// *((uint16_t*)mInputMsg->inputMsgParam);
						mWifiSndMsg->propMsg = WIFI_UP_GAS;
						xQueueSend(wifiSndQueue,mWifiSndMsg,0);
					}
					runningValue.gasValue= dataTemp;// *((uint16_t*)mInputMsg->inputMsgParam);
					
				}
				break;
			case INPUT_MSG_LUMIN:
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
				{
					dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);				
					dataTemp = dataTemp*4095/3300;					
					runningValue.lumin= dataTemp;// *((uint16_t*)mInputMsg->inputMsgParam);
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
					if(dataTemp >=10)
					{
					dataTemp = 48000/dataTemp;
					dataTemp <<=2;
					runningValue.speed.currentSpd = dataTemp;
					}else
					runningValue.speed.currentSpd = 0;
				}
				break;
			case INPUT_MSG_TEMP:
				dataTemp = *((int8_t*)mInputMsg->inputMsgParam);	
			  runningValue.temperature = dataTemp;
			  
				break;
			case INPUT_MSG_HUMI:
				dataTemp = *((int8_t*)mInputMsg->inputMsgParam);	
			  runningValue.humi = dataTemp;
			  
				break;
		default:
			break;
			}         
	}
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
						runningValue.mode = (_eMODE)*dataPointer;
						MainSetMode((uint8_t*)&runningValue.mode);
						MainSetSpeed(runningValue.mode);
						MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
					}
					break;			   
					case WIFI_MSG_LED:
					dataPointer = (uint8_t*)mWifiRecMsg->wifiMsgParam;	
					if((*dataPointer) == 1)
			 	    runningValue.blueLed = BLUE_LED_ON;
					else 
						runningValue.blueLed = BLUE_LED_OFF;					
						mOutputMsg->outputMsgParam = &runningValue.blueLed;
						mOutputMsg->outputMsg = OUTPUT_MSG_BLUELED;
						mOutputMsg->paramType = MSG_PARAM_UCHAR;
						xQueueSend(outputMsgQueue,mOutputMsg,1);
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
	
					break;

					case WIFI_MSG_FILTER:
	
					break;

					case WIFI_MSG_VER:
	
					break;
					case WIFI_MSG_SPEED:
	
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
					break;
					case WIFI_TVOC_REF:
	
					break;			 
				 case WIFI_MSG_NET:
				 	
				 	if(runningValue.netStatus != *((uint8_t*)mWifiRecMsg->wifiMsgParam))
				 	{
						if(*((uint8_t*)mWifiRecMsg->wifiMsgParam) == 3)
							 runningValue.netStatus = 2;
					else
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
/********* to do gas and dust level parse**
*********  filter live count  timing count 
*********  and fan fault detection       ***/

SecondLoopProcess();
FaultDetection();
FilterLiveCount();
// if(runningValue.netStatus >= 1)
// {
// 	upCnt++;
//	if(upCnt >= 500)// 20ms *500 = 10000 ms = 10s
//	{
//		upCnt = 0;
//		mWifiSndMsg->propMsg = WIFI_UP_ALL;
//		xQueueSend(wifiSndQueue,mWifiSndMsg,0);	
//	}
// }
/************************************************/
	}
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
   mWifiRecMsg = pvPortMalloc(sizeof(_sWIFI_REC_MSG));
   mWifiSndMsg = pvPortMalloc(sizeof(_sWIFI_SND_MSG));
   secondLoop = pvPortMalloc(sizeof(_sLOOPTIMER));
   mFilterCnt = pvPortMalloc(sizeof(_sLOOPTIMER));
   semWifiVariable = xSemaphoreCreateBinary();
   LoopTimerInit(secondLoop,1000);//
   LoopTimerInit(mFilterCnt,60000);// one minute per loop 
   memcpy((char*)&runningValue,(const char*)&runningValueDefault,sizeof(runningValueDefault));
   memcpy((char*)&globalParameter.speedRef,(const char*)&spdRefDefault,sizeof(spdRefDefault));
   memcpy((char*)&globalParameter.dustRef,(const char*)&dustRefDefault,sizeof(dustRefDefault));
   memcpy((char*)&globalParameter.gasRef,(const char*)&gasRefDefault,sizeof(gasRefDefault));
   memcpy((char*)&globalParameter.lumiRef,(const char*)&luminDefaultRef,sizeof(luminDefaultRef));
   memcpy((char*)&globalParameter.atuoSpdRef,(const char*)&autoSpdRefDefault,sizeof(autoSpdRefDefault));
}

/****************
* Function Name:      MainSetMode
* Description:        send current mode and send to ouput task make it action
                      
* Parameter:          mode
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
static void MainSetMode(uint8_t* mode)
{
	mOutputMsg->outputMsg = OUTPUT_MSG_MODE;
	mOutputMsg->paramType = MSG_PARAM_UCHAR;
	mOutputMsg->outputMsgParam = mode;
	xQueueSend(outputMsgQueue,mOutputMsg,0);
   mWifiSndMsg->propMsg = WIFI_UP_MODE;
   xQueueSend(wifiSndQueue,mWifiSndMsg,0);
}

/****************
* Function Name:      MainSetBuzzer
* Description:        send buzzer type to outputtask make hardware execute
                      
* Parameter:          buzzer type
* return:             none
* Date:               20170630
*author:              CTK  luxq
***************/

static void MainSetBuzzer(uint8_t buzType)
{
	mOutputMsg->outputMsg = (_eOUTPUTMSG_TYPE)buzType;
	mOutputMsg->paramType = MSG_PARAM_NONE;
	xQueueSend(outputMsgQueue,mOutputMsg,0);
}
static void MainSetNetState(void)
{
	mOutputMsg->outputMsg = OUTPUT_MSG_NET;
	mOutputMsg->paramType = MSG_PARAM_UCHAR;
	mOutputMsg->outputMsgParam = (uint8_t*)(&runningValue.netStatus);
	xQueueSend(outputMsgQueue,mOutputMsg,0);


}

/****************
* Function Name:      MainSetSpeed
* Description:        send current speed and send to ouput task make it action
                      
* Parameter:          speed
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
static void MainSetSpeed(uint8_t mode)
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
	mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
	mOutputMsg->paramType = MSG_PARAM_USHORT;
	mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
	xQueueSend(outputMsgQueue,mOutputMsg,0);
}


void MainSetTiming(void)
{
		
	mOutputMsg->outputMsg = OUTPUT_MSG_TIMING;
	mOutputMsg->paramType = MSG_PARAM_UCHAR;
	mOutputMsg->outputMsgParam = &runningValue.timingLevel;
	xQueueSend(outputMsgQueue,mOutputMsg,0);
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
    if(CheckTickExpired(mFilterCnt))
		{
			filterCnt.min++;
			if(filterCnt.min%10 == 0)
			{
				mWifiSndMsg->propMsg = WIFI_UP_ALL;
				xQueueSend(wifiSndQueue,mWifiSndMsg,0);	
			
			}
	    if(filterCnt.min == 60)
	    {
			filterCnt.min = 0;
			filterCnt.hour++;
			if(runningValue.mode != MODE_STANDBY)
			{
				globalParameter.filterVar.filterHoursCnt++;
				if(globalParameter.filterVar.filterHoursCnt>= globalParameter.filterVar.maxFilterHours)
				{
				  runningValue.filterState = FILTER_STATE_CHANGED;
				}
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
     if(runningValue.mode!= MODE_STANDBY)
			 if(runningValue.speed.currentSpd <= 200)
			 {
				 runningValue.fault = 0x01;
//				 mInputMsg->inputMsg = KEY_POWER_PRESS;
//				 mInputMsg->paramType = MSG_PARAM_NONE;
//			 	xQueueSend(inputMsgQueue,mInputMsg,0);
			 }
}



void AqiCaculation(void)
{
	uint16_t* p_data; 
	uint8_t aqi_tmp;
	if((runningValue.dustDensity <=globalParameter.dustRef.dustGoodRef)&&(runningValue.gasValue <= globalParameter.gasRef.gasGoodRef))
		aqi_tmp= AQI_LEVEL_GOOD;
	if(((runningValue.dustDensity>globalParameter.dustRef.dustGoodRef)&&(runningValue.dustDensity<=globalParameter.dustRef.dustFineRef))||
		((runningValue.gasValue >globalParameter.gasRef.gasGoodRef)&&(runningValue.gasValue<=globalParameter.gasRef.gasFineRef)))
		aqi_tmp = AQI_LEVEL_FINE;
	if(((runningValue.dustDensity>globalParameter.dustRef.dustFineRef))||((runningValue.gasValue >globalParameter.gasRef.gasFineRef)))
		aqi_tmp = AQI_LEVEL_BAD;
	if(runningValue.mode == MODE_AUTO)
	{
		if(runningValue.aqiLevel != aqi_tmp)
			{
				p_data = (uint16_t*)&globalParameter.atuoSpdRef;
				runningValue.aqiLevel = (_eAQI_LEVEL)aqi_tmp;
				runningValue.speed.targetSpd =*(p_data+aqi_tmp);
//				mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
//				mOutputMsg->paramType = MSG_PARAM_USHORT;
//				mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
//				xQueueSend(outputMsgQueue,mOutputMsg,0);
				
				mOutputMsg->outputMsg = OUTPUT_MSG_RGB;
				mOutputMsg->paramType = MSG_PARAM_USHORT;
				mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
				xQueueSend(outputMsgQueue,mOutputMsg,0);				
			}
			p_data = (uint16_t*)&globalParameter.atuoSpdRef;
			 runningValue.speed.targetSpd =*(p_data+aqi_tmp);
				if(runningValue.speed.currentSpd != runningValue.speed.targetSpd)
				{
				mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
				mOutputMsg->paramType = MSG_PARAM_USHORT;
				mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
				xQueueSend(outputMsgQueue,mOutputMsg,0);	
				}					
	}
	
}

static void SecondLoopProcess(void)
{
	if(CheckTickExpired(secondLoop))
	{
	FaultDetection();
	TimingCount();
	AqiCaculation();
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
  	 if(runningValue.mode != MODE_STANDBY)
  	 {
		 runningValue.mode = MODE_STANDBY;
		 MainSetMode((uint8_t*)&runningValue.mode);
		 MainSetSpeed(runningValue.mode);
		 MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
		 runningValue.timingLevel = TIMING_LEVEL_NONE;
	 }
  }
	}
	if(conTimeOut>0)
	{
		conTimeOut--;
		if(conTimeOut == 0)
		{
		//	mWifiSndMsg->propMsg = WIFI_REBOOT;
		//	xQueueSend(wifiSndQueue, mWifiSndMsg, 0);
			
		}


	}
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

void ReadGlobalParameter(void)
{
	GetFlashSpdRef(SPDREF_ADDR);
	GetFlashAutoSpdRef(AUTOSPDREF_ADDR);
	GetFlashDustRef(DUSTREF_ADDR);
	GetFlashLumiRef(LUMIREF_ADDR);
	GetFlashFilterVar(FILTER_ADDR);
	GetFlashSen(DUSTSEN_ADDR);
	GetFlashGasBase(GASBASE_ADDR);
	GetFlashSoftVer(VERSION_ADDR);
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
 return (uint8_t*)(&spdRefDefault);
}
uint8_t* GetLuminRef(void)
{
  return (uint8_t*)(&globalParameter.lumiRef);
}
uint8_t* GetDustSen(void)
{
 return (uint8_t*)(&runningValue.sense);
}
uint8_t* GetTVOCRef(void)
{
 return (uint8_t*)(&globalParameter.gasRef);
}

uint16_t GetFirmVersion(void)
{
  return globalParameter.softVersion;
}











