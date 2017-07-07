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
unsigned char length2;//³¤¶È = ÃüÁî + Êý¾Ý + checksum Êý¾Ý³¤¶È  + 2
unsigned char length1;
_eCOMMAND cmd;      // ÃüÁî
p_func   fecthdata;   // »ñÈ¡Êý¾Ý
get_proc getchecksum;// ¼ÆËã³öchecksum
}_sWIFI_CMD_PROC;



// attribute for sending to wifi module
typedef enum
{
DEV_MODE = 0x00, // Ä£Ê½
DEV_DUST,        // »Ò³¾Å¨¶È
DEV_GAS,         // ÆøÌå´«¸ÐÆ÷Öµ
DEV_LUMIN,       // ¹âÕÕÇ¿¶È
DEV_BLUE,        // À¶µÆ×´Ì¬
DEV_SPD,         //µ±Ç°×ªËÙ
DEV_COVER,       //¶¥¸Ç×´Ì¬
DEV_FAULT,       // ¹ÊÕÏÀàÐÍ
DEV_TIMING,      // Éè±¸¶¨Ê±
DEV_USETIME,     // Éè±¸Ê¹ÓÃÊ±¼ä
DEV_FILTER_TIME,  // ÂËÍøÊÙÃü
DEV_FILTER_STATE,

DEV_SPD_REF,     //Ã¿¸öÄ£Ê½µÄ×ªËÙ
DEV_AUTO_REF,    // ×Ô¶¯Ä£Ê½µÄ²Î¿¼×ªËÙ
DEV_DUST_REF,    // ¸÷µµÎ»·Û³¾Å¨¶ÈµÄ²Î¿¼Öµ
DEV_GAS_REF,     // ¸÷¸öµµÎ»ÓÐ»úÆøÌåÅ¨¶È²Î¿¼
DEV_LUMIN_REF,
DEV_FILTER_REF,  // ÂËÍø ÊÙÃü²Î¿¼å
DEV_DUST_SEN     //·Û³¾´«¸ÐÆ÷µÄÁéÃô¶È
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



