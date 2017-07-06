#include "device_outputTask.h"
#include "bspOutput.h"
/******Functions Declare **********/
extern void SetLed1On(void);
extern void SetLed1Off(void);
extern void ChangeFanPeriod(uint16_t freq);
extern void LoopTimerInit(_sLOOPTIMER * timer, uint16_t interval);
extern uint8_t CheckTickExpired(_sLOOPTIMER * timer);
extern void EnableRGBLEDLight(uint8_t en);
extern uint16_t GetRGBCurrentCompare_R(void);
extern uint16_t GetRGBCurrentCompare_G(void);
extern uint16_t GetRGBCurrentCompare_B(void);
extern uint16_t GetLightCurrentCompare(void);


void LedOffAll(void);
void OutputVariablesInit(void);
static void SetFanSpeed(uint16_t spd);
static void SetMode(_eMODE mode);
static void SetLightBrightness(uint16_t brightness);
void SetBlueLed(uint8_t op);
static void SetBuzzer(uint8_t op);
void BuzzerLedTimer(void);
void RgbLightFade(void);
/*********Variables*****/
extern xTaskHandle deviceOutputTask;
xQueueHandle outputMsgQueue;
_sOUTPUT_MSG* outputMsg;
_sOUTPUT_VAR outputVariable;
_sLOOPTIMER* buzLedTimer;
extern _sRGBLIGHT rgbLightValue;

/****************
* Function Name:      DeviceOutputTask
* Description:        hardware interface control setting  
                      
* Parameter:          arg  
* return:             none
* Date:               20170502
*author:              CTK  luxq
********************************/
void DeviceOutputTask(void* arg)
{
   uint8_t mode;
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
				SetMode((_eMODE)mode);
			}
		   	break;           
			 case OUTPUT_MSG_LIGHT:
		   	if(outputMsg->paramType == MSG_PARAM_INT)
		   	{
           SetLightBrightness(*((uint16_t*)outputMsg->outputMsgParam));
			  }else      
			 {
               ;// can not return  task is not allow return 
		     
			 }
		   	break;
		   case OUTPUT_MSG_BLUELED:
		   	if(outputMsg->paramType == MSG_PARAM_UCHAR)
		   	{
				SetBlueLed(*((uint8_t*)outputMsg->outputMsgParam));

			}else
			{
              ;
			}
		   	break;
		   case OUTPUT_MSG_BUZZ_KEY:
		   case OUTPUT_MSG_BUZZ_CONF:
		   case OUTPUT_MSG_BUZZ_WARN:
				 SetBuzzer(outputMsg->outputMsg - OUTPUT_MSG_BUZZ_KEY +1);
		   	break;
		   case OUTPUT_MSG_SPEED:
		     if(outputMsg->paramType == MSG_PARAM_USHORT)	   	
				 {			
					 SetFanSpeed(*((uint16_t*)outputMsg->outputMsgParam));
				 }else		
				 {
                ;		
				 }
		   	break;
		   case OUTPUT_MSG_RGB:
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
static void SetModeLed(_eMODE mode)
{
  LedOffAll();
  switch(mode)
  {
  case MODE_STANDBY:
  	POWER_LED_OFF();
  	break;
  case MODE_AUTO:
  	AUTO_LED_ON();
	POWER_LED_ON();
  	break;
  case MODE_JET:
  	FAST_LED_ON();
  	break;
  case MODE_LOW:
  	LOW_LED_ON();
  	break;
  case MODE_MEDIUM:
  	MEDIUM_LED_ON();
    break;
  case MODE_HIGH:
  	HIGH_LED_ON();
  	break;
  default:
  	break;
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
static void SetMode(_eMODE mode)
{  
	SetModeLed(mode); 
//	SetFanSpeed(mode);
	if(mode == MODE_STANDBY)
		EnableRGBLEDLight(DISABLE);
	else
		EnableRGBLEDLight(ENABLE);
	rgbLightValue.LuminCompare = 12000;// for test
	
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
  SetLed1On();
	else
	SetLed1Off();
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
	if(CheckTickExpired(buzLedTimer))
	{
		switch(outputVariable.buzType)
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
			if(buzCnt == 50)
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
		   if(ledCnt == 10)
		   	ledCnt = 0;
			break;
		default:
			break;
		}
		RgbLightFade();
	}
	
	
}


const uint32_t RGBLightRegister[4]={(TIM3_BASE+0x34),(TIM3_BASE+0x38),(TIM3_BASE+0x3C),(TIM3_BASE+0x40)};
void RgbLightFade(void)
{
	uint16_t currentCC;
	uint16_t* rgbData;
	uint8_t i = 0;
	rgbData = &(rgbLightValue.RGB_RCompare);
	for(i=0;i<4;i++)
	{
		currentCC = (*((uint32_t*)RGBLightRegister[i]))&0x0000ffff;
	if(currentCC != *(rgbData+i))
	{
		if(currentCC>*(rgbData+i))
		{
			currentCC -= 100;
			if(currentCC<*(rgbData+i))
				currentCC = *(rgbData+i);
		}else 
		{
			currentCC += 100;
			if(currentCC>*(rgbData+i))
				currentCC = *(rgbData+i);
		}
		*((uint32_t*)RGBLightRegister[i])= currentCC;
	}
	}
}




