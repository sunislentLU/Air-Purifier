#ifndef _WIFIPROTOCOL_H_
#define _WIFIPROTOCOL_H_

#define HEAD   0xFF   // frame head
#define END    0xFE  // frame end
#define MAX_SEND_SIZE  64
#define WIFI_MAX_BUF_LEN    128
#define WIFI_MAX_SEND_BUFF  128
#define DATA_HEAD_SIZE    0x03



typedef enum
{
 CMD_GET_MAC = 0x00,
 CMD_GET_NET,
 CMD_GET_PID_KEY,
 CMD_SET_PID_KEY,
 CMD_SET_CONNECT,
 CMD_REBOOT_WIFI,
 CMD_RESET_WIFI,
 CMD_GET_FIREWARE,
 CMD_GET_TIME,
 CMD_SET_VISIBLE,

 CMD_ASK_UPDATE = 0x30,// mcu  inquiry  update infomation via wifi module
 CMD_REQ_UPDATE,       //MCU requird wifi update mcu
 CMD_UPDATE_DATA,     //wifi transmit firmware data to mcu
 CMD_UPDATE_FINISH,

 
 CMD_REC_DATA = 0x80,
 CMD_SND_DATA,
 CMD_REC_TERM_DATA,
 CMD_SND_TERM_DATA,
 CMD_SND_ALL_TERM_DATA,
 CMD_SND_SEV_TERM_DATA,
}_eCOMMAND;


typedef struct
{
unsigned char  head;
unsigned short length;
_eCOMMAND cmd;
unsigned char data[WIFI_MAX_BUF_LEN];
unsigned char checksum;//cmd + length + sn +ip +data
unsigned char end;
}_sWIFI_FORMAT;

typedef void (*p_func)(void); 
typedef unsigned char  (*get_proc)(unsigned char x );



typedef struct
{
unsigned char length2;//长度 = 命令 + 数据 + checksum 数据长度  + 2
unsigned char length1;
_eCOMMAND cmd;      // 命令
p_func   fecthdata;   // 获取数据
get_proc getchecksum;// 计算出checksum
}_sWIFI_CMD_PROC;



// attribute for sending to wifi module
typedef enum
{
DEV_MODE = 0x00, // 模式
DEV_DUST,        // 灰尘浓度
DEV_GAS,         // 气体传感器值
DEV_LUMIN,       // 光照强度
DEV_BLUE,        // 蓝灯状态
DEV_SPD,         //当前转速
DEV_COVER,       //顶盖状态
DEV_FAULT,       // 故障类型
DEV_TIMING,      // 设备定时
DEV_USETIME,     // 设备使用时间
DEV_FILTER_TIME,  // 滤网寿命
DEV_FILTER_STATE,

DEV_SPD_REF,     //每个模式的转速
DEV_AUTO_REF,    // 自动模式的参考转速
DEV_DUST_REF,    // 各档位粉尘浓度的参考值
DEV_GAS_REF,     // 各个档位有机气体浓度参考
DEV_LUMIN_REF,
DEV_FILTER_REF,  // 滤网 寿命参考�
DEV_DUST_SEN     //粉尘传感器的灵敏度
}_eDEV_STATE;

typedef struct
{
_eDEV_STATE devState;
unsigned char length;
unsigned char* value;
}_sSTATE_FRAME;

typedef enum
{
  WIFI_UART_HEAD = 0x00,
  WIFI_UART_LEN,
  WIFI_UART_CMD,
  WIFI_UART_DATA,
  WIFI_UART_CHKSUM,
  WIFI_UART_END
}_eWIFI_UART_STATE;


typedef enum
{
NET_OFFLINE = 0x00,
NET_LAN,
NET_CLOUD = 0x03,
NET_STARTUP
}_eWIFI_NET_STATE;


#endif



