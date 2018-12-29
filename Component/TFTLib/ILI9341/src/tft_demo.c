
#include "tft_dma.h"
#include "tft_text.h"
#include "tft_itoa.h"
#include "tft_encoder.h"
#include "os_global.h"





void TFT_Demo(void)
{
    u32 ov = os_get_tick() / 100;
    //u16 encVal;
	char buf[16];
	
    LCD_Config();
    ENC_init();
    LCD_fillScreen(ORANGE);

    LCD_setTextBgColor(GREEN);
    LCD_setTextSize(3);
	
    while (1)
	{
        if( ov != (os_get_tick() / 100))
		{
		    ov = os_get_tick() / 100;
            LCD_setCursor(0, 0);
            tft_itoa((int)ov, buf, 10);
            LCD_fillRect(0, 0, 50, 24, BLACK);
            LCD_writeString((uint8_t *)buf);
        }
        delay_ms(10);
    }

    //while (1);
}

