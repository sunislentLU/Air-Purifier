#include "mainTask.h"
#include "stdio.h"
#include "string.h"
/*function declare*/
void MainVariablesInit(void);
static void MainSetMode(uint8_t* mode);
static void MainSetSpeed(uint8_t mode);
static void FilterLiveCount(void);
static void FaultDetection(void);
static void TimingCount(void);
static void MainSetBuzzer(uint8_t buzType);
static void SecondLoopProcess(void);
extern void LoopTimerInit(_sLOOPTIMER* timer,uint16_t interval);
extern uint8_t CheckTickExpired(_sLOOPTIMER* timer);
/*********************variables****************/
_sINPUT_MSG* mInputMsg;
_sWIFI_REC_MSG* mWifiRecMsg;
_sWIFI_SND_MSG* mWifiSndMsg;
_sOUTPUT_MSG* mOutputMsg;
_sRUNNINGVALUE runningValue;
_sFILTERLIVE filterLive;
_sSPEED_REFERENCE spdRef;
_sAUTOSPD_REF autoSpdRef;
_sDUST_REFERENCE dustRef;
_sGAS_REFERENCE  gasRef;
_sLUMIN_REF  luminRef;



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
const uint8_t timingTable[4]={TIMING_LEVEL_0,TIMING_LEVEL_1,TIMING_LEVEL_2,TIMING_LEVEL_3};
const _sLUMIN_REF luminDefaultRef={DEFAULT_LUMIN_DARK,DEFAULT_LUMIN_LIGHT};

const uint8_t version[2]={0x01,0x05};



_sDATE filterCnt;
uint16_t timingCnt = 0;
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

//uint32_t stackRemain;
void MainTask(void* arg)
{
  uint8_t* dataPointer;
  uint16_t upCnt;
  uint16_t dataTemp;
  MainVariablesInit();
  for(;;)
	{
	if(xQueueReceive(inputMsgQueue,mInputMsg,10) == pdTRUE)// input task messages
	{
		switch(mInputMsg->inputMsg)
		{           
			case KEY_POWER_PRESS:
			case KEY_POWER_LPRESS:
			case KEY_POWER_HOLD:
				
				if(runningValue.mode == MODE_STANDBY)
				{
					runningValue.mode = MODE_AUTO;
				}
				else
				{
					runningValue.mode = MODE_STANDBY;
					if(timingCnt !=0)
						timingCnt = 0;
					if(runningValue.timingLevel != TIMING_LEVEL_NONE)
						runningValue.timingLevel = TIMING_LEVEL_NONE;
				}
				MainSetMode((uint8_t*)&runningValue.mode);
				MainSetSpeed(runningValue.mode);
				MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
				break;
			case KEY_MODE_PRESS:
			case KEY_MODE_LPRESS:
			case KEY_MODE_HOLD:
//				p_uint16 = (uint16_t*)&spdRef;
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
			case KEY_VOLUME_PRESS:
			case KEY_VOLUME_LPRESS:
			case KEY_VOLUME_HOLD://				p_uint16 = (uint16_t*)&spdRef;
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
                    ;
				}
				}
				break;				            
			case KEY_TIMING_PRESS:
			case KEY_TIMING_LPRESS:
			case KEY_TIMING_HOLD:
				if(runningValue.mode != MODE_STANDBY)// in standby mode no action
				{
				runningValue.timingLevel++;
				  if(runningValue.timingLevel>TIMING_8_HOUR)
					runningValue.timingLevel = TIMING_LEVEL_NONE;
				  timingCnt  = timingTable[runningValue.timingLevel]*60*60;
				MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
				}
				break;
			case INPUT_MSG_DUST:
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
				{
				runningValue.dustDensity = *((uint16_t*)mInputMsg->inputMsgParam);
				}
				break;
			case INPUT_MSG_GAS:
				if(mInputMsg->paramType == MSG_PARAM_SHORT)
				{
				
				dataTemp = *((uint16_t*)mInputMsg->inputMsgParam);
				dataTemp = dataTemp*4095/3300;		
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
						runningValue.mode = (_eMODE)*dataPointer;
						MainSetMode((uint8_t*)&runningValue.mode);
						MainSetSpeed(runningValue.mode);
						MainSetBuzzer(OUTPUT_MSG_BUZZ_KEY);
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
	
					break;
					case WIFI_TVOC_REF:
	
					break;			 
				 case WIFI_MSG_NET:
				  runningValue.netStatus = *((uint8_t*)mWifiRecMsg->wifiMsgParam);
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
	
 if(runningValue.netStatus >= 1)
 {
 	upCnt++;
	if(upCnt >= 500)// 20ms *500 = 10000 ms = 10s
	{
		upCnt = 0;
		mWifiSndMsg->propMsg = WIFI_UP_ALL;
		xQueueSend(wifiSndQueue,mWifiSndMsg,0);	
	}
 }
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
   memcpy((char*)&spdRef,(const char*)&spdRefDefault,sizeof(spdRefDefault));
   memcpy((char*)&dustRef,(const char*)&dustRefDefault,sizeof(dustRefDefault));
   memcpy((char*)&gasRef,(const char*)&gasRefDefault,sizeof(gasRefDefault));
   memcpy((char*)&luminRef,(const char*)&luminDefaultRef,sizeof(luminDefaultRef));
   memcpy((char*)&autoSpdRef,(const char*)&autoSpdRefDefault,sizeof(autoSpdRefDefault));
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
			runningValue.speed.targetSpd = spdRef.standbySpdRef;
			break;
		case MODE_JET:
			runningValue.speed.targetSpd = spdRef.jetSpdRef;
			break;
		case MODE_LOW:
			runningValue.speed.targetSpd = spdRef.lowSpdRef;
			break;
		case MODE_AUTO:
			runningValue.speed.targetSpd = autoSpdRef.autoLowSpdRef;
			break;
		case MODE_MEDIUM:
			runningValue.speed.targetSpd = spdRef.mediumSpdRef;
			break;
		case MODE_HIGH:
			runningValue.speed.targetSpd = spdRef.highSpdRef;
			break;
		default:
		break;
	}
	mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
	mOutputMsg->paramType = MSG_PARAM_USHORT;
	mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
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
	    if(filterCnt.min == 60)
	    {
			filterCnt.min = 0;
			filterCnt.hour++;
			if(runningValue.mode != MODE_STANDBY)
			{
				filterLive.filterHoursCnt++;
				if(filterLive.filterHoursCnt >= filterLive.maxFilterHours)
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
				 runningValue.fault = 0x01;
}



void AqiCaculation(void)
{
	uint16_t* p_data; 
	uint8_t aqi_tmp;
	if((runningValue.dustDensity <=dustRef.dustGoodRef)&&(runningValue.gasValue <= gasRef.gasGoodRef))
		aqi_tmp= AQI_LEVEL_GOOD;
	if(((runningValue.dustDensity>dustRef.dustGoodRef)&&(runningValue.dustDensity<=dustRef.dustFineRef))||
		((runningValue.gasValue >gasRef.gasGoodRef)&&(runningValue.gasValue<=gasRef.gasFineRef)))
		aqi_tmp = AQI_LEVEL_FINE;
	if(((runningValue.dustDensity>dustRef.dustFineRef))||((runningValue.gasValue >gasRef.gasFineRef)))
		aqi_tmp = AQI_LEVEL_BAD;
	if(runningValue.mode == MODE_AUTO)
	{
	if(runningValue.aqiLevel != aqi_tmp)
	  {
	p_data = (uint16_t*)&autoSpdRef;
	runningValue.aqiLevel = (_eAQI_LEVEL)aqi_tmp;
	runningValue.speed.targetSpd =*(p_data+aqi_tmp);
	mOutputMsg->outputMsg = OUTPUT_MSG_SPEED;
	mOutputMsg->paramType = MSG_PARAM_USHORT;
	mOutputMsg->outputMsgParam = &runningValue.speed.targetSpd;
	xQueueSend(outputMsgQueue,mOutputMsg,0);
	  }
  }
	
}

static void SecondLoopProcess(void)
{
	FaultDetection();
	TimingCount();
	AqiCaculation();
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
		 runningValue.timingLevel = TIMING_LEVEL_NONE;
	 }
  }
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

uint8_t* GetFirmwareVersion(void)
{
 return (uint8_t*)(&version[1]);
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
 return (uint8_t*)(&spdRef);
}

uint8_t* GetDustRef(void)
{
 return (uint8_t*)(&dustRef);
}

uint8_t* GetAutoSpdRef(void)
{
 return (uint8_t*)(&spdRefDefault);
}
uint8_t* GetLuminRef(void)
{
  return (uint8_t*)(&luminRef);
}
uint8_t* GetDustSen(void)
{
 return (uint8_t*)(&runningValue.sense);
}
uint8_t* GetTVOCRef(void)
{
 return (uint8_t*)(&gasRef);
}











