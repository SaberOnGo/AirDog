#include "stm32f10x.h"
#include "ILI9341_Drv.h"
#include "UserGUI.h"
#include "delay.h"
#include "ILI9341_4Line_SPI.h"

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 UserGUI_LCD_BGR2RGB(u16 c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}




void UserGUI_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{//Bresenham算法 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        UserGUI_DrawPoint(X+a,Y+b,fc);     //        7 
        UserGUI_DrawPoint(X-a,Y+b,fc);     //        6 
        UserGUI_DrawPoint(X+a,Y-b,fc);     //        2 
        UserGUI_DrawPoint(X-a,Y-b,fc);     //        3 
        UserGUI_DrawPoint(X+b,Y+a,fc);     //        8 
        UserGUI_DrawPoint(X-b,Y+a,fc);     //        5 
        UserGUI_DrawPoint(X+b,Y-a,fc);     //        1 
        UserGUI_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        UserGUI_DrawPoint(X+a,Y+b,fc); 
        UserGUI_DrawPoint(X+a,Y+b,fc); 
        UserGUI_DrawPoint(X+a,Y-b,fc); 
        UserGUI_DrawPoint(X-a,Y-b,fc); 
        UserGUI_DrawPoint(X+b,Y+a,fc); 
        UserGUI_DrawPoint(X-b,Y+a,fc); 
        UserGUI_DrawPoint(X+b,Y-a,fc); 
        UserGUI_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//画线函数，使用Bresenham 画线算法
void UserGUI_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color)   
{
int dx,             // difference in x's
    dy,             // difference in y's
    dx2,            // dx,dy * 2
    dy2, 
    x_inc,          // amount in pixel space to move during drawing
    y_inc,          // amount in pixel space to move during drawing
    error,          // the discriminant i.e. error i.e. decision variable
    index;          // used for looping	


	UserGUI_SetXY(x0,y0);
	dx = x1-x0;//计算x距离
	dy = y1-y0;//计算y距离

	if (dx>=0)
	{
		x_inc = 1;
	}
	else
	{
		x_inc = -1;
		dx    = -dx;  
	} 
	
	if (dy>=0)
	{
		y_inc = 1;
	} 
	else
	{
		y_inc = -1;
		dy    = -dy; 
	} 

	dx2 = dx << 1;
	dy2 = dy << 1;

	if (dx > dy)//x距离大于y距离，那么每个x轴上只有一个点，每个y轴上有若干个点
	{//且线的点数等于x距离，以x轴递增画点
		// initialize error term
		error = dy2 - dx; 

		// draw the line
		for (index=0; index <= dx; index++)//要画的点数不会超过x距离
		{
			//画点
			UserGUI_DrawPoint(x0,y0,Color);
			
			// test if error has overflowed
			if (error >= 0) //是否需要增加y坐标值
			{
				error-=dx2;

				// move to next line
				y0+=y_inc;//增加y坐标值
			} // end if error overflowed

			// adjust the error term
			error+=dy2;

			// move to the next pixel
			x0+=x_inc;//x坐标值每次画点后都递增1
		} // end for
	} // end if |slope| <= 1
	else//y轴大于x轴，则每个y轴上只有一个点，x轴若干个点
	{//以y轴为递增画点
		// initialize error term
		error = dx2 - dy; 

		// draw the line
		for (index=0; index <= dy; index++)
		{
			// set the pixel
			UserGUI_DrawPoint(x0,y0,Color);

			// test if error overflowed
			if (error >= 0)
			{
				error-=dy2;

				// move to next line
				x0+=x_inc;
			} // end if error overflowed

			// adjust the error term
			error+=dx2;

			// move to the next pixel
			y0+=y_inc;
		} // end for
	} // end else |slope| > 1
}



void UserGUI_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	UserGUI_DrawLine(x,y,x+w,y,0xEF7D);
	UserGUI_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	UserGUI_DrawLine(x,y+h,x+w,y+h,0x2965);
	UserGUI_DrawLine(x,y,x,y+h,0xEF7D);
    UserGUI_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
}
void UserGUI_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		UserGUI_DrawLine(x,y,x+w,y,0xEF7D);
		UserGUI_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		UserGUI_DrawLine(x,y+h,x+w,y+h,0x2965);
		UserGUI_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		UserGUI_DrawLine(x,y,x+w,y,0x2965);
		UserGUI_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		UserGUI_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		UserGUI_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		UserGUI_DrawLine(x,y,x+w,y,0xffff);
		UserGUI_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		UserGUI_DrawLine(x,y+h,x+w,y+h,0xffff);
		UserGUI_DrawLine(x,y,x,y+h,0xffff);
	}
}


/**************************************************************************************
功能描述: 在屏幕显示一凸起的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void UserGUI_DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	UserGUI_DrawLine(x1,  y1,  x2,y1,    GRAY2);  //H
	UserGUI_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
	UserGUI_DrawLine(x1,  y1,  x1,y2,    GRAY2);  //V
	UserGUI_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
	UserGUI_DrawLine(x1,  y2,  x2,y2,    WHITE);  //H
	UserGUI_DrawLine(x2,  y1,  x2,y2,    WHITE);  //V
}

/**************************************************************************************
功能描述: 在屏幕显示一凹下的按钮框
输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
输    出: 无
**************************************************************************************/
void UserGUI_DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
	UserGUI_DrawLine(x1,  y1,  x2,y1, WHITE); //H
	UserGUI_DrawLine(x1,  y1,  x1,y2, WHITE); //V
	
	UserGUI_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
	UserGUI_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
	UserGUI_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    UserGUI_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}




