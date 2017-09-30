#include "fifo.h"
#include "stdio.h"
#include "string.h"


void FifoInit(_sFIFO* fifo)
{
 fifo->count = 0;
 memset(fifo->queue,0,QUEUE_LEN);
 fifo->front = fifo->rear = 0;
}

_eFIFO_RET PushInFifo(_sFIFO* fifo,unsigned char dat)
{
	 if((fifo->front == fifo->rear)&&(fifo->count == QUEUE_LEN))
	 	return FIFO_FULL;
        fifo->queue[fifo->front] = dat;
	    fifo->front = (fifo->front+1)%QUEUE_LEN;
	    fifo->count++;
	    return FIFO_NORMAL;
} 

_eFIFO_RET PopFromFifo(_sFIFO* fifo,unsigned char* pdata)
{
     if((fifo->rear == fifo->front)&&(fifo->count == 0))
	 	return FIFO_EMPTY;
	 *pdata = fifo->queue[fifo->rear];
	 fifo->rear = (fifo->rear+1)%QUEUE_LEN;
	 fifo->count -- ;
	 return FIFO_NORMAL;
}



void RetFifoInit(_sRETFIFO* fifo)
{
 fifo->count = 0;
 memset(fifo->queue,0,RETFIFO_LEN);
 fifo->front = fifo->rear = 0;
}

_eFIFO_RET PushInRetFifo(_sRETFIFO* fifo,unsigned char dat)
{
	 if((fifo->front == fifo->rear)&&(fifo->count == RETFIFO_LEN))
	 	return FIFO_FULL;
        fifo->queue[fifo->front] = dat;
	    fifo->front = (fifo->front+1)%RETFIFO_LEN;
	    fifo->count++;
	    return FIFO_NORMAL;
} 

_eFIFO_RET PopFromRetFifo(_sRETFIFO* fifo,unsigned char* pdata)
{
     if((fifo->rear == fifo->front)&&(fifo->count == 0))
	 	return FIFO_EMPTY;
	 *pdata = fifo->queue[fifo->rear];
	 fifo->rear = (fifo->rear+1)%RETFIFO_LEN;
	 fifo->count -- ;
	 return FIFO_NORMAL;
}





