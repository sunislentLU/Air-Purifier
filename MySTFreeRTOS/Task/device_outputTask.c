#include "device_outputTask.h"
#include "bspOutput.h"
#include "stdio.h"
#include "string.h"
#include "mainTask.h"
/******Functions Declare **********/
extern void SetLed1On(void);
extern void SetLed1Off(void);
extern void ChangeFanPeriod(uint16_t freq);
extern void LoopTimerInit(_sLOOPTIMER * timer, uint16_t interval);
extern uint8_t CheckTickExpired(_sLOOPTIMER * timer);
extern void EnableRGBLEDLight(uint8_t en);
extern uint8_t* GetAqiValue(void);
extern uint8_t* GetModeState(void);
extern uint16_t GetRGBCurrentCompare_R(void);
extern uint16_t GetRGBCurrentCompare_G(void);
extern uint16_t GetRGBCurrentCompare_B(void);
extern uint16_t GetLightCurrentCompare(void);
extern void OutputTestHandle(void);
extern void FanTimerCounterStop(void);
extern void FanTimerStart(void);

uint8_t GetTestModeFlag(void);
void LedOffAll(void);
void OutputVariablesInit(void); 
void SetFanSpeed(uint16_t spd);
static void SetMode(uint8_t mode);
static void SetLightBrightness(uint16_t brightness);
void SetBlueLed(uint8_t op);
void SetBuzzer(uint8_t op);
void SetTimgLed(uint8_t timingType);
void BuzzerLedTimer(void);
void RgbLightFade(void);
void SetAllLedPowerOff(void);
void SetAllLedPowerOn(void);
void PowerOffDisplay(void);
void PowerOnDisplay(void);
void OprationIndLedsOn(void);
void OprationIndLedsOff(void);
void MotorPowerProcess(void);
#ifndef FOR_JP
extern uint8_t GetWifiRstFlag(void);
#endif
/*********Variables*****/
extern xTaskHandle deviceOutputTask;
xQueueHandle outputMsgQueue;
_sOUTPUT_MSG* outputMsg;
_sOUTPUT_VAR outputVariable;
_sLOOPTIMER* buzLedTimer;
extern _sRGBLIGHT rgbLightValue;
uint8_t 	ledStandbyFlag = 1;

//_sLightAdj ledAdj;
//uint8_t ledDeltaCnt;
//uint16_t brightnessTmp;
//const _sRGBLIGHT rgbValueRef[4]={{0x00,0x00,0x00,0x00,0x00},{0x00,0x3f,0xff,0xff,0xff},
//	                             {0xff,0xff,0x00,0xff,0xff},{0xef,0x3f,0x00,0xff,0xff}};
//								 
//const _sRGBLIGHT rgbValueRef[4]={{0x00,0x00,0x00,0x00,0x00},{0x00,0x26,0x9a,0xCC,0xCC},
//	                             {0x4d,0x9a,0x00,0xCC,0xCC},{0x48,0x26,0x00,0xCC,0xCC}};

const _sRGBLIGHT rgbValueRef[4]={{0x00,0x00,0x00,0x00,0x00},{0x00,0x15,0x9a,0xCC,0x9a},
	                             {0x66,0x9a,0x00,0xCC,0x9a},{0x66,0x22,0x00,0xCC,0x9a}};															 								 
const _sRGBLIGHT rgbValueRefDark[4]={{0x00,0x00,0x00,0x00,0x00},{0x00,0x04,0x10,0x10,0x10},
	                             {0x08,0x10,0x00,0x10,0x10},{0x0f,0x04,0x00,0x10,0x10}};

const uint32_t RGBLightRegister[5]={(TIM3_BASE+0x34),(TIM3_BASE+0x38),(TIM3_BASE+0x3C),(TIM3_BASE+0x40),(TIM15_BASE + 0x38)};
const uint16_t RGBFadeStep[3][5] = {{0xfff,0x15f,0x9af,0xfff,0xfff},
                                    {0x66f,0x9af,0xfff,0xfff,0xfff},
                                    {0x66f,0x22f,0xfff,0xfff,0xfff}};
/****************
* Function Name:      DeviceOutputTask
* Description:        hardware interface control setting  
                      
* Parameter:          arg  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
																		   
uint8_t MotorDelayTime;

void DeviceOutputTask(void* arg)
{
	uint16_t dataTmp;
   OutputHardwareInit();
   PowerOnDisplay();
   vTaskDelay(1500);
   PowerOffDisplay();
	outputVariable.light = 1;
for(;;)
{
	if(GetTestModeFlag())
	{
	   OutputTestHandle();
	   BuzzerLedTimer();
	}
  else
	{		
	if(xQueueReceive(outputMsgQueue, outputMsg, 10))// receive a msg 
    {
     switch(outputMsg->outputMsg)
     {
	 	     
			 case OUTPUT_MSG_LEDSON:
			 	 OprationIndLedsOn();
			 	break;
			 	case OUTPUT_MSG_LEDSOFF:
			 	 OprationIndLedsOff();
			 	break;
			 case OUTPUT_MSG_MODE:
		   	if(outputMsg->paramType != MSG_PARAM_UCHAR)// message's parameter type error
		   	{
                ;// can not return  task is not allow return 
			}else
			{
				dataTmp = *((uint8_t*)outputMsg->outputMsgParam);
				outputVariable.mode = dataTmp;
				if(outputVariable.mode == MODE_STANDBY)//上一个状态是关机		
					MotorDelayTime = 100;
				SetMode((_eMODE)dataTmp);
			}
		   	break;           
			 case OUTPUT_MSG_LIGHT:
			 	dataTmp = *((uint16_t*)outputMsg->outputMsgParam);
				if(dataTmp >=DEFAULT_LUMIN_LIGHT)
				{
					if(outputVariable.light == LIGHT_BRIGHT)
					{
					memcpy(&rgbLightValue,&rgbValueRef[outputVariable.aqiLevel+1],(sizeof(_sRGBLIGHT)-2));
					rgbLightValue.FilterCompare = rgbValueRef[1].FilterCompare;
					}
					//outputVariable.light = LIGHT_BRIGHT;
						
					SetLightBrightness(rgbValueRef[1].LuminCompare);
				}
				else if(dataTmp <= DEFAULT_LUMIN_DARK)
				{
					//outputVariable.light = LIGHT_DARK;
					if(outputVariable.light == LIGHT_BRIGHT)
					{
					  rgbLightValue.FilterCompare = rgbValueRefDark[1].FilterCompare;
					  memcpy(&rgbLightValue,&rgbValueRefDark[outputVariable.aqiLevel+1],(sizeof(_sRGBLIGHT)-2));
					}
					SetLightBrightness(rgbValueRefDark[1].LuminCompare);
					
				}
		   	break;
		   case OUTPUT_MSG_BLUELED:
		   	  outputVariable.light = *((uint8_t*)outputMsg->outputMsgParam);
				SetBlueLed(*((uint8_t*)outputMsg->outputMsgParam));
		   	break;
		   case OUTPUT_MSG_BUZZ_KEY:
		   case OUTPUT_MSG_BUZZ_CONF:
		   case OUTPUT_MSG_BUZZ_WARN:
#ifdef BUZ_TEST
			 case OUTPUT_MSG_BUZ_TST1:
			 case OUTPUT_MSG_BUZ_TST2:
			 case OUTPUT_MSG_BUZ_TST3:	
			 case OUTPUT_MSG_BUZ_TST4:
			 case OUTPUT_MSG_BUZ_TST5:
#endif
				 SetBuzzer(outputMsg->outputMsg - OUTPUT_MSG_BUZZ_KEY +1);
		   	break;
		   case OUTPUT_MSG_SPEED:
		     if(outputMsg->paramType == MSG_PARAM_USHORT)	   	
				 {	
				 	dataTmp = *((uint16_t*)outputMsg->outputMsgParam);
					 SetFanSpeed(dataTmp);
		   	break;
		   case OUTPUT_MSG_RGB:
				 dataTmp = *((uint8_t*)outputMsg->outputMsgParam);
				 outputVariable.aqiLevel = (uint8_t)dataTmp;				
		   	break;
			 case OUTPUT_MSG_TIMING:
			 	dataTmp = *(uint8_t*)outputMsg->outputMsgParam;
				if(dataTmp!=0)
				{
				   if(dataTmp <=3)
					outputVariable.timingLedType=TIMINGLED_TYPE_1;
					else if(dataTmp <=7)
					outputVariable.timingLedType=TIMINGLED_TYPE_2;
					else if(dataTmp<=12)
					outputVariable.timingLedType=TIMINGLED_TYPE_3;
					else
					outputVariable.timingLedType=TIMINGLED_NONE;
				}else
					outputVariable.timingLedType=TIMINGLED_NONE;
				 SetTimgLed(outputVariable.timingLedType);
				 break;
				 case OUTPUT_MSG_NET:
				 outputVariable.wifiLedType = (_eWIFILED)(*(uint8_t*)outputMsg->outputMsgParam);
				 break;
			 case OUTPUT_MSG_FDIS_NRL:
			 case OUTPUT_MSG_FDIS_CLR:
			 case OUTPUT_MSG_FDIS_WRN:
			 	outputVariable.filter = outputMsg->outputMsg - OUTPUT_MSG_FDIS_NRL;
			 	break;
			 default:
				break;
	 }

}

}
	BuzzerLedTimer();
	}

}
}


/****************
* Function Name:      OutputVariablesInit
* Description:        init message and queue 
                      
* Parameter:          none  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
void OutputVariablesInit(void)
{  
	outputMsgQueue = xQueueCreate(OUTPUT_QUEUE_LEN, sizeof(_sOUTPUT_MSG));
	outputMsg = pvPortMalloc(sizeof(_sOUTPUT_MSG));
	outputVariable.buzType = BUZ_TYPE_NONE;
	outputVariable.wifiLedType = WIFILED_UCON;
  outputVariable.timingLedType = TIMINGLED_NONE;		
	buzLedTimer = pvPortMalloc(sizeof(_sLOOPTIMER));
	LoopTimerInit(buzLedTimer, 20);

}

/****************
* Function Name:      SetFanSpeed
* Description:        set timer period or pwm duty to change 
                      
* Parameter:          none  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
 void SetFanSpeed(uint16_t spd)
{
	uint16_t period,freq;
	float tmp1;
	freq = spd>>1;// 15 pulse per round
	if(freq == 0)
	{
    period = FAN_PERIOD_MAXCOUNT -1;
	MotorPowerCtrl(OFF);
	ChangeFanPeriod(period);
	//MotorPowerCtrl(OFF);
	FanTimerCounterStop();
	}
	else
	{
	tmp1 = ((FAN_PERIOD_COUNT*1.0)/freq);
	period = (uint16_t)tmp1;//((FAN_PERIOD_MAXCOUNT)/freq);
	ChangeFanPeriod(period);
	//MotorPowerCtrl(ON);
	FanTimerStart();
	}
	
}
/****************
* Function Name:      SetModeLed
* Description:        change gpio to set mode led state
                      
* Parameter:          none  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
uint8_t ledStandbyFlag;
static void SetModeLed(uint8_t mode)
{
  switch(mode)
  {
  case MODE_STANDBY:
  	//POWER_LED_OFF();
  break;
  case MODE_AUTO:
	LedOffAll();  	
	AUTO_LED_ON();
	POWER_LED_ON();

  	break;
  case MODE_JET:
		 LedOffAll();
  	FAST_LED_ON();
	  POWER_LED_ON();

  	break;
  case MODE_LOW:
		LedOffAll();
  	LOW_LED_ON();
	  POWER_LED_ON();
  	break;
  case MODE_MEDIUM:
		LedOffAll();
  	MEDIUM_LED_ON();
	  POWER_LED_ON();
    break;
  case MODE_HIGH:
		LedOffAll();
  	HIGH_LED_ON();
	  POWER_LED_ON();
  	break;
  default:
  	break;
  }
}


void SetTimgLed(uint8_t timingType)
{
	TIM1_LED_OFF();
	TIM2_LED_OFF();
	TIM3_LED_OFF();
  switch(timingType)
  {  
		case TIMINGLED_NONE:		
		TIM1_LED_OFF();
		TIM2_LED_OFF();
		TIM3_LED_OFF();
  	break;  
		case TIMINGLED_TYPE_1:			
		TIM1_LED_ON();
  	break;
  case TIMINGLED_TYPE_2:
		TIM2_LED_ON();
  	break;
  case TIMINGLED_TYPE_3:
		TIM3_LED_ON();
  	break;
  default:
  	break;
  }
}



void SetAllLedPowerOff(void)
{			
	memcpy(&rgbLightValue,&rgbValueRef[0],sizeof(_sRGBLIGHT));
}
void SetAllLedPowerOn(void)
{			
	memcpy(&rgbLightValue,&rgbValueRef[outputVariable.aqiLevel+1],sizeof(_sRGBLIGHT));
}


/****************
* Function Name:      SetModeLed
* Description:        change gpio to set mode led state
                      
* Parameter:          none  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/

static void SetMode(uint8_t mode)
{  
	SetModeLed(mode); 
	if(mode!=MODE_STANDBY)
	{
		ledStandbyFlag = 0;
		//SetAllLedPowerOn();

	}else
	{				
		ledStandbyFlag = 1;
		SetAllLedPowerOff();
	}
	
}




/****************
* Function Name:      SetLightBrightness
* Description:        change timer pwm duty to change led brightness
                      
* Parameter:          brightness  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
static void SetLightBrightness(uint16_t brightness)
{
//	outputVariable.ledTarLight = brightness;
	rgbLightValue.LuminCompare = brightness;
}

/****************
* Function Name:      SetBlueLed
* Description:        turn on or turn off blue led
                      
* Parameter:           op : on or off  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
void SetBlueLed(uint8_t op)
{
	if(op == 1)  
	{
		//SetAllLedPowerOn();
		rgbLightValue.FilterCompare = rgbValueRef[outputVariable.aqiLevel+1].FilterCompare;
		rgbLightValue.RGB_RCompare= rgbValueRef[outputVariable.aqiLevel+1].RGB_RCompare;
		rgbLightValue.RGB_GCompare= rgbValueRef[outputVariable.aqiLevel+1].RGB_GCompare;
		rgbLightValue.RGB_BCompare= rgbValueRef[outputVariable.aqiLevel+1].RGB_BCompare;
		
	}
	else
	{
		rgbLightValue.FilterCompare = rgbValueRef[0].FilterCompare;
		rgbLightValue.RGB_RCompare= rgbValueRef[0].RGB_RCompare;
		rgbLightValue.RGB_GCompare= rgbValueRef[0].RGB_GCompare;
		rgbLightValue.RGB_BCompare= rgbValueRef[0].RGB_BCompare;	
	}
}

/****************
* Function Name:      SetBuzzer
* Description:        set the way of buzzer ring 
                      
* Parameter:           op : enum of buzzer type 
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
 void SetBuzzer(uint8_t buztype)
{
	outputVariable.buzType = (_eBUZTYPE)buztype;
}




void LedOffAll(void)
{
 POWER_LED_OFF();
 AUTO_LED_OFF();
 FAST_LED_OFF();
 LOW_LED_OFF();
 MEDIUM_LED_OFF();
 HIGH_LED_OFF();
}
#ifdef BUZ_TEST
static uint8_t buzRepeatCnt = 0;
#endif
void BuzzerLedTimer(void)
{
	static uint8_t buzCnt = 0;
	static uint8_t ledCnt = 0;
	static uint8_t filterLedCnt = 0;
	static uint8_t buzTypeTmp = 0;
	if(CheckTickExpired(buzLedTimer))
	{
		if(buzTypeTmp != outputVariable.buzType)
		{
			//buzCnt = 0;
			if(buzCnt == 0)
			{
			buzTypeTmp = outputVariable.buzType;
			}
		}
		switch(buzTypeTmp)
		{
		case BUZ_TYPE_NONE:
			if(GetBuzOnOffStatus() == ON)
			BuzzerOnOff(OFF);
			break;
		case BUZ_TYPE_KEY:
			if(buzCnt==0)
			BuzzerOnOff(ON);
			if(buzCnt == 3)
			{
				buzCnt = 0;
				BuzzerOnOff(OFF);
				outputVariable.buzType = BUZ_TYPE_NONE;
				break;
			}
			buzCnt++;
			break;
		case BUZ_TYPE_CONF:
			if(buzCnt==0)
			BuzzerOnOff(ON);
			if(buzCnt >= 20)
			{
				BuzzerOnOff(OFF);
				buzCnt = 0;
				outputVariable.buzType = BUZ_TYPE_NONE;
				break;
			}
			buzCnt++;
			break;
		case BUZ_TYPE_WARN:
			if(buzCnt==0)
				BuzzerOnOff(ON);
			if(buzCnt == 5)
				BuzzerOnOff(OFF);
			if(buzCnt == 8)
				BuzzerOnOff(ON);
			if(buzCnt == 13)
				BuzzerOnOff(OFF);
			if(buzCnt == 16)
				BuzzerOnOff(ON);
			if(buzCnt == 21)
				BuzzerOnOff(OFF);
			if(buzCnt == 24)
				BuzzerOnOff(ON);
			if(buzCnt == 29)
				BuzzerOnOff(OFF);
			if(buzCnt == 32)
				BuzzerOnOff(ON);
			if(buzCnt == 37)
			{
				BuzzerOnOff(OFF);
				outputVariable.buzType = BUZ_TYPE_NONE;
				buzCnt = 0;
				break;
			}    
			buzCnt++;
			break;
		default:
			break;
		}
#ifndef FOR_JP
    if((GetTestModeFlag() == 0)&&(GetWifiRstFlag() == 0))
		{

		switch(outputVariable.wifiLedType)
		{
		case WIFILED_UCON:
		case WIFI_OFFLINE:
		if(ledCnt == 0)		   	
			WIFI_LED_ON(); 
		if(ledCnt == 5)
			WIFI_LED_OFF();
			ledCnt++;
		if(ledCnt == 55)
			ledCnt = 0;
			break;
		case WIFILED_ROUTER:
			if(ledCnt == 0)
		   	WIFI_LED_ON();
		   if(ledCnt == 25)
		   	WIFI_LED_OFF();
			ledCnt++;
			if(ledCnt == 100)
				ledCnt = 0;
			break;
		case WIFILED_CLOUD:
			WIFI_LED_ON();
			break;
		case WIFILED_CONFIG:
			if(ledCnt == 0)
		   	WIFI_LED_ON();
		   if(ledCnt == 5)
		   	WIFI_LED_OFF();
			 	 ledCnt++;
		   if(ledCnt >= 10)
		   	ledCnt = 0;
			break;
		default:
			break;
		}
#else
    if(GetTestModeFlag() == 0)
    {
#endif
		switch(outputVariable.filter)
		{
			case FILTER_LED_NORMAL:
				FILTER_LED_OFF();
				break;
			case FILTER_LED_CLR:
				if(filterLedCnt == 0)
					FILTER_LED_ON();
				else if(filterLedCnt == 25)
					FILTER_LED_OFF();
				filterLedCnt++;		
				if(filterLedCnt >= 50)
					 filterLedCnt = 0;
						
				break;
			case FILTER_LED_WARN:
					FILTER_LED_ON();
				break;




		default:
			break;}
		
	}
		RgbLightFade();
		MotorPowerProcess();
}		
}

void MotorPowerProcess(void)
{
	if(MotorDelayTime!=0)	
		MotorDelayTime--;
	else if(outputVariable.mode!=MODE_STANDBY)
		MotorPowerCtrl(ON);

}

//_eBOOL ledFlag = FALSE;
void RgbLightFade(void)
{
	uint16_t currentCC;
	uint8_t* rgbData;
	uint16_t tmp = 0;
	uint16_t tmp2;
	uint8_t i = 0,j=0;
	rgbData = &(rgbLightValue.RGB_RCompare);
	j = *(uint8_t*)GetAqiValue();
	for(i=0;i<5;i++)
	{
		currentCC = (*((uint32_t*)RGBLightRegister[i]))&0x0000ffff;
		tmp = *(rgbData+i);
		if((i == 4)||(i == 3) )
		  tmp <<= 8;	
		else
		tmp <<=7;

	if(currentCC != tmp)
	{
		if(currentCC>tmp)
		{
			tmp2 = RGBFadeStep[j][i];
			if((i == 4)||(i == 3) )
				tmp2>>=2;
			else if(GetTestModeFlag())
				tmp2 = tmp2;
			else
			   tmp2>>=3;
			if(currentCC<tmp2)
				currentCC = tmp;
			else
			currentCC -= tmp2;
			if(currentCC<=(tmp))
			{
				currentCC = (tmp);

			}
		}else 
		{
			tmp2 = RGBFadeStep[j][i];
			if((i == 4)||(i == 3) )
				tmp2>>=2;
			else if(GetTestModeFlag())
				tmp2 = tmp2;
			else
			    tmp2>>=3;
			if(currentCC>(0xffff - tmp2))
				currentCC = (tmp);
			else
			currentCC += tmp2;
			if(currentCC>=tmp)
			{
				currentCC = tmp;
		//		if((i == 4)&&(tmp >= 0xcc00))
		//		ledFlag = TRUE;
			}
		}
		*((uint32_t*)RGBLightRegister[i])= currentCC;
	}
	}
		if(ledStandbyFlag == 1)
	{
		tmp = *(rgbData+i);
		tmp <<=7;
//		if(i==4)
//			tmp <<=8;
			
		for(i=0;i<5;i++)
		{
			currentCC = (*((uint32_t*)RGBLightRegister[i]))&0x0000ffff;
			if(currentCC!=tmp)
				break;	
		}
		if(i==5)
		{
				LedOffAll();	
				TIM1_LED_OFF();
				TIM2_LED_OFF();
				TIM3_LED_OFF();
				WIFI_LED_OFF();
				//ledFlag = FALSE;
		}
	
	}
}




void PowerOnDisplay(void)
{
 POWER_LED_ON();
 AUTO_LED_ON();
 FAST_LED_ON();
 LOW_LED_ON();
 MEDIUM_LED_ON();
 HIGH_LED_ON();	
TIM1_LED_ON();
TIM2_LED_ON();
TIM3_LED_ON();
//WIFI_LED_ON();
FILTER_LED_ON();
*((uint32_t*)RGBLightRegister[3])= 0x3FFF;
*((uint32_t*)RGBLightRegister[4])= 0x3FFF;
*((uint32_t*)RGBLightRegister[2])= 0x3FFF;
}

void PowerOffDisplay(void)
{
 POWER_LED_OFF();
 AUTO_LED_OFF();
 FAST_LED_OFF();
 LOW_LED_OFF();
 MEDIUM_LED_OFF(); 
 HIGH_LED_OFF();	
TIM1_LED_OFF();
TIM2_LED_OFF();
TIM3_LED_OFF();
WIFI_LED_OFF();
FILTER_LED_OFF();
*((uint32_t*)RGBLightRegister[3])= 0;
*((uint32_t*)RGBLightRegister[4])= 0;
*((uint32_t*)RGBLightRegister[2])= 0;
}

void OprationIndLedsOn(void)
{
 POWER_LED_ON();
 AUTO_LED_ON();
 FAST_LED_ON();
 LOW_LED_ON();
 MEDIUM_LED_ON();
 HIGH_LED_ON();	
TIM1_LED_ON();
TIM2_LED_ON();
TIM3_LED_ON();
//WIFI_LED_ON();
rgbLightValue.LuminCompare = 0xcc;
}

void OprationIndLedsOff(void)
{
 POWER_LED_OFF();
 AUTO_LED_OFF();
 FAST_LED_OFF();
 LOW_LED_OFF();
 MEDIUM_LED_OFF(); 
 HIGH_LED_OFF();	
TIM1_LED_OFF();
TIM2_LED_OFF();
TIM3_LED_OFF();
//WIFI_LED_OFF();
rgbLightValue.LuminCompare = 0x00;
}

//_eBOOL GetLedOnOffFlag(void)
//{
//	return ledFlag;

//}







