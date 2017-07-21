
/**
  ******************************************************************************
  * @file    AP-C120
  * @author  CTK 
  * @version V1.0.0
  * @date    26-April-2017
  * @brief    main.c module
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT CTK
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "main.h"
#include "hardware.h"
#include "stdio.h"
#include "string.h"
/*Fucntions Declare*/
void WaitDelayms(uint16_t ms);
uint8_t CheckBufferChksum(uint8_t* buff,uint8_t length);
void UartAskUpdateInfo(void);
void UpdateRequire(void);
void UpdateDataRespond(uint16_t sn);
uint8_t FLASH_ProgramPage(uint8_t* dataBuf,uint32_t address);
void FinishRespond(void);
void UploadNewVersion(uint16_t firmVer);
void RebootWifiModule(void);
extern void CheckCodeMd5Result(unsigned char* resultBuff,uint16_t dataLen,uint32_t startAddr);

uint8_t is_msExpired = 0;
_eBOOT_PRC bootState = BOOT_WAIT2APP;
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint8_t dataBuffer[1024];
uint8_t recBuff[128 + 6];
uint8_t sendBuff[16];
__align(4) uint8_t chkStr[16];
const uint8_t updateStr[]={0xff,0x00,0x03,0x31,0x80,0xB2,0xfe};//请求下发升级数据
const uint8_t finishStr[]={0xff,0x00,0x02,0x33,0x31,0xfe};
const uint8_t rebootStr[]={0xff,0x00,0x02,0x05,0x07,0xfe};
	
uint32_t programAddr;	
uint32_t* u32Pointer;
uint16_t dataLength = 0;
uint16_t wifiLedCnt = 0;
int main(void)
{
	uint16_t bootArg;
	uint16_t updateVer;
	uint16_t waitCnt = 0;
	uint16_t recCnt;
	uint8_t data;
	uint8_t i = 0;
	uint8_t checksum;
	uint16_t dataSn;
	uint8_t* dataPointer;
	uint8_t dataNum;
	uint16_t oldVersion = 0;
	uint8_t ret;
	uint32_t readAddr;
	HardWareInit();	
	readAddr = FIRMLEN_ADDR;
	dataLength = (uint16_t)(*((__IO uint32_t*)readAddr));
	if(dataLength == 0xffff)
		dataLength = 0;
	readAddr = BOOT_DATA_ADDR;	   
	bootArg = (uint16_t)(*((__IO uint32_t*)readAddr));// 如果从APP接收到升级命令进来则这里是新版本的固件版本号
	readAddr = VERSION_ADDR;
	oldVersion = (uint16_t)(*((__IO uint32_t*)readAddr));//旧版本的固件
	if(oldVersion == 0xffff)
		oldVersion = 0x0001;
	if((bootArg == 0xffff)||(bootArg == 0x1234))//update complete 
	{
		//RebootWifiModule();
	//	WaitDelayms(1000);
		bootState = BOOT_WAIT2APP;		
	}else
	{
		if(oldVersion != bootArg)
		{
			WIFI_LED_ON();
			updateVer= bootArg;
			FLASH_Unlock();
			programAddr = APP_START_ADDR;														
			for(i=0;i<APP_PAGE_NUM;i++)							
			{																							
				waitCnt = 0;															
				ret = FLASH_ErasePage(programAddr);															
				if(ret != FLASH_COMPLETE)//错误															
				{									
					break;															
				}								
				programAddr+=PAGE_SIZE;							
			}														
			if(i==APP_PAGE_NUM)//擦除成功						 							
			{																
				bootState = BOOT_WAIT_REQ;								
				waitCnt = 0;
				recCnt = 0;
				memset(recBuff,0,134);															
				FLASH_Lock();						
			}			
		}
	}
	recCnt = 0;
	memset(recBuff,0,134);//init data
	memset(sendBuff,0,16);
	memset(dataBuffer,0,PAGE_SIZE);
	while(1)
	{
	switch(bootState)
	{
		case BOOT_WAIT2APP:
			//--------------------------data receive---------------------------
		if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == SET)
		{
			data = USART_ReceiveData(USART2);
			if(data == 0xff)// head 
				recCnt =0;
			if((data == 0xfd)&&(recBuff[recCnt -1]>=0x7d))//
			{
				recBuff[recCnt - 1] += 0x80;
				break;
			}
			recBuff[recCnt++] = data;
			if(data == 0xfe)//end
			{
				checksum = 0;
				for(i=1;i<recCnt-2;i++)
				{
					checksum^=recBuff[i];
				}
				if(checksum == recBuff[recCnt - 2])
				{
					if((recBuff[3] == 0x30)&&(recBuff[4] == 1))
					{
							updateVer = recBuff[5];
						  updateVer<<=8;
							updateVer = recBuff[6];
							if(updateVer == oldVersion)//升级版本和现有版本一致
							{
								waitCnt = 0;						
								bootState = BOOT_END;
								break;
							}
							FLASH_Unlock();
							ret = FLASH_ErasePage(BOOT_OPTION_ADDR);
							if(ret == FLASH_COMPLETE)
							{
								bootArg = 0x1234;
								ret = FLASH_ProgramHalfWord(BOOT_OPTION_ADDR,bootArg);
								if(ret != FLASH_COMPLETE)
									break;
							}
							programAddr = APP_START_ADDR;							
							for(i=0;i<APP_PAGE_NUM;i++)
							{															
								waitCnt = 0;							
								ret = FLASH_ErasePage(programAddr);							
								if(ret != FLASH_COMPLETE)//错误							
								{
									break;							
								}
								programAddr+=PAGE_SIZE;
							}							
							if(i==APP_PAGE_NUM)//擦除成功						 
							{	
								bootState = BOOT_WAIT_REQ;								
								waitCnt = 0;
								recCnt = 0;
								memset(recBuff,0,134);							
								FLASH_Lock();		
                break;								
							}else//擦除失败						 
							{						 
							}			
					}
				
				}else//checksum 失败
				{
					break;
				}
				recCnt = 0;	
        				
			}
		}else
		//-------------------------time out check----------------------------------------------------
		{
			if(is_msExpired == 1)
			{
				is_msExpired = 0;
				waitCnt++;								
				if(waitCnt >= 2000)// 1second to jump to app branch
				{
					waitCnt = 0;	
					bootState = BOOT_END;//BOOT_END;// BOOT_CHK;	
				}			
				if((waitCnt%500) == 0)// 100ms ask wifi module update information
					UartAskUpdateInfo();			
			}		
		}						
			break;
		case BOOT_WAIT_REQ:		// 发送 升级请求			
		while(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == RESET)
		{
			if(is_msExpired == 1)
			{
				is_msExpired = 0;
				waitCnt++;								
				if(waitCnt >= 10000)// 1second to jump to app branch
				{
					waitCnt = 0;
					RebootWifiModule();
					bootState = BOOT_WAIT2APP;
          					
				}			
			if((waitCnt%500) == 0)// 100ms ask wifi module update information
					UpdateRequire();			
			}						
		}
		  waitCnt = 0;
			data = USART_ReceiveData(USART2);
			if(data == 0xff)// head 
				recCnt =0;
			if((data == 0xfd)&&(recBuff[recCnt -1]>=0x7d))//
			{
				recBuff[recCnt - 1] += 0x80;
				break;
			}
			recBuff[recCnt++] = data;
			if(data == 0xfe)//end
			{
				checksum = 0;
				for(i=1;i<recCnt-2;i++)
				{
					checksum^=recBuff[i];
				}
				if(checksum == recBuff[recCnt - 2])//校验正确
				{	
					if(recBuff[3] == 0x31)
					{
						if(recBuff[4] == 0x01)//开始升级
						{
							programAddr = APP_START_ADDR;
							FLASH_Unlock();

							bootState = BOOT_WAIT_DATA;
							memset(recBuff,0,134);
							waitCnt = 0; 
							dataSn = 0;
							dataPointer = dataBuffer;
							dataLength = 0;
							programAddr = APP_START_ADDR;
							break;							
						}else if(recBuff[4] == 0x00)//没有升级
						{
							waitCnt = 0; 
							dataSn = 0;		
              bootState = BOOT_END;							
						}					
					  waitCnt = 0;	
					}								
					recCnt = 0;
				}
			}break;
	 case BOOT_WAIT_DATA:// receive flash data
		 while(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == RESET)
		 {
			 if(is_msExpired == 1)
			{
				is_msExpired = 0;
				waitCnt++;
        wifiLedCnt++;				
				if(waitCnt >= 5000)// 1second to jump to app branch
				{
					RebootWifiModule();			
					WaitDelayms(100);		
					bootState = BOOT_WAIT2APP;
					waitCnt = 0;		
					break;
				}
				if((wifiLedCnt%200) == 0)
				{
					if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_14) == 0)
						WIFI_LED_OFF();
					else
						WIFI_LED_ON();
				
				}
				
			}			 
		 }
			waitCnt = 0;
			data = USART_ReceiveData(USART2);
			if(data == 0xff)// head 
				recCnt =0;
			if((data == 0xfd)&&(recBuff[recCnt -1]>=0x7d))//
			{
				recBuff[recCnt - 1] += 0x80;
				break;
			}
			recBuff[recCnt++] = data;
			if(data == 0xfe)//end
			{
				checksum = 0;
				for(i=1;i<recCnt-2;i++)
				{
					checksum^=recBuff[i];
				}
				if(checksum == recBuff[recCnt - 2])//校验正确
				{
					if(recBuff[3] == 0x32)//data frame
					{
						dataSn = recBuff[4];
						dataSn <<=8;
						dataSn |= recBuff[5];
						dataNum = recBuff[6];
						if(dataNum == 0x00)//256 bytes format
						{
						memcpy(dataPointer,&recBuff[7],256);//
						dataPointer += 256;	
						dataLength += 256;
						}
						else
						{
							memcpy(dataPointer,&recBuff[7],dataNum);//
							dataPointer += dataNum;	
							dataLength += dataNum;
						}
						if((dataSn%8) == 0)//接收到1K 数据 一页Flash 的内容
						{
							dataPointer = dataBuffer;
							FLASH_Unlock();
							ret = FLASH_ProgramPage(dataPointer,programAddr);
							if(ret != 0)// 错误
							{
								break;							
							}
							programAddr+=1024;
							FLASH_Lock();
							dataPointer = dataBuffer;
							memset(dataPointer,0xff,1024);
						}							
						UpdateDataRespond(dataSn);
					}else if(recBuff[3] == 0x33)//finish frame
					{									
						bootState = BOOT_CHK;
						waitCnt = 0;
						recCnt = 0;	
						if(dataLength>(programAddr-APP_START_ADDR))
						{
							dataPointer = dataBuffer;
							FLASH_Unlock();
							ret = FLASH_ProgramPage(dataPointer,programAddr);
							if(ret != 0)// 错误
							{
								break;							
							}
						}
						dataLength = recBuff[4];
						dataLength<<=8;
						dataLength |=recBuff[5];
						dataLength<<=8;
						dataLength |=recBuff[6];
						dataLength<<=8;
						dataLength |=recBuff[7];
            dataPointer = chkStr;
            memcpy(dataPointer,&recBuff[8],16);			
						FLASH_Unlock();	 			 
						ret = FLASH_ErasePage(BOOT_OPTION_ADDR);	 	 			 
						bootArg = 0xffff;					
            readAddr = 	BOOT_OPTION_ADDR;					
						ret = FLASH_ProgramHalfWord(readAddr,bootArg);									 			 
						if(ret != FLASH_COMPLETE)										 			 
						break;	 
            readAddr = 	VERSION_ADDR;							
						ret = FLASH_ProgramHalfWord(readAddr,updateVer);									 
						if(ret!= FLASH_COMPLETE)									
						break;	  	             	 			 
						readAddr = 	FIRMLEN_ADDR;											 			 
						ret = FLASH_ProgramHalfWord(readAddr,dataLength);															 			 
						if(ret!= FLASH_COMPLETE)									 
						break;	   						
						FLASH_Unlock();	            			 
						programAddr = CHECK_RESULT;
            memcpy(chkStr,dataPointer,16);	
            u32Pointer = 	(uint32_t*)chkStr;						
						for(i=0;i<16;i=i+4)            			 
						{				 										 
							ret = FLASH_ProgramWord(programAddr,*u32Pointer);				 
							if(ret != FLASH_COMPLETE)												 
							break;											 
							programAddr+=4;											 
							u32Pointer ++;            			 
						}					 
						FinishRespond();
						break;
					}					
				}
				recCnt = 0;
			}
			break;
	 case BOOT_CHK:// check version and upload 	
   if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE) == SET)
		{
			waitCnt = 0;
			data = USART_ReceiveData(USART2);
			if(data == 0xff)// head 
				recCnt =0;
			if((data == 0xfd)&&(recBuff[recCnt -1]>=0x7d))//
			{
				recBuff[recCnt - 1] += 0x80;
				break;
			}
			recBuff[recCnt++] = data;
			if(data == 0xfe)//end
			{
				checksum = 0;
				for(i=1;i<recCnt-2;i++)
				{
					checksum^=recBuff[i];
				}
				if(checksum == recBuff[recCnt - 2])//校验正确
				{	
					if(recBuff[3] == 0x34)
					{
//						if(recBuff[4] >= 0x01)//无论上报是否成功都要转到新的APP中运行
//						{
							waitCnt = 0;
						//	RebootWifiModule();
							bootState = BOOT_END; 
							break;
//						}
					}
				}
			}
		}			
	 if(is_msExpired == 1)
	 {
     is_msExpired = 0;
     waitCnt ++;
     if(waitCnt >= 100)
		 {
       waitCnt = 0;			 
			 UploadNewVersion(oldVersion);
		 } 
	 }
		 break;
	 
	case BOOT_END:
	memset(chkStr,0,16);
	memset(sendBuff,0,16);
	CheckCodeMd5Result(chkStr,dataLength,APP_START_ADDR);
	readAddr = CHECK_RESULT;
	memcpy(sendBuff,(uint32_t*)readAddr,16);
//	if(strncmp((const char*)chkStr,(const char*)sendBuff,16) == 0)
//	{
	
    if (((*(__IO uint32_t*)APP_START_ADDR) & 0x2FFE0000 ) == 0x20000000)
    { 
			RebootWifiModule();
			WaitDelayms(100);
      JumpAddress = *(__IO uint32_t*) (APP_START_ADDR + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      __set_MSP(*(__IO uint32_t*) APP_START_ADDR);  
      Jump_To_Application();
		}else
		{
		  NVIC_SystemReset();
		}
			break;
//	 }else
//	{									
//		waitCnt = 0;							
//		bootState = BOOT_WAIT2APP; 
//	}
//	break;
		default:
			break;

	}
	}	
}



void WaitDelayms(uint16_t ms)
{
	uint16_t waitms;
	waitms = ms;
	while(waitms)
	{
   while(is_msExpired == 1)
    {
	    is_msExpired = 0; 
			waitms -- ;
    }
  }
}


void UartAskUpdateInfo(void)
{
	uint8_t* pointer;
	uint8_t* pointer1;
	uint16_t firmVer;
	uint8_t chksum;
	uint8_t i;
	uint32_t address;
	pointer = sendBuff;
	*pointer++ = 0xff;
	*pointer++ = 0x00;
	*pointer++ = 0x09;
	*pointer++ = 0x30;
	*pointer++ = 0x02;
	address = VERSION_ADDR;
	firmVer = (uint16_t) *((__IO uint32_t*)address);
	if(firmVer == 0xffff)
		firmVer = 0x0001;
	*pointer++ = (uint8_t)(firmVer>>8);
	*pointer++ = (uint8_t)(firmVer);
	*pointer++ = 0x00;
	*pointer++ = 0x00;
	*pointer++ = 0x00;
	*pointer++ = 0x01;
	pointer1 = sendBuff;
	pointer1++;
	chksum = CheckBufferChksum(pointer1,10);
	*pointer++ = chksum;
	*pointer = 0xfe;
	pointer = sendBuff;
	for(i=0;i<13;i++)
	{					
		USART_SendData(USART2,*pointer);	
		pointer++;
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)== RESET)
		{}
	}
	memset(pointer,0,16);
}


uint8_t CheckBufferChksum(uint8_t* buff,uint8_t length)
{
	uint8_t* pointer;
	uint8_t i=0;		
	uint8_t chksum;
	pointer = buff;
	chksum = 0;
	for(i=0;i<length;i++)
	{
		chksum^=*(pointer++);
	}
  return chksum;
}


void UpdateRequire(void)
{
	uint8_t length;
	uint8_t i;
	length = sizeof(updateStr);
	i = 0;
	while(length--)
	{
	 USART_SendData(USART2,updateStr[i]);
		i++;			
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)== RESET)
		{}
	}
}

void FinishRespond(void)
{
	uint8_t length;
	uint8_t i;
	length = sizeof(finishStr);
	i = 0;
	while(length--)
	{
	 USART_SendData(USART2,finishStr[i]);
		i++;				
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)== RESET)
		{}
	}
}


void UpdateDataRespond(uint16_t sn)
{
	uint8_t* pointer,*pointer1;
	uint8_t chksum;
	uint8_t i;
	pointer = sendBuff;
	*pointer++ = 0xff;
	*pointer++ = 0x00;
	*pointer++ = 0x04;
	*pointer++ = 0x32;
	*pointer++ = sn>>8;
	*pointer++ = sn;
	pointer1 = sendBuff +1;
	chksum = CheckBufferChksum(pointer1,5);
	*pointer ++ = chksum;
	*pointer = 0xfe;
	pointer = sendBuff;
	for(i=0;i<8;i++)
	{
		USART_SendData(USART2,*pointer);	
		pointer++;				
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)== RESET)
		{}
	}
	memset(pointer,0,16);
}


void UploadNewVersion(uint16_t firmVer)
{
	uint8_t* pointer,*pointer1;
	uint8_t chksum;
	uint8_t i;
	uint32_t readAddr;
	pointer = sendBuff;
	*pointer++ = 0xff;
	*pointer++ = 0x00;
	*pointer++ = 0x0B;// 11 个字节
	*pointer++ = 0x34;
  *pointer++ = 0x02;
	*pointer++ = 0x00;
	*pointer++ = 0x00;	 	
	*pointer++ = 0x00;
	*pointer++ = 0x01;
	*pointer++ = firmVer>>8;
	*pointer++ = firmVer;
	readAddr = VERSION_ADDR;
  firmVer = (uint16_t) *((__IO uint32_t*)readAddr);
	if(firmVer == 0xffff)
		firmVer = 0x0001;
	*pointer++ = (uint8_t)(firmVer>>8);
	*pointer++ = (uint8_t)(firmVer);
	pointer1 = sendBuff;
	pointer1++;
	chksum = CheckBufferChksum(pointer1,12);
	*pointer ++ = chksum;
	*pointer = 0xfe;
	pointer = sendBuff;
	for(i=0;i<15;i++)
	{	
		USART_SendData(USART2,*pointer);	
		pointer++;
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)== RESET)
		{}
	}
	memset(pointer,0,16);
}



uint8_t FLASH_ProgramPage(uint8_t* dataBuf,uint32_t address)
{
  uint32_t addr;
	uint32_t* dataPointer;
	uint16_t i;
	uint8_t ret;
	addr = address;
	dataPointer = (uint32_t*)dataBuf;
	for(i=0;i<1024;i=i+4)
	{
		ret = FLASH_ProgramWord(addr,*dataPointer);
		if(ret != FLASH_COMPLETE)
			return 1;//error 
		dataPointer++;
		addr+=4;
	}			
	return 0;
}

void RebootWifiModule(void)
{
	uint8_t length;
	uint8_t i;
	length = sizeof(rebootStr);
	i = 0;
	while(length--)
	{
	 USART_SendData(USART2,rebootStr[i]);			
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)== RESET)
			{}
		i++;
	}
}






