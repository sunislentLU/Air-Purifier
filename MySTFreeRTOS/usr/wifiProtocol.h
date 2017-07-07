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
unsigned char length2;//���� = ���� + ���� + checksum ���ݳ���  + 2
unsigned char length1;
_eCOMMAND cmd;      // ����
p_func   fecthdata;   // ��ȡ����
get_proc getchecksum;// �����checksum
}_sWIFI_CMD_PROC;



// attribute for sending to wifi module
typedef enum
{
DEV_MODE = 0x00, // ģʽ
DEV_DUST,        // �ҳ�Ũ��
DEV_GAS,         // ���崫����ֵ
DEV_LUMIN,       // ����ǿ��
DEV_BLUE,        // ����״̬
DEV_SPD,         //��ǰת��
DEV_COVER,       //����״̬
DEV_FAULT,       // ��������
DEV_TIMING,      // �豸��ʱ
DEV_USETIME,     // �豸ʹ��ʱ��
DEV_FILTER_TIME,  // ��������
DEV_FILTER_STATE,

DEV_SPD_REF,     //ÿ��ģʽ��ת��
DEV_AUTO_REF,    // �Զ�ģʽ�Ĳο�ת��
DEV_DUST_REF,    // ����λ�۳�Ũ�ȵĲο�ֵ
DEV_GAS_REF,     // ������λ�л�����Ũ�Ȳο�
DEV_LUMIN_REF,
DEV_FILTER_REF,  // ���� �����ο��
DEV_DUST_SEN     //�۳���������������
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



