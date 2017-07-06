#include "pms7003i.h"


uint8_t GetParticalDensity(uint16_t*pm25,uint16_t* pm100)
{
	uint16_t timeout;
	uint8_t regAddr;
	I2C_TransferHandling(I2C2, PMS7003I_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TXIS) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
	regAddr = PM25AIRDATAH_ADDR;
	I2C_SendData(I2C2,regAddr);
		timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_TC) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
	I2C_TransferHandling(I2C2, PMS7003I_ADDR, 4, I2C_SoftEnd_Mode, I2C_Generate_Start_Read);
	timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
	*pm25 = I2C_ReceiveData(I2C2);
	(*pm25)<<=8;
	timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}	
	(*pm25)|= I2C_ReceiveData(I2C2);
	timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}
	*pm100 = I2C_ReceiveData(I2C2);
	(*pm100)<<=8;
	timeout = PMS7003_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
		if((timeout--) == 0)
			return 1;
	}	
	(*pm100)|= I2C_ReceiveData(I2C2);	

return 0;
}







