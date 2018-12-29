
#ifndef __USER_GUI_H__
#define  __USER_GUI_H__

#include "GlobalDef.h"

void UserGUI_Circle(u16 X,u16 Y,u16 R,u16 fc); 
void UserGUI_DrawLine(u16 x0, u16 y0,u16 x1, u16 y1,u16 Color);  
void UserGUI_box(u16 x, u16 y, u16 w, u16 h,u16 bc);
void UserGUI_box2(u16 x,u16 y,u16 w,u16 h, u8 mode);
void UserGUI_DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2);
void UserGUI_DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2);


#endif

