#include "key.h"
#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"



uint8_t  GetIOstatus(void)
{
uint8_t  key_value=0;//,IO_temp=0,temp=0;
if(GPIO_ReadInputDataBit(KEY_POWER_PORT,KEY_POWER_PIN) == 0)
	key_value = 1;
if(GPIO_ReadInputDataBit(KEY_MODE_PORT,KEY_MODE_PIN) == 0)
	key_value = 2;
if(GPIO_ReadInputDataBit(KEY_VOLUME_PORT,KEY_VOLUME_PIN) == 0)
	key_value = 3;
if(GPIO_ReadInputDataBit(KEY_TIMING_PORT,KEY_TIMING_PIN) == 0)
	key_value = 4;
if(GPIO_ReadInputDataBit(KEY_LED_PORT,KEY_LED_PIN) == 0)
	key_value = 5;
return key_value;
}



uint16_t  KeyScan(void)
{
static uint8_t  debounce_cnt=0,key_cnt =0,key_cnt2=0;
static uint8_t key_is_down=0;
static uint8_t key_status=KEY_STATE_IDLE;
static uint16_t  key_temp2=0;
//static  uint8_t is_long_hold=0;
uint8_t  key_temp1=0;
uint16_t  key_event=0;
key_temp1 = GetIOstatus();
switch(key_status)
{
case KEY_STATE_IDLE:
if(key_is_down)
{
	key_event = key_is_down;
	key_event<<=8;
	key_event|=key_temp2;
	key_temp2=0;
	key_is_down=0x00;
	return  key_event;
}
if(key_temp1 ==0x00)
{
	debounce_cnt = 0;
	return 0;
}
key_status = KEY_STATE_DEBOUNCE;
key_temp2 =key_temp1;
break;	
case KEY_STATE_DEBOUNCE:
if(key_temp1 !=key_temp2 )
{
   debounce_cnt = 0;
   key_status = KEY_STATE_IDLE;
   break;
}
debounce_cnt++;
if(debounce_cnt==4)
{
	debounce_cnt = 0;
	key_status = KEY_STATE_DOWN;
}
break;
case KEY_STATE_DOWN:
if(key_temp1 ==0 )
{
	key_is_down = KEY_TYPE_PRESS;
	key_status = KEY_STATE_IDLE;
	 key_cnt=0;
    break;
}
key_temp2 =key_temp1;
key_cnt++;
if(key_cnt==100)
{
    key_cnt=0;
    key_status = KEY_STATE_HOLD;   
}
break;
case KEY_STATE_HOLD:

  if(key_temp1 ==0 )
//  if(key_temp1 !=key_temp2 )
{
	if(key_is_down == KEY_TYPE_HOLD)
		key_is_down = 0;
	else
	key_is_down = KEY_TYPE_LONG_PRESS;
	key_cnt = 0;
	key_is_down = 0;
	key_status = KEY_STATE_IDLE;
    break;
}
key_cnt++;
if(key_cnt==200)
{
	key_cnt=0;
	key_is_down = KEY_TYPE_HOLD;
	key_event = key_is_down;
	key_event<<=8;
	key_event|=key_temp2;
	//key_is_down = 0;
	key_cnt2++;
//if(key_cnt2>=3)
//{
//   key_cnt2 = 0;
//   key_status = KEY_STATE_LONG_HOLD;  
//}
return  key_event;
}
break;


//case KEY_STATE_LONG_HOLD:
//if(key_temp1 ==0 )
//// if(key_temp1 !=key_temp2 )
//{
//	key_cnt = 0;
//	key_status = KEY_STATE_IDLE;
//	if(is_long_hold)
//	{
//	key_is_down= KEY_TYPE_LONG_LONG_PRESS;
//	}
//    is_long_hold = 0;
//   break;
// }
//  key_cnt++;
// if(key_cnt==150)
// {
//   key_is_down = KEY_TYPE_LONG_HOLD;
//	 key_event = key_is_down;
//	 key_event<<=8;
//	 key_event|=key_temp2;
//	 key_is_down = 0;
//	 key_status = KEY_STATE_LONG_HOLD;
//	 return  key_event;
// }
//if(key_cnt==200)
//{
//	key_cnt=0;
//	is_long_hold = 1;
//}
//  
//  break;
default : break;
}
return 0;	
}

uint8_t GetSwitchIO(void)
{
    if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11) == 0)
		return 1;
	return 0;
}


typedef enum
{
SW_STATE_IDLE = 0x00,
SW_STATE_DEBOUNCE,
SW_STATE_DOWN
}_eSW_STATE;


uint8_t SwitchScan(void)
{
static uint8_t swCnt = 0;
static uint8_t swIOTemp = 0;
static _eSW_STATE state = SW_STATE_IDLE;
uint8_t swIO;
swIO = GetSwitchIO();
switch(state)
{
  case SW_STATE_IDLE:
	if(swIOTemp != swIO) 		
	{				
		swIOTemp = swIO;			
		state = SW_STATE_DEBOUNCE;		
    swCnt = 0;		
		break;	
	}
  	break;
  case SW_STATE_DEBOUNCE:
	if(swIOTemp == swIO)
	{              
		swCnt++;      
		if(swCnt == 10)       	
		{       	     
			state = SW_STATE_DOWN;		   
      swCnt = 0;		  
		}
	}
	else
	{
		swCnt = 0;
		state = SW_STATE_IDLE;
	}
  	break;
 case SW_STATE_DOWN:
	 if(swIO == 0)
	 {
		 state = SW_STATE_IDLE;
		 return 1;
	 }
	 else 
	 {
		 state = SW_STATE_IDLE;
		 return 2;
	 }
 default:
 	break;
}
return 0;

}
