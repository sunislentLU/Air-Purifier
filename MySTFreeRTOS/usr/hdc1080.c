#include "hdc1080.h"
#include "bspInput.h"
extern 	void vTaskDelay( const uint32_t xTicksToDelay );
uint8_t GetTempHumi(uint16_t* temp,uint16_t* hum)
{
	
	uint16_t timeout;
	uint8_t regAddr;
	uint16_t config;
	regAddr = HDC1080_ADDR;
   /* Configure slave address, nbytes, reload and generate start */
  I2C_TransferHandling(I2C2, regAddr, 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  /* Wait until TXIS flag is set */
  timeout = HDC1080_TIMEOUT;
  while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET)
  {
    if((timeout--) == 0) return RET_TIMEOUT;
  }
	regAddr = HDC1080_CONGIG_ADDR;
	I2C_SendData(I2C2,regAddr); 
	timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET)
  {
    if((timeout--) == 0) return RET_TIMEOUT;
  }
	config = HDC1080_CONFIG;
	I2C_TransferHandling(I2C2, HDC1080_ADDR, 2, I2C_SoftEnd_Mode, I2C_No_StartStop);
	while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET)
	{
		if((timeout--) == 0) return RET_TIMEOUT;
	}
	timeout = HDC1080_TIMEOUT;
	I2C_SendData(I2C2,(uint8_t)(config>>8));
	while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET)
	{
		if((timeout--) == 0) return RET_TIMEOUT;
	}
	I2C_SendData(I2C2,(uint8_t)config);
	timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2, I2C_ISR_TC) == RESET)
	{
		if((timeout--) == 0) return RET_TIMEOUT;
	}
	timeout = HDC1080_TIMEOUT;
	I2C_TransferHandling(I2C2, HDC1080_ADDR, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
  while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET)
  {
    if((timeout--) == 0) return RET_TIMEOUT;
  }
	regAddr = 0x00;// send start measure
	I2C_SendData(I2C2,regAddr); 
  timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2, I2C_ISR_TC) == RESET)
	{
		if((timeout--) == 0) return RET_TIMEOUT;
	}
	timeout = HDC1080_TIMEOUT;	
			vTaskDelay(14);//return RET_TIMEOUT;
	I2C_TransferHandling(I2C2, HDC1080_ADDR, 4, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);

	timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
	  if((timeout--) == 0) 
			return RET_TIMEOUT;
	}
	*temp = I2C_ReceiveData(I2C2);
	(*temp)<<=8;
	timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
	  if((timeout--) == 0) return RET_TIMEOUT;
	}
	*temp |= I2C_ReceiveData(I2C2);
		timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
	  if((timeout--) == 0) return RET_TIMEOUT;
	}
	*hum = I2C_ReceiveData(I2C2);
	(*hum)<<=8;
	timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_RXNE) == RESET)
	{
	  if((timeout--) == 0) return RET_TIMEOUT;
	}
	*hum |= I2C_ReceiveData(I2C2);
	timeout = HDC1080_TIMEOUT;
	while(I2C_GetFlagStatus(I2C2,I2C_ISR_STOPF) == RESET)
	{
		if((timeout--) == 0) return RET_TIMEOUT;	
	}
	I2C_ClearFlag(I2C2,I2C_ISR_STOPF);
return RET_OK;
}




void I2CSensorGenerateStop(void)
{

	I2C_GenerateSTOP(I2C2,ENABLE);
	I2C_Cmd(I2C2,DISABLE);
	SensorI2CGpio_Init();
}










