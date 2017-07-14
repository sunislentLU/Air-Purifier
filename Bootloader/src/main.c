
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


uint8_t is_msExpired = 0;
_eBOOT_PRC bootState = BOOT_WAIT2APP;
pFunction Jump_To_Application;
uint32_t JumpAddress;
uint8_t dataBuffer[1024];
uint8_t recBuff[256 + 6];
uint8_t sendBuff[16];
const uint8_t updateStr[]={0xff,0x00,0x03,0x31,0x00,0x32,0xfe};//请求下发升级数据
const uint8_t finishStr[]={0xff,0x00,0x02,0x33,0x31,0xfe};
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
	uint32_t programAddr;
	uint16_t oldVersion;
	uint32_t dataLength ;
	uint8_t ret;
	
	
	HardWareInit();
	bootArg = (uint16_t)(*(__IO uint32_t*)BOOT_DATA_ADDR);
	oldVersion = (uint16_t)(*(__IO uint32_t*) VERSION_ADDR);
	if(bootArg == 0xffff)//update complete 
	{
		bootState = BOOT_WAIT2APP;		
	}else
	{
		if(bootArg == 0xa5a5)
		{
			bootState = BOOT_WAIT_REQ;
		}else if(bootArg == 0x1234)
		{
			bootState = BOOT_WAIT_REQ;
		}
	
	}
	recCnt = 0;
	memset(recBuff,0,262);//init data
	memset(sendBuff,0,16);
	memset(dataBuffer,0,1024);
	dataLength = 0;
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
						if(recCnt == 8)
						{
							updateVer = recBuff[5];
						  updateVer<<=8;
							updateVer = recBuff[6];
							FLASH_Unlock();
							ret = FLASH_ErasePage(BOOT_OPTION_ADDR);
							if(ret == FLASH_COMPLETE)
							{
								bootArg = 0x1234;
								ret = FLASH_ProgramHalfWord(BOOT_OPTION_ADDR,bootArg);
								if(ret != FLASH_COMPLETE)
									break;
								ret = FLASH_ProgramHalfWord(VERSION_ADDR,updateVer);
								if(ret!= FLASH_COMPLETE)
									break;
								bootState = BOOT_WAIT_REQ;
								waitCnt = 0;
								memset(recBuff,0,262);
							}
							FLASH_Lock();
						}else//小于8说明是错误帧
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
				if(waitCnt >= 1000)// 1second to jump to app branch
				{
					waitCnt = 0;	
					bootState = BOOT_END;	
				}			
				if((waitCnt%100) == 0)// 100ms ask wifi module update information
					UartAskUpdateInfo();			
			}		
		}						
			break;
		
		case BOOT_WAIT_REQ:		// 发送 升级请求			
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
					if(recBuff[3] == 0x31)
					{
						if(recBuff[4] == 0x01)//开始升级
						{
							bootState = BOOT_WAIT_DATA;
							memset(recBuff,0,262);
							waitCnt = 0; 
							dataSn = 0;
							dataPointer = dataBuffer;
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
			}
		}else
		{						
			if(is_msExpired == 1)
			{
				is_msExpired = 0;
				waitCnt++;								
				if(waitCnt >= 1000)// 1second to jump to app branch
				{
					waitCnt = 0;		
				}			
			if((waitCnt%100) == 0)// 100ms ask wifi module update information
					UpdateRequire();			
			}			
		}
			break;
		
	 case BOOT_WAIT_DATA:// receive flash data
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
					if(recBuff[3] == 0x32)//data frame
					{
						dataSn = recBuff[4];
						dataSn <<=8;
						dataSn |= recBuff[5];
						dataNum = recBuff[6];
						if(dataNum == 0)//256 bytes format
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
						if((dataSn%4) == 0)//接收到1K 数据 一页Flash 的内容
						{
							dataPointer = dataBuffer;
							FLASH_Unlock();
							programAddr = dataSn/4;
							ret = FLASH_ErasePage(programAddr);
							if(ret != FLASH_COMPLETE)//错误
							{
								break;
							}
							ret = FLASH_ProgramPage(dataPointer,programAddr);
							if(ret != 0)// 错误
							{
								break;							
							}
							programAddr+=1024;
							FLASH_Lock();
						}							
						UpdateDataRespond(dataSn);
					}else if(recBuff[3] == 0x33)//finish frame
					{									
						bootState = BOOT_CHK;
						waitCnt = 0;
						recCnt = 0;												
						FinishRespond();
						break;
					}					
				}
				recCnt = 0;
			}
		}else
		{						
			if(is_msExpired == 1)
			{
				is_msExpired = 0;
				waitCnt++;								
				if(waitCnt >= 1000)// 1second to jump to app branch
				{
					waitCnt = 0;		
				}			
				if((waitCnt%100) == 0)// 100ms ask wifi module update information
					UpdateRequire();			
			}	
		}
			break;
	 case BOOT_CHK:// check version and upload 		 
	  UploadNewVersion(oldVersion);
	  bootState = BOOT_END;
		 break;
	 
	case BOOT_END:
    if (((*(__IO uint32_t*)APP_START_ADDR) & 0x2FFE0000 ) == 0x20000000)
    { 
      JumpAddress = *(__IO uint32_t*) (APP_START_ADDR + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      __set_MSP(*(__IO uint32_t*) APP_START_ADDR);  
      Jump_To_Application();
		}else
		  NVIC_SystemReset();
			break;
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
	pointer = sendBuff;
	*pointer++ = 0xff;
	*pointer++ = 0x00;
	*pointer++ = 0x09;
	*pointer++ = 0x30;
	*pointer++ = 0x02;
	firmVer = (uint16_t) *((__IO uint32_t*)VERSION_ADDR);
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
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!= SET)
			;
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
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!= SET)
			;
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
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!= SET)
			;
	}
}


void UpdateDataRespond(uint16_t sn)
{
	uint8_t* pointer;
	uint8_t chksum;
	uint8_t i;
	pointer = sendBuff;
	*pointer++ = 0xff;
	*pointer++ = 0x00;
	*pointer++ = 0x04;
	*pointer++ = 0x32;
	*pointer++ = sn>>8;
	*pointer++ = sn;
	chksum = CheckBufferChksum(sendBuff,5);
	*pointer ++ = chksum;
	*pointer = 0xfe;
	pointer = sendBuff;
	for(i=0;i<8;i++)
	{	
		USART_SendData(USART2,*pointer);	
		pointer++;
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!= SET)
			;
	}
	memset(pointer,0,16);
}


void UploadNewVersion(uint16_t firmVer)
{
	uint8_t* pointer;
	uint8_t chksum;
	uint8_t i;
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
  firmVer = (uint16_t) *((__IO uint32_t*)VERSION_ADDR);
	if(firmVer == 0xffff)
		firmVer = 0x0001;
	*pointer++ = 0x00;
	*pointer++ = (uint8_t)(firmVer>>8);
	*pointer++ = (uint8_t)(firmVer);
	chksum = CheckBufferChksum(sendBuff,12);
	*pointer ++ = chksum;
	*pointer = 0xfe;
	pointer = sendBuff;
	for(i=0;i<15;i++)
	{	
		USART_SendData(USART2,*pointer);	
		pointer++;
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)!= SET)
			;
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
	for(i=0;i<1024;i=i+4)
	{
		dataPointer = (uint32_t*)dataBuf;
		ret = FLASH_ProgramWord(addr,*dataPointer);
		if(ret != FLASH_COMPLETE)
			return 1;//error 
		dataPointer++;
		addr+=4;
	}			
	return 0;
}








