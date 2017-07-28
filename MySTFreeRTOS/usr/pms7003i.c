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
	I2C_TransferHandling(I2C2, PMS7003I_ADDR, 16, I2C_SoftEnd_Mode, I2C_Generate_Start_Read);
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







