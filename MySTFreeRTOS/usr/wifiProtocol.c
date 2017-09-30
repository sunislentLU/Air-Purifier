#ifndef FOR_JP
#include "wifiProtocol.h"
#include "fifo.h"
#include "stdio.h"
#include "string.h"
extern void CheckFifoSendFirstData(_sFIFO* fifo);
/****************
* Function Name:      WifiCmdStructure
* Description:        struct the wifi send data format
                      
* Parameter:          cmd :command  data: data need to send ,fifo: uartsendFifo
* return:             none
* Date:               20170517
*author:              CTK  luxq
***************/
//void WifiCmdStructure(_eCOMMAND cmd,unsigned char* data,_sFIFO fifo)
//{
//	unsigned char* sendBuffer;
//	unsigned char* point;
//	unsigned char checksum;
//	unsigned char lenght = 0;
//	unsigned char i;
//    *point++ = HEAD;
//	point +=2;
//	*point++= cmd;
//	checksum ^= cmd;
//	sendBuffer = pvPortMalloc(128*sizeof(unsigned char));
//	lenght = CheckTableLenght(data);
//	for(i = 0;i<lenght;i++)
//	{
//	  checksum^=*data;
//	  if(*data >= 0xfd)
//	  {
//	    *point++ = *data - 0x80;
//	    *point++ = 0x7d;
//	  }else
//        *point++ = *data++;
//	}
//	lenght +=2;
//	checksum ^= lenght;
//	*point++ = checksum;
//	*point = END;
//	 point = sendBuffer;
//	 point++;
//	*point++ = lenght /256;
//	*point = lenght%256;
//	lenght = CheckTableLenght(sendBuffer);
//	for(i=0;i<lenght;i++)
//	{
//       PushInFifo(fifo, sendBuffer[i]);
//	}
//	vPortFree(sendBuffer);
//}
/****************
* Function Name:      CheckTableLenght
* Description:        check table length
                      
* Parameter:          table pointer
* return:             table variable data
* Date:               20170517
*author:              CTK  luxq
***************/
unsigned char CheckTableLenght(unsigned char* table)
{
  unsigned char i=0;
  unsigned char* table_tmp;
  table_tmp = table;
  while(*table_tmp != 0x00)
  {
  	i++;
	table_tmp++;
  }
  return i;
}


/****************
* Function Name:      ParseWifiDatas
* Description:        parse wifi raw datas 
                      
* Parameter:          wifiData: wifi receive datas
* return:             1: data is right  0:data is error
* Date:               20170517
*author:              CTK  luxq
***************/
unsigned char  ParseWifiDatas(_sWIFI_FORMAT* wifiData)
{
// unsigned char* data_tmp;
 unsigned char checksum = 0;
 unsigned char i= 0;//,j = 0,lenght = 0,length1=0;;
// data_tmp = wifiData->data;
// lenght = wifiData->length - 1;//CheckTableLenght(data_tmp);
// for(i=0;i<lenght;i++)
// {
//    if((*data_tmp >= 0x7d)&&(*(data_tmp+1) == 0xfd))
//    {
//		*data_tmp +=0x80;
//		data_tmp++;
//		length1 = lenght-j+1;
//		for(j=i;j<length1;j++)
//		{
//			*data_tmp = *(data_tmp+1);
//			 data_tmp++;
//		}
//		data_tmp = data_tmp - lenght-j-1;
//	}	else
//		data_tmp++;
// }

 for(i = 0;i<wifiData->length;i++)
 {
   checksum ^=wifiData->data[i];
 }
  checksum ^=wifiData->cmd;
   checksum^=wifiData->length;
 if(checksum == wifiData->checksum)
 	return 1;
 else
 	return 0;
}
/****************
* Function Name:      SendCmd2WifiModule
* Description:        wifi uart send data frame struct
                      
* Parameter:          cmd:comamnds  cmdarry[]:command and the other member of frame store address
                      databuff: pointer of the datas
                      sendfifo: use for sendata store datas which need to send to wifi module via uart
* return:             none
* Date:               20170519
*author:              CTK  luxq
***************/
void SendCmd2WifiModule(unsigned char cmd,const _sWIFI_CMD_PROC cmdarray[],unsigned char* databuff,_sFIFO* sendfifo)
{
  unsigned char command;
  unsigned short dataLen;
  unsigned short i;
  unsigned char chksum;
  unsigned char* pointer;
  memset(databuff,0,WIFI_MAX_SEND_BUFF);//
 
  
  pointer = databuff;
  command = cmd;
	if(cmd == 0x05)
	{
	 command --;//= 0x05;
		command++;
		return ;
		
		
	}
	dataLen = 0;
  dataLen = cmdarray[command].length2*256+cmdarray[command].length1;
  if(dataLen <= 2) // command + datas +checksum  == 2 ;mean the command contain no data
  {
    PushInFifo(sendfifo,HEAD);
	PushInFifo(sendfifo, cmdarray[command].length2);
    PushInFifo(sendfifo, cmdarray[command].length1);
    PushInFifo(sendfifo,command);
	chksum = cmdarray[command].getchecksum(command);
  }else // have data
  {
	PushInFifo(sendfifo,HEAD);
	PushInFifo(sendfifo, cmdarray[command].length2);
    PushInFifo(sendfifo, cmdarray[command].length1);
    PushInFifo(sendfifo,command);
	cmdarray[command].fecthdata();
	chksum = cmdarray[command].getchecksum(command);// get checksum
	for(i=0;i<dataLen-2;i++)
	{
     if(*pointer >= 0xFD)
     {
        PushInFifo(sendfifo,*pointer - 0x80);
		PushInFifo(sendfifo, 0xFD);
	 }else
	 {
       PushInFifo(sendfifo, *pointer);
	 }
	 pointer++;
	}
	pointer = databuff;
	memset(pointer,0x00,dataLen);
  }
  PushInFifo(sendfifo,chksum);
  PushInFifo(sendfifo,END);
  CheckFifoSendFirstData(sendfifo);
}
#endif















