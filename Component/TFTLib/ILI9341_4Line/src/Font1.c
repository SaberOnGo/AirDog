
#include "stm32f10x.h"
#include "ILI9341_Drv.h"
#include "Font1.h"
#include "delay.h"
#include "UserFont.h"
#include "ILI9341_4Line_SPI.h"

// X 轴 8 个像素, Y 轴 为 16个像素
void UserGUI_DrawFont_GBK16(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0 = x;

	while(*s) 
	{	
		if((*s) < 128) // 字符范围判断
		{
			k=*s;
			if (k == '\r')  // 0x0D, 回车键
			{
				x = x0;
				y += 16;  // 下一列, 纵座标 + 16
			}
			else 
			{
				if (k > ' ')k -= ' ';  // 0x20 开始后的字符, 0x20 为 空格字符
				else k = 0;

	            // Y 轴像素
			    for(i = 0; i < 16; i++) // i 为该字符取模中的值, 比如一个字符取模 为 16B, 则 i 为 0 - 15
			    {
				    for(j = 0; j < 8; j++)   // X 轴像素 
					{
				    	if(asc16[k * 16 + i]&( 0x80 >> j))	
							UserGUI_DrawPoint(x + j, y + i, fc);  // 前景色
						else 
						{
							if(fc != bc)UserGUI_DrawPoint(x + j, y + i, bc);  // 背景色
						}
					}
			    }
				x += 8;
			}
			s++;
		}
			
		else 
		{
			for (k = 0; k < hz16_num; k++) 
			{
			  if ((hz16[k].Index[0] == *(s)) && (hz16[k].Index[1] == *(s+1)))
			  { 
				    for(i = 0; i < 16; i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2]&(0x80>>j))	
									UserGUI_DrawPoint(x+j,y+i,fc);
								else {
									if (fc!=bc) UserGUI_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz16[k].Msk[i*2+1]&(0x80>>j))	UserGUI_DrawPoint(x+j+8,y+i,fc);
								else 
								{
									if (fc!=bc) UserGUI_DrawPoint(x+j+8,y+i,bc);
								}
							}
				    }
				}
			  }
			s+=2;x+=16;
		} 
		
	}
}

void UserGUI_DrawFont_GBK24(u16 x, u16 y, u16 fc, u16 bc, u8 *s)
{
	unsigned char i,j;
	unsigned short k;

	while(*s) 
	{
		if( *s < 0x80 ) 
		{
			k=*s;
			if (k>32) k-=32; else k=0;

		    for(i=0;i<16;i++)
			for(j=0;j<8;j++) 
				{
			    	if(asc16[k*16+i]&(0x80>>j))	
					UserGUI_DrawPoint(x+j,y+i,fc);
					else 
					{
						if (fc!=bc) UserGUI_DrawPoint(x+j,y+i,bc);
					}
				}
			s++;x+=8;
		}
		else 
		{

			for (k=0;k<hz24_num;k++) 
			{
			  if ((hz24[k].Index[0]==*(s))&&(hz24[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<24;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3]&(0x80>>j))
								UserGUI_DrawPoint(x+j,y+i,fc);
								else 
								{
									if (fc!=bc) UserGUI_DrawPoint(x+j,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+1]&(0x80>>j))	UserGUI_DrawPoint(x+j+8,y+i,fc);
								else {
									if (fc!=bc) UserGUI_DrawPoint(x+j+8,y+i,bc);
								}
							}
						for(j=0;j<8;j++) 
							{
						    	if(hz24[k].Msk[i*3+2]&(0x80>>j))	
								UserGUI_DrawPoint(x+j+16,y+i,fc);
								else 
								{
									if (fc!=bc) UserGUI_DrawPoint(x+j+16,y+i,bc);
								}
							}
				    }
			  }
			}
			s+=2;x+=24;
		}
	}
}

// x轴 32个像素, 4B; Y轴32个像素
void UserGUI_DrawFont_Num32(u16 x, u16 y, u16 fc, u16 bc, u16 num)
{
	unsigned char i,j,k,c;
	//lcd_text_any(x+94+i*42,y+34,32,32,0x7E8,0x0,sz32,knum[i]);
//	w=w/8;

    for(i = 0; i < 32; i++)  // Y 轴像素
	{
		for(j=0;j<4;j++) 
		{
			c=*(sz32+num*32*4+i*4+j); //  i * 4 为第 N列(0 - 31), j 为一列中的第几个字节(0 - 3)
			for (k=0;k<8;k++)	
			{
	
		    	if(c&(0x80>>k))UserGUI_DrawPoint(x + j * 8 + k, y + i, fc);
				else {
					if (fc!=bc) UserGUI_DrawPoint(x+j*8+k,y+i,bc);
				}
			}
		}
	}
}

