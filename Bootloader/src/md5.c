#include <stdio.h> 
#include <stdlib.h> 
//#include <time.h> 
#include <string.h> 

typedef unsigned char *POINTER; 
typedef unsigned short int uint16_t; 
typedef unsigned  int uint32_t; 

#define S11 7 
#define S12 12 
#define S13 17 
#define S14 22 
#define S21 5 
#define S22 9 
#define S23 14 
#define S24 20 
#define S31 4 
#define S32 11 
#define S33 16 
#define S34 23 
#define S41 6 
#define S42 10 
#define S43 15 
#define S44 21 
/*
����bits���Ļ�������ΪʲôҪ64���ֽ��أ���Ϊ�������ܵ���Ϣ��bits����512��������Ϊ448ʱ��
��Ҫ����bits�����ֵΪ512=64*8��
*/
//static unsigned char PADDING[64] = { 
// 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
// 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
//}; 
/*
���������⼸���궨����md5�㷨�涨�ģ����Ƕ���Ϣ����md5���ܶ�Ҫ�������㡣
��˵�о���ĸ��ָ��ٳ���ʱ�����⼸������Ĳ����Ϳ��Զ϶��ǲ����õ�md5
*/
/*F,G,HandIarebasicMD5functions.
*/
#define F(x, y, z) (((x) & (y)) | ((~x) & (z))) 
#define G(x, y, z) (((x) & (z)) | ((y) & (~z))) 
#define H(x, y, z) ((x) ^ (y) ^ (z)) 
#define I(x, y, z) ((y) ^ ((x) | (~z))) 
/*ROTATE_LEFTrotatesxleftnbits.
*/
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n)))) 
/*FF,GG,HH,andIItransformationsforrounds1,2,3,and4.
Rotationisseparatefromadditiontopreventrecomputation.
*/
#define FF(a,b,c,d,x,s,ac) {(a)+=F((b),(c),(d))+(x)+(uint32_t)(ac); (a)=ROTATE_LEFT((a),(s)); (a) += (b);} 
#define GG(a, b, c, d, x, s, ac) { (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); } 
#define HH(a, b, c, d, x, s, ac) { (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); } 
#define II(a, b, c, d, x, s, ac) { (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); (a) = ROTATE_LEFT ((a), (s)); (a) += (b); } 

void Encode(unsigned char *output, uint32_t *input, unsigned int len) 
{ 
 unsigned int i, j; 
 
 for (i = 0, j = 0; j < len; i++, j += 4) { 
 output[j] = (unsigned char)(input[i] & 0xff); 
 output[j+1] = (unsigned char)((input[i] >> 8) & 0xff); 
 output[j+2] = (unsigned char)((input[i] >> 16) & 0xff); 
 output[j+3] = (unsigned char)((input[i] >> 24) & 0xff); 
 } 
} 


static void MD5Transform (uint32_t state[4], unsigned int x[16])
{
	uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
	FF(a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */
	/* Round 2 */
	GG(a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */
	/* Round 3 */
	HH(a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[ 6], S34,   0x4881d05); /* 44 */
	HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */
	/* Round 4 */
	II(a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	/* Zeroize sensitive information. */
}


	uint32_t dataBuff[16];
	uint32_t stateTemp[4];
	uint32_t addr;
	uint16_t i;
	uint16_t length;
	uint16_t lengthMod;
void CheckCodeMd5Result(unsigned char* resultBuff,uint16_t dataLen,uint32_t startAddr)
{

	unsigned char* u8Pointer;
	uint32_t bitNum;
	addr = startAddr;
	stateTemp[0] = 0x67452301;   
	stateTemp[1] = 0xefcdab89;   
	stateTemp[2] = 0x98badcfe;   
	stateTemp[3] = 0x10325476;
	length = dataLen/64;//512bit *dataLen 
	lengthMod = dataLen%64;
	bitNum = dataLen*8;
	for(i=0;i<length;i++)
	{	
		memcpy(dataBuff,(uint32_t*)addr,64);
		MD5Transform(stateTemp,dataBuff);
		addr+=64;
		memset(dataBuff,0,64);
	}
	memcpy(dataBuff,(uint32_t*)addr,lengthMod);
	if(lengthMod != 55)
	{
		u8Pointer = (unsigned char*)dataBuff;
		u8Pointer+=lengthMod;
		*u8Pointer++=0x80;
		if(lengthMod >55)
		{
			MD5Transform(stateTemp,dataBuff);
		memset(dataBuff,0,64);
	  }
		u8Pointer = (unsigned char*)dataBuff;
		u8Pointer +=56;
		memcpy(u8Pointer,&bitNum,4);
		MD5Transform(stateTemp,dataBuff);
	}else
	{
		u8Pointer = (unsigned char*)dataBuff;
		u8Pointer+=lengthMod;
		*u8Pointer++=0x80;
	  memcpy(u8Pointer,&bitNum,4);
		MD5Transform(stateTemp,dataBuff);
	}
	
	Encode(resultBuff,stateTemp,16);
}

