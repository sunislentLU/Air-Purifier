#ifndef _FIFO_H_
#define _FIFO_H_

#define QUEUE_LEN  128

typedef enum
{
 FIFO_NORMAL = 0x00,
 FIFO_EMPTY,
 FIFO_FULL
}_eFIFO_RET;

typedef struct
{
unsigned char queue[QUEUE_LEN];
unsigned short count;
unsigned short front;
unsigned short rear;
}_sFIFO;



#define RETFIFO_LEN  4
typedef struct
{
unsigned char queue[RETFIFO_LEN];
unsigned short count;
unsigned short front;
unsigned short rear;
}_sRETFIFO;


void FifoInit(_sFIFO* fifo);
_eFIFO_RET PushInFifo(_sFIFO * fifo, unsigned char dat);
_eFIFO_RET PopFromFifo(_sFIFO * fifo, unsigned char * pdata);
void RetFifoInit(_sRETFIFO* fifo);
_eFIFO_RET PushInRetFifo(_sRETFIFO* fifo,unsigned char dat);
_eFIFO_RET PopFromRetFifo(_sRETFIFO* fifo,unsigned char* pdata);

#endif

