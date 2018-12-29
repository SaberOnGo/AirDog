#ifndef _GAGENT_MD5_H
#define _GAGENT_MD5_H

#include <stdint.h>


#if 1
#define GIZ_MD5_DBG(fmt, ...)  printf(fmt, ##__VA_ARGS__)
#else
#define GIZ_MD5_DBG(...)
#endif



#pragma pack(1)
typedef struct
{
	unsigned int count[2];
	unsigned int state[4];
	unsigned char buffer[64];
} MD5_CTX;
#pragma pack()

#define F(x,y,z) ((x & y) | (~x & z))
#define G(x,y,z) ((x & z) | (y & ~z))
#define H(x,y,z) (x^y^z)
#define I(x,y,z) (y ^ (x | ~z))
#define ROTATE_LEFT(x,n) ((x << n) | (x >> (32-n)))
#define FF(a,b,c,d,x,s,ac) \
{ \
	a += F(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define GG(a,b,c,d,x,s,ac) \
{ \
	a += G(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define HH(a,b,c,d,x,s,ac) \
{ \
	a += H(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}
#define II(a,b,c,d,x,s,ac) \
{ \
	a += I(b,c,d) + x + ac; \
	a = ROTATE_LEFT(a,s); \
	a += b; \
}

void GAgent_MD5Init(MD5_CTX *context);
void GAgent_MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputlen);

//void GAgent_MD5Final(MD5_CTX *context, unsigned char digest[16]);
void GAgent_MD5Final(MD5_CTX *context, unsigned char *digest);

//void GAgent_MD5Transform(unsigned int state[4], unsigned char block[64]);
void GAgent_MD5Transform(unsigned int * state, unsigned char * block);

void GAgent_MD5Encode(unsigned char *output, unsigned int *input, unsigned int len);

//void GAgent_MD5Decode(unsigned int *output, unsigned char *input, unsigned int len);
void GAgent_MD5Decode(uint32_t * output, uint8_t * input,  uint32_t len);

#endif
