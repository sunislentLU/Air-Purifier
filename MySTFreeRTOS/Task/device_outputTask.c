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


void LedOffAll(void);
void OutputVariablesInit(void);
static void SetFanSpeed(uint16_t spd);
static void SetMode(uint8_t mode);
static void SetLightBrightness(uint16_t brightness);
void SetBlueLed(uint8_t op);
static void SetBuzzer(uint8_t op);
void SetTimgLed(uint8_t timingType);
void BuzzerLedTimer(void);
void RgbLightFade(void);
void SetAllLedPowerOff(void);
void SetAllLedPowerOn(void);
void SetTestSpdType(uint8_t tstspdtype);
void SetTestSpdValue(uint8_t spd);

/*********Variables*****/
extern xTaskHandle deviceOutputTask;
xQueueHandle outputMsgQueue;
_sOUTPUT_MSG* outputMsg;
_sOUTPUT_VAR outputVariable;
_sLOOPTIMER* buzLedTimer;
extern _sRGBLIGHT rgbLightValue;

const _sRGBLIGHT rgbValueRef[4]={{0x00,0x00,0x00,0x00,0x00},{0x00,0x3f,0xff,0xff,0xff},
	                             {0xff,0xff,0x00,0xff,0xff},{0xef,0x3f,0x00,0xff,0xff}};
const _sRGBLIGHT rgbValueRefDark[4]={{0x00,0x00,0x00,0x00,0x00},{0x00,0x04,0x10,0x10,0x10},
	                             {0x10,0x10,0x00,0x10,0x10},{0x0f,0x04,0x00,0x10,0x10}};

const uint32_t RGBLightRegister[5]={(TIM3_BASE+0x34),(TIM3_BASE+0x38),(TIM3_BASE+0x3C),(TIM3_BASE+0x40),(TIM15_BASE + 0x38)};
const uint16_t RGBFadeStep[3][5] = {{0xfff,0x3ff,0xfff,0xfff,0xfff},
                                    {0xfff,0xfff,0xfff,0xfff,0xfff},
                                    {0xeff,0x3ff,0xfff,0xfff,0xfff}};
/****************
* Function Name:      DeviceOutputTask
* Description:        hardware interface control setting  
                      
* Parameter:          arg  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
																		   
uint8_t mode;
void DeviceOutputTask(void* arg)
{
	uint16_t dataTmp;
   OutputHardwareInit();
for(;;)
{
if(xQueueReceive(outputMsgQueue, outputMsg, 10))// receive a msg 
{
     switch(outputMsg->outputMsg)
     {
			 case OUTPUT_MSG_MODE:
		   	if(outputMsg->paramType != MSG_PARAM_UCHAR)// message's parameter type error
		   	{
                ;// can not return  task is not allow return 
			}else
			{
				mode = *((uint8_t*)outputMsg->outputMsgParam);
				if(mode >= MODE_TEST)
				{
					//SetAllLedPowerOn();
					rgbLightValue.LuminCompare = 0x7f;
					SetTestSpdType(mode);

				}else
				SetMode((_eMODE)mode);
			}
		   	break;           
			 case OUTPUT_MSG_LIGHT:
			 	dataTmp = *((uint16_t*)outputMsg->outputMsgParam);
				if(dataTmp >=DEFAULT_LUMIN_LIGHT)
				{
					SetLightBrightness(rgbValueRef[1].LuminCompare);
					rgbLightValue.FilterCompare = rgbValueRef[1].FilterCompare;
					outputVariable.light = LIGHT_BRIGHT;
				memcpy(&rgbLightValue,&rgbValueRef[outputVariable.aqiLevel+1],(sizeof(_sRGBLIGHT)-2));				
				}
				else if(dataTmp <= DEFAULT_LUMIN_DARK)
				{
				outputVariable.light = LIGHT_DARK;
				SetLightBrightness(rgbValueRefDark[1].LuminCompare);
				rgbLightValue.FilterCompare = rgbValueRefDark[1].FilterCompare;
				memcpy(&rgbLightValue,&rgbValueRefDark[outputVariable.aqiLevel+1],(sizeof(_sRGBLIGHT)-2));
				}
		   	break;
		   case OUTPUT_MSG_BLUELED:
				SetBlueLed(*((uint8_t*)outputMsg->outputMsgParam));
		   	break;
		   case OUTPUT_MSG_BUZZ_KEY:
		   case OUTPUT_MSG_BUZZ_CONF:
		   case OUTPUT_MSG_BUZZ_WARN:
				 SetBuzzer(outputMsg->outputMsg - OUTPUT_MSG_BUZZ_KEY +1);
		   	break;
		   case OUTPUT_MSG_SPEED:
		     if(outputMsg->paramType == MSG_PARAM_USHORT)	   	
				 {	
				 	dataTmp = *((uint16_t*)outputMsg->outputMsgParam);
					 SetFanSpeed(dataTmp);
					if(mode >= MODE_TEST)
					{
						dataTmp/=10;
						if( mode >= TEST_LED_SPDLOW)
						SetTestSpdValue((uint8_t)dataTmp);
					}
					 
				 }else		
				 {
                ;		
				 }
		   	break;
		   case OUTPUT_MSG_RGB:
				 dataTmp = *((uint8_t*)outputMsg->outputMsgParam);
				 outputVariable.aqiLevel = (uint8_t)dataTmp;
//			if(outputVariable.light == LIGHT_BRIGHT)
//				memcpy(&rgbLightValue,&rgbValueRef[outputVariable.aqiLevel+1],(sizeof(_sRGBLIGHT)-2));
//			else
//				memcpy(&rgbLightValue,&rgbValueRefDark[outputVariable.aqiLevel+1],(sizeof(_sRGBLIGHT)-2));
				
		   	break;
			 case OUTPUT_MSG_TIMING:				 
				 SetTimgLed((*(uint8_t*)outputMsg->outputMsgParam));
				 break;
				 case OUTPUT_MSG_NET:
				 //	if((*(uint8_t*)outputMsg->outputMsgParam) == 0)
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
BuzzerLedTimer();
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
static void SetFanSpeed(uint16_t spd)
{
	uint16_t period,freq;
	float tmp1;
	freq = spd>>1;// 15 pulse per round
	if(freq == 0)
	{
	MotorPowerCtrl(OFF);
    period = FAN_PERIOD_MAXCOUNT;
	}
	else
	{
	MotorPowerCtrl(ON);
	tmp1 = ((FAN_PERIOD_COUNT*1.0)/freq);
	period = (uint16_t)tmp1;//((FAN_PERIOD_MAXCOUNT)/freq);
	}
	ChangeFanPeriod(period);
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
	case TEST_LED_SOFT:
		break;
	case 	TEST_LED_WKT:
		break;
	case TEST_LED_FILT:
		break;	
	case TEST_LED_SEN:
		break;
	case TEST_LED_KEY:
		break;
	case TEST_LED_PARAM:
	break;		
	case TEST_LED_SPDLOW:
		LedOffAll();
		break;
	case TEST_LED_SPDHIGH:
		break;
	case TEST_LED_SPDALOW:
		break;
	case TEST_LED_SPDAMED:
		break;
	case TEST_LED_SPDAHIGH:
		break;
	case TEST_LED_SPDFAST:
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
	memcpy(&rgbLightValue,&rgbValueRef[1],sizeof(_sRGBLIGHT));
}




/****************
* Function Name:      SetModeLed
* Description:        change gpio to set mode led state
                      
* Parameter:          none  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
uint8_t 	ledStandbyFlag = 1;
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
		SetAllLedPowerOn();	
	else	
		SetAllLedPowerOff();	
}

/****************
* Function Name:      SetBuzzer
* Description:        set the way of buzzer ring 
                      
* Parameter:           op : enum of buzzer type 
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
static void SetBuzzer(uint8_t buztype)
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
			if(buzCnt == 0)
			buzTypeTmp = outputVariable.buzType;
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
			if(buzCnt == 35)
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
    if(mode<MODE_TEST)
		{
		switch(outputVariable.wifiLedType)
		{
		case WIFILED_UCON:
           if(ledCnt == 0)
		   	WIFI_LED_ON();
		   if(ledCnt == 5)
		   	WIFI_LED_OFF();
			ledCnt++;
			if(ledCnt == 50)
				ledCnt = 0;
			break;
		case WIFILED_ROUTER:
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
		switch(outputVariable.filter)
		{
			case FILTER_LED_NORMAL:
				FILTER_LED_OFF();
				break;
			case FILTER_LED_CLR:
				if(filterLedCnt == 0)
					FILTER_LED_ON();
				else if(filterLedCnt == 5)
					FILTER_LED_OFF();
				if(filterLedCnt >= 10)
				filterLedCnt++;				
				break;
			case FILTER_LED_WARN:
				if(filterLedCnt == 0)
					FILTER_LED_ON();
				else if(filterLedCnt == 10)
					FILTER_LED_OFF();
				if(filterLedCnt >= 20)
				filterLedCnt++;	
				break;




		default:
			break;}
		
	}
		RgbLightFade();
}
	
	
}


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
		tmp <<=7;
	if(currentCC != tmp)
	{
		if(currentCC>tmp)
		{
			tmp2 = RGBFadeStep[j][i];
			tmp2>>=2;
			if(currentCC<tmp2)
				currentCC = tmp;
			else
			currentCC -= tmp2;
			if(currentCC<=(tmp))
				currentCC = (tmp);
		}else 
		{
			tmp2 = RGBFadeStep[j][i];
			tmp2>>=2;
			if(currentCC>(0xffff - tmp2))
				currentCC = (tmp);
			else
			currentCC += tmp2;
			if(currentCC>=tmp)
				currentCC = tmp;
		}
		*((uint32_t*)RGBLightRegister[i])= currentCC;
	}
	}
		if(ledStandbyFlag == 1)
	{
		tmp = *(rgbData+i);
		tmp <<=7;
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
		}
	
	}
}


void SetTestSpdType(uint8_t tstspdtype)
{
	switch(tstspdtype)
	{
		case TEST_LED_SPDLOW:
			POWER_LED_OFF();
			WIFI_LED_OFF();
			rgbLightValue.RGB_BCompare = 0x7f;
			break;
		case TEST_LED_SPDMED:
			POWER_LED_OFF();
			WIFI_LED_ON();
			rgbLightValue.RGB_BCompare = 0x00;
			break;
		case TEST_LED_SPDHIGH:
			POWER_LED_OFF();
			WIFI_LED_ON();
			rgbLightValue.RGB_BCompare = 0x7f;
			break;
		case TEST_LED_SPDALOW:
			POWER_LED_ON();
			WIFI_LED_OFF();
			rgbLightValue.RGB_BCompare = 0x00;
			break;
		case TEST_LED_SPDAMED:
			POWER_LED_ON();
			WIFI_LED_OFF();
			rgbLightValue.RGB_BCompare = 0x7f;
			break;
		case TEST_LED_SPDAHIGH:
			POWER_LED_ON();
			WIFI_LED_ON();
			rgbLightValue.RGB_BCompare = 0x00;
			break;
		default:
	break;
	}

}


void SetTestSpdValue(uint8_t spd)
{
	if(spd&0x80)
		AUTO_LED_ON();
	else
		AUTO_LED_OFF();
	if(spd&0x40)
		FAST_LED_ON();
	else
		FAST_LED_OFF();
	if(spd&0x20)
		LOW_LED_ON();		
	else
		LOW_LED_OFF();	

	if(spd&0x10)
		MEDIUM_LED_ON();		
	else
		MEDIUM_LED_OFF();	
	if(spd&0x08)
		HIGH_LED_ON();		
	else
		HIGH_LED_OFF();	
	if(spd&0x04)
		TIM1_LED_ON();		
	else
		TIM1_LED_OFF();	
	if(spd&0x02)
		TIM2_LED_ON();		
	else
		TIM2_LED_OFF();	
	if(spd&0x01)
		TIM3_LED_ON();		
	else
		TIM3_LED_OFF();	
}







