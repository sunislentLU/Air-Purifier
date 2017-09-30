#include "device_inputTask.h"
#include "stdio.h"
#include "string.h"
#include "bspInput.h"
/*****************Functions*************************/
extern uint16_t GetFanPeriod(void);
extern uint8_t GetTempHumValue(uint16_t* temp,uint16_t* hum);
extern void I2CSensorGenerateStop(void);
void InputVariableInit(void);

 void KeyProcess(void);
static void TopCoverProcess(void);
static void BubbleSort(uint16_t*buffer,uint8_t length);
//void DustBufferSort(uint16_t* buffer,uint8_t length);
void GasDetection(void);
void EnvironmentValueScanProcess(void);
void LoopTimerInit(_sLOOPTIMER* timer,uint16_t interval);
uint8_t CheckTickExpired(_sLOOPTIMER* timer);
/*****************Variables********************************/
_sINPUTVALUE* inputValue;// input task global variables struct 
_sINPUT_MSG* inputMsg;// input messages struct 
QueueHandle_t inputMsgQueue;// message queue use to send message to maintask from inputtask
uint16_t gasValueBuf[INPUT_GAS_BUF_SIZE];
uint16_t luminValueBuf[INPUT_LUMI_BUF_SIZE];
uint16_t speedValueBuf[INPUT_SPEED_BUF_SIZE];

#ifndef FOR_JP
static uint8_t dustSampleCnt = 0;
static uint16_t dustSum = 0;
static uint16_t dustSubSum = 0;
static void DustDensityProcess(void);
#endif


_sLOOPTIMER* ms100Loop;
_sLOOPTIMER* ms200Loop;

extern xTaskHandle deviceInputTask;
//extern uint8_t dustEnable;


const _eINPUTMSG_TYPE key2Msgtab[][3]={{INPUT_MSG_KEY1_PRESS,INPUT_MSG_KEY1_LPRESS,INPUT_MSG_KEY1_HOLD},
	                                   {INPUT_MSG_KEY2_PRESS,INPUT_MSG_KEY2_LPRESS,INPUT_MSG_KEY2_HOLD},
							           {INPUT_MSG_KEY3_PRESS,INPUT_MSG_KEY3_LPRESS,INPUT_MSG_KEY3_HOLD},
							           {INPUT_MSG_KEY4_PRESS,INPUT_MSG_KEY4_LPRESS,INPUT_MSG_KEY4_HOLD},
									   {INPUT_MSG_KEY5_PRESS,INPUT_MSG_KEY5_LPRESS,INPUT_MSG_KEY5_HOLD}};
/****************

* Function Name:      DeviceInputTask
* Description:        all input device such as gas dust sensor  keys scan
                      and analyze process
* Parameter:          arg  -- not use
* return:             none
* Date:               20170426
*author:              CTK  luxq

***************/
void DeviceInputTask(void* arg)
{
 //InputVariableInit();// initialize gpio adc uart 
 InputHardwareInit();// alloc or malloc queue varible point and so on 

for(;;)
{
 //KeyProcess();
 TopCoverProcess();
 EnvironmentValueScanProcess();
 vTaskDelay(10);// 10ms to task yield
}

}




void InputVariableInit(void)
{
inputValue = pvPortMalloc(sizeof(_sINPUTVALUE));
inputMsg = pvPortMalloc(sizeof(_sINPUT_MSG));
inputMsgQueue = xQueueCreate(INPUT_QUEUE_LEN,sizeof(_sINPUT_MSG));
//inputDustSem = xSemaphoreCreateBinary();

  ms100Loop = pvPortMalloc(sizeof(_sLOOPTIMER));
	ms200Loop = pvPortMalloc(sizeof(_sLOOPTIMER));
	LoopTimerInit(ms100Loop,100);
	LoopTimerInit(ms200Loop,200);
memset(gasValueBuf,0,INPUT_GAS_BUF_SIZE);
memset(luminValueBuf,0,INPUT_LUMI_BUF_SIZE);
memset(speedValueBuf,0,INPUT_SPEED_BUF_SIZE);
}

/****************
* Function Name:      KeyProcess
* Description:        key scan and change key value to message send to main task
                      5ms loop (recommend maybe create a softtimer)
* Parameter:          none
* return:             none
* Date:               20170427
*author:              CTK  luxq
***************/
 void KeyProcess(void)
{
uint16_t keyValue;
uint8_t keyType;
uint8_t keyNumber;
BaseType_t xHigherPriorityTaskWoken;

keyValue = KeyScan();
keyNumber = (uint8_t)keyValue;
keyType = (uint8_t)(keyValue>>8);
if((keyNumber <=INPUT_KEY_NUM)&&(keyType<= INPUT_KEY_TYPE_MAX)&&(keyNumber != 0))
{
   inputMsg->inputMsg = key2Msgtab[keyNumber - 1][keyType - 1];
   if(inputMsgQueue != NULL)
   xQueueSendFromISR(inputMsgQueue,inputMsg,&xHigherPriorityTaskWoken);
}
}

static void TopCoverProcess(void)
{
    uint8_t coverState;
	coverState = SwitchScan();
	if(coverState!= inputValue->coverState)
	{
    inputValue->coverState = (_eCOVER_STATE)coverState;
		inputMsg->inputMsg = INPUT_MSG_TOP;
		inputMsg->paramType = MSG_PARAM_UCHAR;
		inputMsg->inputMsgParam = &inputValue->coverState;
		xQueueSend(inputMsgQueue,inputMsg,0);
	}

}





static void SpeedScan(void)
{
  uint16_t speedValue;
	static uint8_t speedCnt = 0;
	static uint16_t* pBuff = speedValueBuf;
		speedValue = GetFanFreq();
		*pBuff = speedValue;
		speedCnt++;
		pBuff++;
		if(speedCnt >= INPUT_SPEED_BUF_SIZE)
		{           
			speedCnt = 0;
			pBuff = speedValueBuf;
			BubbleSort(pBuff,INPUT_SPEED_BUF_SIZE);
			inputValue->speed= GetAverPayloadFromBuffer(speedValueBuf,INPUT_SPEED_BUF_SIZE,INPUT_SPEED_IGNORE_SIZE);
			memset(pBuff,0x00,INPUT_SPEED_BUF_SIZE*2);
			inputMsg->inputMsg = INPUT_MSG_SPEED;
			inputMsg->inputMsgParam = (void*)&(inputValue->speed);
			inputMsg->paramType = MSG_PARAM_USHORT;
			xQueueSend(inputMsgQueue, inputMsg, 0);
		}
}



/****************

* Function Name:      GasDetection
* Description:        gas value detect and send the message to main task
                      
* Parameter:          none 
* return:             none
* Date:               20170502
*author:              CTK  luxq

***************/
void GasDetection(void)
{
	uint16_t gasValue;   
	static uint8_t gasCnt = 0;
	static uint16_t* pBuff= gasValueBuf;
		gasValue = GetGasAdcValue();
		*pBuff = gasValue;
		pBuff++;	
		gasCnt++;
       if(gasCnt >= INPUT_GAS_BUF_SIZE)
       {
	   	   gasCnt = 0;	   
				 pBuff = gasValueBuf;
				 BubbleSort(pBuff,INPUT_GAS_BUF_SIZE);
	       inputValue->gasValue= GetAverPayloadFromBuffer(gasValueBuf, INPUT_GAS_BUF_SIZE, INPUT_GAS_IGNORE_SIZE);		   
				 memset(pBuff,0,INPUT_GAS_BUF_SIZE*2);		   
				 inputMsg->inputMsg = INPUT_MSG_GAS;	   
				 inputMsg->inputMsgParam = (void*)(&(inputValue->gasValue));		   
				 inputMsg->paramType = MSG_PARAM_SHORT;		   
				 xQueueSend(inputMsgQueue, inputMsg, 0);
       }
}


void LuminaceDetection(void)
{
	uint16_t lumiValue;
	static uint8_t lumiCnt = 0;
	static uint16_t* pBuff = luminValueBuf;
		lumiValue = GetLumiAdcValue();
		*pBuff = lumiValue;
		pBuff++;
		lumiCnt ++;      
		if(lumiCnt >= INPUT_LUMI_BUF_SIZE)     
		{
			 lumiCnt = 0;
			 pBuff = luminValueBuf;
			 BubbleSort(pBuff,INPUT_LUMI_BUF_SIZE);
			 inputValue->lumin = GetAverPayloadFromBuffer(luminValueBuf,INPUT_LUMI_BUF_SIZE,INPUT_LUMI_IGNORE_SIZE);
			 memset(pBuff,0,INPUT_LUMI_BUF_SIZE*2);
		   inputMsg->inputMsg = INPUT_MSG_LUMIN;
		   inputMsg->inputMsgParam = (void*)(&inputValue->lumin);
		   inputMsg->paramType = MSG_PARAM_SHORT;
		   xQueueSend(inputMsgQueue, inputMsg, 0);
		}
}


#ifndef FOR_JP
void TempHumiScan(void)
{
	uint16_t humtmp,temptmp;
	uint32_t tmp;
	
	if(GetTempHumValue(&temptmp,&humtmp))	
	{
		I2CSensorGenerateStop();
		inputMsg->inputMsg = INPUT_MSG_HTFAULT;						
		inputMsg->paramType = MSG_PARAM_NONE;
		xQueueSend(inputMsgQueue, inputMsg, 0);
	}
	else{
		inputMsg->inputMsg = INPUT_MSG_TEMP;
	    tmp = temptmp;
		tmp*=165;
		tmp>>=16;
		tmp -= 40;
		inputValue->temp = tmp;//((tmp*165)/65536) - 40;
		inputMsg->inputMsgParam = (void*)(&inputValue->temp);
		inputMsg->paramType = MSG_PARAM_SHORT;
		xQueueSend(inputMsgQueue, inputMsg, 0);
        tmp = humtmp;
		tmp *=100;
		tmp>>=16;
		inputValue->humi = tmp;//(tmp*100)/65536;
		inputMsg->inputMsg = INPUT_MSG_HUMI;
		inputMsg->inputMsgParam = (void*)(&inputValue->humi);
		inputMsg->paramType = MSG_PARAM_SHORT;
		xQueueSend(inputMsgQueue, inputMsg, 0);
	}
}

/****************

* Function Name:      DustDensityProcess
* Description:        get dust raw data and sort caculate the average of payload 
                      
* Parameter:          none
* return:             none
* Date:               20170428
*author:              CTK  luxq

***************/
extern uint8_t GetParticalDensity(uint16_t*pm25,uint16_t* pm100);
static void DustDensityProcess(void)
{
  uint16_t dustValue = 0;
	uint16_t dustSubValue = 0;
	if(GetParticalDensity(&dustValue,&dustSubValue) == 0)
	{
		dustSum+= dustValue;
		dustSubSum += dustSubValue;
		dustSampleCnt++;
		if(dustSampleCnt >= DUST_SMAPLE_CNT)
		{
		  dustSampleCnt = 0;
		  inputValue->dustValue = dustSum/DUST_SMAPLE_CNT;
		  inputMsg->inputMsg = INPUT_MSG_DUST;			
		  inputMsg->inputMsgParam = (void*)&(inputValue->dustValue);			
		  inputMsg->paramType = MSG_PARAM_SHORT;
		  xQueueSend(inputMsgQueue, inputMsg, 0);
		  inputValue->dustSubValue = dustSubSum/DUST_SMAPLE_CNT;
		  inputMsg->inputMsg = INPUT_MSG_DUST_SUB;			
		  inputMsg->inputMsgParam = (void*)&(inputValue->dustSubValue);			
		  inputMsg->paramType = MSG_PARAM_SHORT;
		  xQueueSend(inputMsgQueue, inputMsg, 0);
		  dustSum = 0;
		  dustSubSum = 0;
		}
	}else
	{
		I2CSensorGenerateStop();
		inputMsg->inputMsg = INPUT_MSG_DFAULT;						
		inputMsg->paramType = MSG_PARAM_NONE;
		xQueueSend(inputMsgQueue, inputMsg, 0);
	}
}
#endif
void EnvironmentValueScanProcess(void)
{
	if(CheckTickExpired(ms100Loop) == 1)
	{
		LuminaceDetection();
		SpeedScan();
	}
	if(CheckTickExpired(ms200Loop) == 1)
	{
		GasDetection();
#ifndef FOR_JP
	  DustDensityProcess();
	  TempHumiScan();
#endif
	}
}

/****************

* Function Name:      BubbleSort
* Description:        bubble sort min top max down 
                      
* Parameter:          buffer->datas need sort  ;length: the number need sort
* return:             none
* Date:               20170428
*author:              CTK  luxq

***************/
static void BubbleSort(uint16_t*buffer,uint8_t length)
{
	uint8_t low = 0;
	uint8_t high = length;
	uint16_t tmp,j;
	while(low < high)
	{
        for(j=low;j<high-1;j++)
        {		
					if(buffer[j]>buffer[j+1])				
					{					
						tmp = buffer[j];                  
						buffer[j] = buffer[j+1];					
						buffer[j+1] = tmp; 
			     }
		}
		--high;
		for(j = high;j>low;j--)
		{
			if(buffer[j]<buffer[j-1])
				{
					tmp = buffer[j];
					buffer[j] = buffer[j-1];
					buffer[j-1] = tmp;
				}
		}
		low++;
	}

}

/****************

* Function Name:      GetAverPayloadFromBuffer
* Description:        get the average value of payload in the buffer
                      
* Parameter:          buffer: data buffer;maxLen the buffer length;ignoreLen ignore byte in buffer max and min
* return:             the average value
* Date:               20170428
*author:              CTK  luxq

***************/
uint16_t GetAverPayloadFromBuffer(uint16_t* buffer,uint8_t maxLen,uint8_t ignoreLen)
{     
	 uint16_t* pBuff;
	 uint8_t i=0;
	 uint32_t sum = 0;
	 uint16_t aver;
	 uint8_t len;
	 len = maxLen - ignoreLen;
	 pBuff = buffer+(ignoreLen/2);
	 for(i=0;i<len;i++)
	 {         
		 sum+=*pBuff;
		 pBuff++;
	 }
	 aver = sum/len; 
	 return aver;
}

/****************

* Function Name:      CheckTickExpired
* Description:        check loop timer expired 
                      
* Parameter:          timer
* return:             the jude result
* Date:               20170502
*author:              CTK  luxq

***************/
uint8_t CheckTickExpired(_sLOOPTIMER* timer)
{
	uint8_t ret = 0;
	uint32_t currentTick;
	portENTER_CRITICAL();
	currentTick = xTaskGetTickCount();
	portEXIT_CRITICAL();

   if(currentTick>=timer->nextTick)//正常情况下时间到了
	 {
	 	
		if(timer->nextTick>=timer->intervalTick)//正常情况下
		{
			timer->nextTick =currentTick+ timer->intervalTick;
			ret = 1;
		}else//下一个定时小于周期值说明大于0xffffffff循环了
		{
			if(currentTick<timer->intervalTick)//如果这个值小于一个周期说明定时时间到
			{
				timer->nextTick =currentTick+ timer->intervalTick;
				ret = 1;
			}else 
			ret = 0;
		}			
		if(timer->nextTick == currentTick)
			{							
				timer->nextTick =currentTick+ timer->intervalTick;			
				ret = 1;			
			}
	}else
	 {
		 if((timer->nextTick - currentTick > timer->intervalTick))//
		 { 					
			 timer->nextTick = currentTick+timer->intervalTick;
			 ret = 1;
		 }
	 
	 }
	 return ret;

}

/****************

* Function Name:      LoopTimerInit
* Description:        initialize loop timer 
                      
* Parameter:          timerand interval time
* return:             none
* Date:               20170502
*author:              CTK  luxq
***************/
void LoopTimerInit(_sLOOPTIMER* timer,uint16_t interval)
{
	timer->nextTick = xTaskGetTickCount()+interval;
	timer->intervalTick = interval;
}






