#include "wifiTask.h"
#include "fifo.h"
#include "wifiProtocol.h"
#include "bspWifi.h"
#include "stdio.h"
#include "string.h"

extern xSemaphoreHandle semWifiVariable;
void WifiVariablesInit(void);
uint8_t CheckTableLenght(uint8_t* table);
extern void WifiHardwareInit(void);
void FetchNullData(void);
uint8_t GetChecksum(uint8_t cmd);
extern unsigned char  ParseWifiDatas(_sWIFI_FORMAT* wifiData);
extern void SendCmd2WifiModule(uint8_t cmd,const _sWIFI_CMD_PROC cmdarray[],\
	                                            unsigned char* databuff,_sFIFO* sendfifo);
extern uint8_t CheckTickExpired(_sLOOPTIMER* timer);
extern void LoopTimerInit(const _sLOOPTIMER* timer,uint16_t interval);
void CheckFifoSendFirstData(_sFIFO* fifo);

extern uint8_t* GetModeState(void);
extern uint8_t* GetDustValue(void);
extern uint8_t* GetDustSubValue(void);
extern uint8_t* GetAqiValue(void);
extern uint8_t* GetTimingValue(void);
extern uint8_t* GetFilterRemain(void);
extern uint8_t* GetLedState(void);
extern uint8_t* GetDevFault(void);
extern uint8_t* GetTemp(void);
extern uint8_t* GetHumi(void);
extern uint8_t* GetFirmwareVersion(void);
extern uint8_t* GetCoverState(void);
extern uint8_t* GetTVOCValue(void);
extern uint8_t* GetLuminValue(void);
extern uint8_t* GetFanSpeed(void);
extern uint8_t* GetFanSpdRef(void);
extern uint8_t* GetDustRef(void);
extern uint8_t* GetAutoSpdRef(void);
extern uint8_t* GetLuminRef(void);
extern uint8_t* GetDustSen(void);
extern uint8_t* GetTVOCRef(void);
extern uint16_t GetFirmVersion(void);



extern xTaskHandle wifiTask;


void FetchPidPkey(void);
void WifiRecRespond(void);
void TermDataStruct(uint8_t term_id,uint8_t* buffer);
void TransmitTermData(uint8_t term_id,const _sTERMI_FORMAT* datacmd,uint8_t* sendbuffer,_sFIFO* sendfifo);
void TransmitAllTermData(const _sTERMI_FORMAT* datacmd,uint8_t* sendbuffer,_sFIFO* sendfifo,uint8_t sendType);
void ClearReceiveBuffer(void);
void CheckFifoSendFirstData(_sFIFO* fifo);
void GetUpdateStatus(void);

xQueueHandle wifiRecQueue;
xQueueHandle wifiSndQueue;
xSemaphoreHandle semRecComplete;
xSemaphoreHandle semSendEnable;
_sWIFI_SND_MSG* wifiSndMsg;
_sWIFI_REC_MSG* wifiRecMsg;
_sWIFI_FORMAT* wifiFrame;
uint8_t* uartDataBuf;
uint8_t uartSendBuf[WIFI_MAX_SEND_BUFF];
_sFIFO* uartSendFifo;
uint8_t netStatus = 0;
uint8_t netStatus_tmp = 0;
_eWIFI_PROCESS wifiState = WIFI_START;
_sLOOPTIMER* wifiLedLoop;

const _sTERMI_FORMAT termination_info[]={
{0x01,MSG_PARAM_UCHAR,GetModeState},// mode
{0x02,MSG_PARAM_SHORT,GetDustValue}, // pm2.5
{0x02,MSG_PARAM_SHORT,GetDustSubValue}, // pm10
{0x01,MSG_PARAM_UCHAR,GetAqiValue}, // aqi value
{0x01,MSG_PARAM_UCHAR,GetTimingValue},// timing value
{0x01,MSG_PARAM_UCHAR,GetFilterRemain}, //filter remain persent
{0x01,MSG_PARAM_UCHAR,GetLedState},     // led state
{0x02,MSG_PARAM_USHORT,GetDevFault},    //deveice fault warning
{0x02,MSG_PARAM_SHORT,GetTemp},         // device enviroment temperature
{0x01,MSG_PARAM_UCHAR,GetHumi},         // device humidity
{0x02,MSG_PARAM_USHORT,GetFirmwareVersion},// firmware version
{0x01,MSG_PARAM_UCHAR,GetCoverState},   //cover state
{0x02,MSG_PARAM_USHORT,GetTVOCValue},    //tvoc value of resistor
{0x02,MSG_PARAM_SHORT,GetLuminValue},   // luminace value voltage
{0x02,MSG_PARAM_UCHAR,GetFanSpeed},     // current speed
{0x0a,MSG_PARAM_SHORT,GetFanSpdRef},    // mode speed reference 
{0x06,MSG_PARAM_SHORT,GetDustRef},      //pm2.5 level reference 
{0x06,MSG_PARAM_SHORT,GetAutoSpdRef},   //auto mode speed reference 
{0x04,MSG_PARAM_SHORT,GetLuminRef},     // lumin level reference
{0x02,MSG_PARAM_SHORT,GetDustSen},     //
{0x06,MSG_PARAM_SHORT,GetTVOCRef}
};
                                    
const _sWIFI_CMD_PROC sendCmdArray[]={
//length-----  cmd---fetch datas--return chksum
{0x00,0x02,CMD_GET_MAC,FetchNullData,GetChecksum},                                      
{0x00,0x02,CMD_GET_NET,FetchNullData,GetChecksum},
{0x00,0x02,CMD_GET_PID_KEY,FetchNullData,GetChecksum},        				  
{0x00,0x42,CMD_SET_PID_KEY,FetchPidPkey,GetChecksum},                                      
{0x00,0x02,CMD_SET_CONNECT,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_REBOOT_WIFI,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_RESET_WIFI,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_GET_FIREWARE,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_GET_TIME,FetchNullData,GetChecksum},							  
{0x00,0x02,CMD_SET_VISIBLE,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_REC_DATA,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_SND_DATA,FetchNullData,GetChecksum},							  
{0x00,0x02,CMD_REC_TERM_DATA,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_SND_TERM_DATA,FetchNullData,GetChecksum},								  
{0x00,0x02,CMD_SND_ALL_TERM_DATA,FetchNullData,GetChecksum},							  
{0x00,0x02,CMD_SND_SEV_TERM_DATA,FetchNullData,GetChecksum},							 
									                    };

const char productId[]={"160fa2b247a103e9160fa2b247a19201"};// xlink  product id
const char productKey[]={"dc5bfec839156d57015c802a504042bd"};// xlink product key





/****************
* Function Name:      WIFITask
* Description:        product register wifi message receive and send task
                      
* Parameter:          arg
* return:             none
* Date:               20170520
*author:              CTK  luxq
***************/
void WIFITask(void* arg)
{
	uint8_t command = 0;// just make compiler happy
	uint8_t chkNetCnt = 0;
	uint8_t* dataPointer;
    WifiHardwareInit();
   for(;;)
   {
	switch(wifiState)
	{
		case WIFI_START:
		  if(xSemaphoreTake(semRecComplete,2000))//2 second to check module quarity
			{
			if(ParseWifiDatas(wifiFrame))
			{
			  if(wifiFrame->cmd == CMD_GET_NET)
				{
				 if(wifiFrame->data[0] == 2)
				 {
				   wifiState = WIFI_CHK_NET;
					 break;
				 }
				}
			}
		}else //module is bad
			{
			wifiState = WIFI_CHK_NET;			
			}
			break;
    case WIFI_CHK_NET:
		SendCmd2WifiModule(CMD_GET_NET,sendCmdArray,uartSendBuf,uartSendFifo);
	  if(xSemaphoreTake(semRecComplete,1000))
	  {
       if(ParseWifiDatas(wifiFrame))
			 {
			 if(wifiFrame->cmd == CMD_GET_NET)
			 {
			 	netStatus = (wifiFrame->data[1]<<1)|wifiFrame->data[0];
				netStatus_tmp = netStatus;
				wifiRecMsg->wifiMsg = WIFI_MSG_NET;
				wifiRecMsg->length= 1;
				wifiRecMsg->wifiMsgParam = &netStatus;
				xQueueSend(wifiRecQueue,wifiRecMsg,0);
				if(netStatus == 1)
					wifiState = WIFI_VER_PID;
			 else if(netStatus == 3)// mcu reset but wifi not reset 
			 {
			  wifiState = WIFI_VER_PID;
				 break;
			 }
			 }else if(wifiFrame->data[0] == 0x02)// start up
			 {
				 
		   }
	    }
			 vTaskDelay(100);
		}else // 1000 ms no respond 
		{
       

		}
		if(xQueueReceive(wifiSndQueue, wifiSndMsg, 0))
		{
			command = wifiSndMsg->propMsg;
			switch(command)
			{           	
				case WIFI_SET_CONN:
					SendCmd2WifiModule(CMD_SET_CONNECT,sendCmdArray,uartSendBuf,uartSendFifo);
					if(xSemaphoreTake(semRecComplete, 1000))
					{ 
						if(ParseWifiDatas(wifiFrame))
						{
							if(wifiFrame->cmd == CMD_SET_CONNECT)	// respond right												
							{
							}
						}
					}					
				break;

				case WIFI_REBOOT:
					WIFI_RST_ENBALE();
					vTaskDelay(5);
					WIFI_RST_DISABLE();
				break;

			  default:
				break;
				}
		
		}
		
	break;
	case WIFI_VER_PID:
		SendCmd2WifiModule(CMD_GET_PID_KEY,sendCmdArray,uartSendBuf,uartSendFifo);
	  if(xSemaphoreTake(semRecComplete, 1000))
		{
			if(ParseWifiDatas(wifiFrame))
			{
				if(wifiFrame->cmd == CMD_GET_PID_KEY)
				{
		     if(strncmp((const char*)(&wifiFrame->data[0]),productId,32) == 0)
		     {
		       if(strncmp((const char*)(&wifiFrame->data[32]),productKey,32) == 0)
			     {
						 ClearReceiveBuffer();
						 SendCmd2WifiModule(CMD_GET_NET,sendCmdArray,uartSendBuf,uartSendFifo);
				     wifiState = WIFI_NORMAL_OP;
					 netStatus = 0x01;
					 netStatus_tmp = 0x01;
			       break;
			     }else
			     {
			       SendCmd2WifiModule(CMD_SET_PID_KEY,sendCmdArray,uartSendBuf,uartSendFifo);
			       if(xSemaphoreTake(semRecComplete, 1000))
				     {
				        if(ParseWifiDatas(wifiFrame))
								{
								   if(wifiFrame->cmd == CMD_SET_PID_KEY)
										 ClearReceiveBuffer();
										 break;
								}
				     }
			    }  
		    }else
		    {
		      SendCmd2WifiModule(CMD_SET_PID_KEY,sendCmdArray,uartSendBuf,uartSendFifo);
					if(xSemaphoreTake(semRecComplete, 100))
					{
					  if(ParseWifiDatas(wifiFrame))
					  {
						   if(wifiFrame->cmd == CMD_SET_PID_KEY)
						   break;
					  }
				  }
		    }
	    }else //not get pid and pkey respond
			{
				vTaskDelay(1000);
				
			}
		}
		
		}else// maybe the module is not good 
		{
		 
		}
		ClearReceiveBuffer();
	break;
	case WIFI_NORMAL_OP:
		if(xSemaphoreTake(semRecComplete, 100))
		{
           if(ParseWifiDatas(wifiFrame))
           {
              command = wifiFrame->cmd;  
						 switch(command)  
						 {                   
							 case CMD_GET_MAC: // receive mac address
				   	
							 break;                   
							 case CMD_GET_NET:
								 netStatus = (wifiFrame->data[1]<<1)|wifiFrame->data[0];					 
							   if((netStatus == NET_CLOUD)&&(netStatus_tmp != NET_CLOUD))//first connect to cloud					 
							     {
							   	    TransmitAllTermData(termination_info,uartSendBuf,uartSendFifo,CMD_SND_ALL_TERM_DATA);
					      	    if(xSemaphoreTake(semRecComplete, 1000))
									    { 
										
										    if(ParseWifiDatas(wifiFrame))
										    {
												if(wifiFrame->cmd == CMD_SND_ALL_TERM_DATA)	// respond right
												{
												TransmitAllTermData(termination_info,uartSendBuf,uartSendFifo,CMD_SND_SEV_TERM_DATA);
													//break;	
												}													
												else// respond error
												{

												}
										     }
											else// wifi frame error
											{


											}
									    
									   }
							     }					 
							  if((netStatus_tmp == NET_CLOUD)&&(netStatus != NET_CLOUD))// off line					 
							  {
					 
							  }else if(netStatus == NET_OFFLINE)// 				 
							  {
             				 
							  }													
								wifiRecMsg->wifiMsg = WIFI_MSG_NET;
								wifiRecMsg->length= 1;
								wifiRecMsg->wifiMsgParam =  &netStatus;
								xQueueSend(wifiRecQueue,wifiRecMsg,0);			
							  netStatus_tmp = netStatus;	
							  if(xSemaphoreTake(semWifiVariable,portMAX_DELAY))								
							  	ClearReceiveBuffer();								
							 break;			   
							 case CMD_GET_PID_KEY:				   	
							 break;                   					 
							 case CMD_SET_PID_KEY:
				   	
							 break;                   					 
							 case CMD_SET_CONNECT:
							 	
							 break;                   					 
							 case CMD_REBOOT_WIFI:
				   	
							 break;                   					 
							 case CMD_RESET_WIFI:
				   	
							 break;                   					 
							 case CMD_GET_FIREWARE:
				   	
							 break;                   					 
							 case CMD_GET_TIME:
				   	
							 break;                   					 
							 case CMD_SET_VISIBLE:
				   	
							 break;				   
							 case CMD_REC_DATA:
				   	   
							 break;                   					 
							 case CMD_SND_DATA:
				   	
							 break;                   					 
							 case CMD_REC_TERM_DATA:// 接收端点数据 强制只能接收一个端点数据
							 dataPointer = wifiFrame->data;
							 wifiRecMsg->wifiMsg = (_eWIFIMSG_TYPE)((*dataPointer) + WIFI_MSG_MODE);// index begin at 0 my command or element begin at 1
							 wifiRecMsg->length = *(dataPointer+2);
							 wifiRecMsg->wifiMsgParam = dataPointer+3;
							 xQueueSend(wifiRecQueue,wifiRecMsg,1);		
						   if(xSemaphoreTake(semWifiVariable,portMAX_DELAY))
						   	ClearReceiveBuffer();	
							 WifiRecRespond();
							 break;                   					 
							 case CMD_SND_TERM_DATA:
							 break;                   					 
							 case CMD_SND_ALL_TERM_DATA:
							 break;                   					 
							 case CMD_SND_SEV_TERM_DATA:
							 if(wifiFrame->data[0] == 0x01)//only 0x01 mean just upload to router
							 {								 
							 if(xSemaphoreTake(semRecComplete, 1000))
							 {
							  if(ParseWifiDatas(wifiFrame))
								{
								 if(wifiFrame->data[0] == 0x02)// data transmit to cloud
								 {
								 
								 }
								}
							 }
						   }
							 break;
							 case CMD_ASK_UPDATE:// 0x30 command 
							 dataPointer = wifiFrame->data;
							 if(*dataPointer == 0x01)//new state
							 {
							 	dataPointer++;
							 	wifiRecMsg->wifiMsg = WIFI_MSG_UPDATE;
							 	wifiRecMsg->length = 0x06;
								wifiRecMsg->wifiMsgParam = dataPointer;
								xQueueSend(wifiRecQueue,wifiRecMsg,0);
							 }

							 break;
						 }						 							 
					 }		
		}else
		{
			chkNetCnt++;
			if(chkNetCnt == 50)			    
			{					
				if(xSemaphoreTake(semSendEnable, 2))
						GetUpdateStatus();
			}
			if(chkNetCnt == 100)// two second to check net satus
			{
			chkNetCnt = 0;
		   if(xSemaphoreTake(semSendEnable, 2))
		   SendCmd2WifiModule(CMD_GET_NET,sendCmdArray,uartSendBuf,uartSendFifo);
			}
		}
		
		if(xQueueReceive(wifiSndQueue, wifiSndMsg, 0))
		{
			command = wifiSndMsg->propMsg;
			switch(command)
			{           
				case WIFI_UP_MODE:
				case WIFI_UP_DUST:
				case WIFI_UP_DUST_SUB:
				case WIFI_UP_GAS:
				case WIFI_UP_LED:
				case WIFI_UP_TIMING:
				case WIFI_UP_TEMP:
				case WIFI_UP_HUMI:
				case WIFI_UP_AQI:
					if(netStatus < 1)
						break;
					TransmitTermData(command - 1,termination_info,uartSendBuf,uartSendFifo);
					if(xSemaphoreTake(semRecComplete, 100))
						{
							if(ParseWifiDatas(wifiFrame))
								{
									if(wifiFrame->cmd == CMD_SND_TERM_DATA)
										break;
									}
							}
					break;

/*			  case WIFI_UP_DUST:
			  	TransmitTermData(TERM_DUST,termination_info,uartSendBuf,uartSendFifo);
				break;
				case WIFI_UP_DUST_SUB:

					TransmitTermData(TERM_DUST_SUB,termination_info,uartSendBuf,uartSendFifo);
					break;
			  case WIFI_UP_GAS:
			  	TransmitTermData(TERM_TVOC,termination_info,uartSendBuf,uartSendFifo);
			 	break;
			  case WIFI_UP_LED:
			  	TransmitTermData(TERM_LED,termination_info,uartSendBuf,uartSendFifo);
			 	break;
				case WIFI_UP_TIMING:
					TransmitTermData(TERM_TIMING,termination_info,uartSendBuf,uartSendFifo);
					break;
				case WIFI_UP_TEMP:
					TransmitTermData(TERM_TEMP,termination_info,uartSendBuf,uartSendFifo);
					break;
				case WIFI_UP_HUMI:
					TransmitTermData(TERM_HUMI,termination_info,uartSendBuf,uartSendFifo);
					break;
				case WIFI_UP_AQI:
					TransmitTermData(TERM_AQI,termination_info,uartSendBuf,uartSendFifo);
					break;*/
			  case WIFI_UP_LOCK:
			 	break;
			  case WIFI_UP_SPEED:
			 	break;
			  case WIFI_UP_VERSION:
			 	break;
			  case WIFI_UP_ALL:
					TransmitAllTermData(termination_info,uartSendBuf,uartSendFifo,CMD_SND_SEV_TERM_DATA);
				  if(xSemaphoreTake(semRecComplete, 1000))
					{ 
						if(ParseWifiDatas(wifiFrame))
						{
							if(wifiFrame->cmd == CMD_SND_SEV_TERM_DATA)	// respond right												
							{
							}
						}
					}
					break;
				case WIFI_SET_CONN:
					SendCmd2WifiModule(CMD_SET_CONNECT,sendCmdArray,uartSendBuf,uartSendFifo);
					if(xSemaphoreTake(semRecComplete, 1000))
					{ 
						if(ParseWifiDatas(wifiFrame))
						{
							if(wifiFrame->cmd == CMD_GET_PID_KEY)	// respond right												
							{
							}
						}
					}					
				break;

				case WIFI_REBOOT:
					WIFI_RST_ENBALE();
					vTaskDelay(5);
					WIFI_RST_DISABLE();					
				break;

			  default:
				break;
				}
		}
	break;
	default:
		break;
		} 
   }
}

/****************
* Function Name:      WifiVariablesInit
* Description:        task variables init and queue create semaphore create
                      
* Parameter:          none
* return:             none
* Date:               20170520
*author:              CTK  luxq
***************/
void WifiVariablesInit(void)
{
	wifiRecQueue = xQueueCreate(WIFI_REC_QUEUE_LEN, sizeof(_sWIFI_REC_MSG));
	wifiSndQueue = xQueueCreate(WIFI_SND_QUEUE_LEN, sizeof(_sWIFI_SND_MSG));
	semRecComplete = xSemaphoreCreateCounting(10,0);
	semSendEnable = xSemaphoreCreateBinary();
	wifiRecMsg = pvPortMalloc(sizeof(_sWIFI_REC_MSG));
	wifiSndMsg = pvPortMalloc(sizeof(_sWIFI_SND_MSG));
	wifiFrame = pvPortMalloc(sizeof(_sWIFI_FORMAT));
	uartSendFifo = pvPortMalloc(sizeof(_sFIFO));
	wifiLedLoop = pvPortMalloc(sizeof(_sLOOPTIMER));
	LoopTimerInit(wifiLedLoop,100);
}

/****************
* Function Name:      WifiFramInit
* Description:        wififrame init
                      
* Parameter:          none
* return:             none
* Date:               20170526
*author:              CTK  luxq
***************/
void WifiFramInit(_sWIFI_FORMAT* frame)
{
  frame->checksum = 0;
  frame->cmd = CMD_GET_NET;
  frame->end = 0;
  frame->length = 0;
  frame->head = 0;
  memset(frame->data,0,WIFI_MAX_BUF_LEN);
}


/****************
* Function Name:      USART2_IRQHandler
* Description:        wifi comunication uart receive and send handler
                      wifi frame receive and auto send process
                      
* Parameter:          none
* return:             none
* Date:               20170520
*author:              CTK  luxq
***************/
_eWIFI_UART_STATE uartState = WIFI_UART_HEAD;
 static uint8_t recCnt = 0;
void USART2_IRQHandler(void)
{
 uint8_t recData = 0;
 uint8_t sendData;
 BaseType_t pxHigherPriorityTaskWoken;
 if(USART_GetITStatus(USART2,USART_IT_RXNE))//receive a byte data
 {
   recData = USART_ReceiveData(USART2);
  switch(uartState)
  {
   case WIFI_UART_HEAD:
   	if(recData == HEAD)
   	{
       wifiFrame->head = recData;
	   uartState = WIFI_UART_LEN;
	}
   break; 
   case WIFI_UART_LEN:
	if((recCnt == 0)&&(recData == 0xff))// debug for long data receive 
		break;
   	recCnt++;
	if(recCnt==1)
	{  
		wifiFrame->length = recData;  
	}else
	{  
		wifiFrame->length<<=8;
		wifiFrame->length|=recData;
		recCnt = 0;
		uartState = WIFI_UART_CMD;
	}
   break;
   case WIFI_UART_CMD:  	
   wifiFrame->cmd = (_eCOMMAND)recData;
	 uartState = WIFI_UART_DATA;
	 recCnt = 0;
   break;
   case WIFI_UART_DATA:
	 if(recData == END)
	{
		uartState = WIFI_UART_END;
		wifiFrame->end = recData;
		wifiFrame->checksum = wifiFrame->data[recCnt -1];
		wifiFrame->data[recCnt -1] = 0;
		recCnt = 0;
  	xSemaphoreGiveFromISR(semRecComplete,0);
		uartState = WIFI_UART_HEAD;
		break;
	}
	if((recData == 0xfd)&&(wifiFrame->data[recCnt - 1]>=0x7d))
	{
		wifiFrame->data[recCnt-1] = wifiFrame->data[recCnt-1]+0x80;
		break;
	}
	 wifiFrame->data[recCnt] = recData;
	 recCnt++;
   break;
	default:
		break;
  }
 }
 if(USART_GetITStatus(WIFI_UART,USART_IT_TC))// send complete
 {
 	USART_ClearITPendingBit(WIFI_UART,USART_IT_TC);
  if(uartSendFifo->count != 0)
  {
  	if(!PopFromFifo(uartSendFifo,&sendData))
          USART_SendData(WIFI_UART,sendData);
	if((sendData == END)&&(uartSendFifo->count == 0))
		xSemaphoreGiveFromISR(semSendEnable,&pxHigherPriorityTaskWoken);
  }
 }
USART2->ICR = 0xffffffff;
USART2->ISR = 0;
}

/****************
* Function Name:      FetchNullData
* Description:        some wifi commands which  without any parameter 
                      fetch a blank parameter 
* Parameter:          none
* return:             none
* Date:               20170520
*author:              CTK  luxq
***************/
void FetchNullData(void)
{
 ;
}


uint8_t GetChecksum(uint8_t cmd)
{
unsigned char checksum = 0;
unsigned char i=0;
unsigned char* pointer;
pointer = uartSendBuf;
checksum^=sendCmdArray[cmd].cmd;
checksum^=sendCmdArray[cmd].length1;
checksum^=sendCmdArray[cmd].length2;
for(i=0;i<sendCmdArray[cmd].length1-2;i++)
{
 checksum ^=*pointer;
 pointer++;
}
return checksum;
}


/****************
* Function Name:      FetchPidPkey
* Description:        get xlink pid and key 
* Parameter:          none
* return:             none
* Date:               20170526
* author:             CTK  luxq
***************/
void FetchPidPkey(void)
{
	uint8_t* pointer;
 uint8_t i=0;
	pointer = uartSendBuf;
 for(i=0;i<32;i++)
	{
	 *pointer++=productId[i];
	}
	for(i=0;i<32;i++)
	{
	*pointer++=productKey[i];
	}
}


void FetchMode(uint8_t* buffer)
{
	TermDataStruct(TERM_MODE,buffer);	
}

void FetchDustDensity(uint8_t* buffer)
{
   TermDataStruct(TERM_DUST,buffer);	
}

void FetchGasAdcValue(uint8_t* buffer)
{
	TermDataStruct(TERM_TVOC,buffer);	
}


void FetchLightLiuminace(uint8_t* buffer)
{
   TermDataStruct(TERM_LUMI,buffer);	

}

void FetchBlueLedState(uint8_t* buffer)
{
    TermDataStruct(TERM_LED,buffer);
}

void FetchFanSpeed(uint8_t* buffer)
{
    TermDataStruct(TERM_SPEED,buffer);
}

void FetchCoverState(uint8_t* buffer)
{
   TermDataStruct(TERM_COVER,buffer);
}

void FetchFaultCode(uint8_t* buffer)
{
   TermDataStruct(TERM_FAULT,buffer);
}

void FetchTimingValue(uint8_t* buffer)
{
 TermDataStruct(TERM_TIMING,buffer);
}

void FetchFilterTime(uint8_t* buffer)
{
 TermDataStruct(TERM_FILTERLIVE,buffer);
}

void FetchSpeedRef(uint8_t* buffer)
{
 TermDataStruct(TERM_SPD_REF,buffer);
}

void FetchAutoSpeedRef(uint8_t* buffer)
{
 TermDataStruct(TERM_AUTO_SPD_REF,buffer);
}

void FetchDustRef(uint8_t* buffer)
{
	TermDataStruct(TERM_DUST_REF,buffer);
}

void FetchGasRef(uint8_t* buffer)
{
  TermDataStruct(TERM_TVOC_REF,buffer);
}

void FetchLightLumin(uint8_t* buffer)
{
  TermDataStruct(TERM_LUMI_REF,buffer);
}

void FetchFilterLiveRef(uint8_t* buffer)
{
  
}

void FetchDustSen(uint8_t* buffer)
{
 TermDataStruct(TERM_SENSE,buffer);
}


void FetchAllTerminations(void)
{

}


/****************
* Function Name:      TermDataStruct
* Description:        构造端点数据澹 根据原先设定好的端点参数
                      将端点的数据整合成符合协议要求的格式
                      
* Parameter:          term_id 端点的编号 索引 buffer: 整合到这个 buffer中
* return:             none
* Date:               20170526
*author:              CTK  luxq
***************/

void TermDataStruct(uint8_t term_id,uint8_t* buffer)
{
 uint8_t* pointer;
 uint8_t* dataPointer;
 uint16_t i = 0;
 pointer = buffer;
 dataPointer = termination_info[term_id].getdatafunc();
 *pointer++ = term_id;
 *pointer++ = termination_info[term_id].length/256;
 *pointer++ = termination_info[term_id].length%256;
 if((termination_info[term_id].datatype == MSG_PARAM_CHAR)||(termination_info[term_id].datatype == MSG_PARAM_UCHAR)) // byte
 {
  for(i=0;i<termination_info[term_id].length;i++)
  {
     *pointer++ = *dataPointer;
  }

 }else if((termination_info[term_id].datatype == MSG_PARAM_SHORT)||(termination_info[term_id].datatype == MSG_PARAM_USHORT))
  {
  for(i=0;i<termination_info[term_id].length;i++)
  {
    *pointer++ = *(dataPointer+1);
	  *pointer++ = *(dataPointer);
	  dataPointer+=2;
	}
  }
}

/****************
* Function Name:      TransmitTermData
* Description:        传输端点数据
                      
                      
* Parameter:          cmd 端点的编号 索引 buffer: 整合到这个 buffer中
* return:             none
* Date:               20170526
*author:              CTK  luxq
***************/
void TransmitTermData(uint8_t term_id,const _sTERMI_FORMAT* datacmd,uint8_t* sendbuffer,_sFIFO* sendfifo)
{ 
   uint8_t* dataPointer;
   uint8_t length = 0,length1,length2;
   uint8_t checksum = 0;
   uint8_t i;
   if(xSemaphoreTake(semSendEnable, 2))
   {
   length = datacmd[term_id].length + 3 +2;//  + cmd + index + checksum
   length1 = (uint8_t)(length/256);
   length2 = (uint8_t)(length%256);
	 memset(sendbuffer,0,128);
   dataPointer = sendbuffer;
   TermDataStruct(term_id,dataPointer);
   checksum ^= CMD_SND_TERM_DATA;
	 checksum ^= length1;
	 checksum ^= length2;
   PushInFifo(sendfifo,HEAD);
   PushInFifo(sendfifo,length1);
   PushInFifo(sendfifo, length2);
	 PushInFifo(sendfifo,CMD_SND_TERM_DATA);
	 length -= 2;
   for(i=0;i<length;i++)
   {
     checksum ^=*dataPointer;
     if(*dataPointer >= 0xFD)
     {
	 	PushInFifo(sendfifo,*dataPointer - 0x80);
	    PushInFifo(sendfifo,0xFD);
     }else
	 PushInFifo(sendfifo, *dataPointer);
     dataPointer++;
   }
   PushInFifo(sendfifo,checksum);
   PushInFifo(sendfifo,END);
	 CheckFifoSendFirstData(sendfifo);
 }
}

/****************
* Function Name:      TransmitTermData
* Description:        传输所有端点数据
                      
                      
* Parameter:           端点的编号 索引 buffer: 整合到这个 buffer中
* return:             none
* Date:               20170526
*author:              CTK  luxq
***************/


void TransmitAllTermData(const _sTERMI_FORMAT* datacmd,uint8_t* sendbuffer,_sFIFO* sendfifo,uint8_t sendType)
{ 
   uint8_t* dataPointer;
   uint8_t length =0,length1,length2;
   uint8_t checksum = 0;
   uint8_t i;
	 _sTERMI_FORMAT* term_pointer;
	 if(xSemaphoreTake(semSendEnable, 2))
	{
   dataPointer = sendbuffer;
	 term_pointer = (_sTERMI_FORMAT*)datacmd;
   length1 = sizeof(termination_info)/sizeof(termination_info[0]);
   for(i=0;i<length1;i++)
   {
   TermDataStruct(i,dataPointer);
   dataPointer =dataPointer+term_pointer->length + 3;
   length = length + term_pointer->length + 3;//  + cmd + index + checksum
	 term_pointer++;
   }
	 length +=2;
   length1 = (uint8_t)(length/256);// 加上 命令 和校验和
   length2 = (uint8_t)(length%256);
   checksum ^= sendType;//CMD_SND_ALL_TERM_DATA;
	 checksum ^= length1;
	 checksum ^= length2;
   PushInFifo(sendfifo,HEAD);
   PushInFifo(sendfifo,length1);
   PushInFifo(sendfifo, length2);
	 PushInFifo(sendfifo,sendType);
	 dataPointer = sendbuffer;
	 length -= 2;
   for(i=0;i<length;i++)
   {
     checksum ^=*dataPointer;
     if(*dataPointer >= 0xFD)
     {
	 	PushInFifo(sendfifo,*dataPointer - 0x80);
	    PushInFifo(sendfifo,0xFD);
     }else
	 PushInFifo(sendfifo, *dataPointer);
     dataPointer++;
   }
   PushInFifo(sendfifo,checksum);
   PushInFifo(sendfifo,END);
	 CheckFifoSendFirstData(sendfifo);
}
}


void ClearReceiveBuffer(void)
{
  memset(wifiFrame->data,0,128);
}


void WifiRecRespond(void)
{
	uint8_t checksum;
	uint8_t sendType;
	checksum = 0;
	checksum  ^=0x02;
	sendType = CMD_REC_TERM_DATA;
	checksum ^= sendType;
	PushInFifo(uartSendFifo,HEAD);
	PushInFifo(uartSendFifo,0x00);   
	PushInFifo(uartSendFifo, 0x02);	 
	PushInFifo(uartSendFifo,sendType);
	PushInFifo(uartSendFifo,checksum);
	PushInFifo(uartSendFifo,END);
	CheckFifoSendFirstData(uartSendFifo);
}

void CheckFifoSendFirstData(_sFIFO* fifo)
{
 uint8_t sendData;
 if(fifo->count !=0)
 {
  while(USART_GetFlagStatus(WIFI_UART,USART_FLAG_TXE) == RESET)
  	;
  PopFromFifo(fifo,&sendData);
  USART_SendData(WIFI_UART,sendData);
 }
}


typedef enum
{
	UPDATE_TYPE_NONE = 0x00,
	UPDATE_TYPE_WIFI,
	UPDATE_TYPE_MCU,
	UPDATE_TYPE_SUB
}_eUPDATE_TYPE;

#define DEV_DESCRIPTION   0x00000001

void GetUpdateStatus(void)
{
	uint8_t checksum;
	uint8_t sendType;
	uint8_t updateType;
	uint16_t upVersion;
	uint32_t description;
	uint8_t* pointer;
	updateType = UPDATE_TYPE_MCU;
	upVersion = GetFirmVersion();  // get version
	description = DEV_DESCRIPTION;
	checksum = 0;
	checksum  ^=0x09;// 2byte + 7byte (type  version description)
	sendType = CMD_ASK_UPDATE;
	checksum ^= sendType;
	PushInFifo(uartSendFifo,HEAD);
	PushInFifo(uartSendFifo,0x00);   
	PushInFifo(uartSendFifo, 0x09);	 
	PushInFifo(uartSendFifo,sendType);
	checksum ^= updateType;
	PushInFifo(uartSendFifo,updateType);
	checksum ^=upVersion;
	PushInFifo(uartSendFifo,upVersion>>8);
	PushInFifo(uartSendFifo,upVersion);
	pointer = (uint8_t*)&description;	
	checksum ^=*pointer++;
	checksum ^=*pointer++;
	checksum ^=*pointer++;
	checksum ^=*pointer++;
	PushInFifo(uartSendFifo,(uint8_t)(description>>24));
	PushInFifo(uartSendFifo,(uint8_t)(description>>16));
	PushInFifo(uartSendFifo,(uint8_t)(description>>8));
	PushInFifo(uartSendFifo,(uint8_t)(description));
	PushInFifo(uartSendFifo,checksum);
	PushInFifo(uartSendFifo,END);
	CheckFifoSendFirstData(uartSendFifo);
}




