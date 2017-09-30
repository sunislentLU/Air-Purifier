#include "pms7003i.h"
#include "stdio.h"
#include "string.h"

uint8_t GetParticalDensity(uint16_t*pm25,uint16_t* pm100)
{
	uint16_t timeout;
	uint8_t regAddr;
	uint8_t readBuffer[16];
	uint8_t i = 0;
	uint16_t tmp;
	I2C_TransferHandling(I2C2, PMS7003I_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TXIS) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
	regAddr = START1_ADDR;
	I2C_SendData(I2C2,regAddr);
		timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TC) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
	I2C_TransferHandling(I2C2, PMS7003I_ADDR, 16, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	timeout = PMS7003_TIMEOUT;
	for(i=0;i<16;i++)
	{
		while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	  {
		if((timeout--) == 0)
			return 1;
	  }
	 readBuffer[i]= I2C_ReceiveData(I2C2);
	}
	I2C_ClearFlag(I2C2,I2C_ISR_STOPF);
	 tmp = readBuffer[0x0c];
	 tmp <<=8;
	 tmp |= readBuffer[0x0d];
	 *pm25 = tmp;
		 
	 tmp = readBuffer[0x0e];
	 tmp <<=8;
	 tmp |= readBuffer[0x0f];
	 *pm100 = tmp;
	 memset(readBuffer,0,16);
return 0;
}


uint8_t SetPms7003iStandby(uint8_t onoff)
{
	uint8_t sendBuf[39];
	uint8_t* pointer;
	uint8_t i = 0;
	uint16_t chk = 0;
	uint16_t timeout;
	
    memset(sendBuf,0,39);
	pointer = sendBuf;
    pointer+=32;
	*pointer++ = 0x42;
	*pointer++ = 0x4d;
	*pointer++ = 0xe4;
	*pointer++ = 0x00;
	*pointer++ = onoff;
	pointer = sendBuf;
    pointer+=32;
	for(i=0;i<5;i++)
	{
		chk+=*pointer++;
	}
	//pointer+=5;
	*pointer++ = (chk>>8);
	*pointer++ = (uint8_t)chk;
		I2C_TransferHandling(I2C2, PMS7003I_ADDR, 39, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TXE) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
	for(i=0;i<39;i++)
	{
	 I2C_SendData(I2C2,sendBuf[i]);
		timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TXE) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
}
//	I2C_ClearFlag(I2C2,I2C_ISR_STOPF);
//	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
//	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
//	I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
//	I2C_InitStructure.I2C_DigitalFilter = 0x00;
//	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
//	I2C_InitStructure.I2C_Timing = 0x20D22E37;
//	//I2C_InitStructure.I2C_OwnAddress1 = 0x00;
//	I2C_Cmd(I2C2,DISABLE);
//	I2C_Init(I2C2,&I2C_InitStructure);
//	I2C_Cmd(I2C2,ENABLE);
return 0;
}









