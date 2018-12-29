#ifndef TEST1_CONFIG_H
#define TEST1_CONFIG_H

// <editor-fold desc="Defaults">

#include "ILI9341_4Line_SPI.h"





#define SPI_MASTER                   SPI3
#define SPI_MASTER_CLK               RCC_APB1Periph_SPI3
#define SPI_MASTER_GPIO              GPIOB
#define SPI_MASTER_GPIO_CLK          RCC_APB2Periph_GPIOB
#define SPI_MASTER_PIN_SCK             GPIO_Pin_3
#define SPI_MASTER_PIN_MISO          GPIO_Pin_4
#define SPI_MASTER_PIN_MOSI          GPIO_Pin_5

// </editor-fold>

// <editor-fold desc="Colors">



// </editor-fold>

#endif //TEST1_CONFIG_H

