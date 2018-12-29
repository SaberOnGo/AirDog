
#include "sfud_demo.h"
#include "GlobalDef.h"

#include <bsp.h>
#include <sfud.h>
#include <stdio.h>
#include <stdlib.h>

#define SFUD_DEMO_TEST_BUFFER_SIZE                     1024

#ifdef SFUD_DEBUG_MODE
static void sfud_demo(uint32_t addr, size_t size, uint8_t *data);

static uint8_t sfud_demo_test_buf[SFUD_DEMO_TEST_BUFFER_SIZE];
#endif

void sfud_demo_task(void)
{
#ifdef SFUD_DEBUG_MODE
    bsp_init();

    /* SFUD initialize */
    if (sfud_init() == SFUD_SUCCESS) {
        sfud_demo(0, sizeof(sfud_demo_test_buf), sfud_demo_test_buf);
    }

    #if 0
    while (1) {
        LED_RUN_ON;
        delay(6000000);
        LED_RUN_OFF;
        delay(6000000);
    }
	#endif
	
#endif
}

/**
 * SFUD demo for the first flash device test.
 *
 * @param addr flash start address
 * @param size test flash size
 * @param size test flash data buffer
 */
#ifdef SFUD_DEBUG_MODE
void flash_reset(void)
{
   const sfud_flash *flash = sfud_get_device_table() + 0;
   
   sfud_chip_erase(flash);
}

static void sfud_demo(uint32_t addr, size_t size, uint8_t *data) {

    sfud_err result = SFUD_SUCCESS;
    const sfud_flash *flash = sfud_get_device_table() + 0;
    size_t i;
    /* prepare write data */
    for (i = 0; i < size; i++) {
        data[i] = i;
    }
    /* erase test */
    result = sfud_erase(flash, addr, size);
    if (result == SFUD_SUCCESS) {
        os_printf("Erase the %s flash data finish. Start from 0x%08X, size is %ld.\r\n", flash->name, addr,
                size);
    } else {
        os_printf("Erase the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* write test */
    result = sfud_write(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        os_printf("Write the %s flash data finish. Start from 0x%08X, size is %ld.\r\n", flash->name, addr,
                size);
    } else {
        os_printf("Write the %s flash data failed.\r\n", flash->name);
        return;
    }
    /* read test */
    result = sfud_read(flash, addr, size, data);
    if (result == SFUD_SUCCESS) {
        os_printf("Read the %s flash data success. Start from 0x%08X, size is %ld. The data is:\r\n", flash->name, addr,
                size);
        os_printf("Offset (h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\r\n");
        for (i = 0; i < size; i++) {
            if (i % 16 == 0) {
                os_printf("[%08X] ", addr + i);
            }
            os_printf("%02X ", data[i]);
            if (((i + 1) % 16 == 0) || i == size - 1) {
                os_printf("\r\n");
            }
        }
        os_printf("\r\n");
    } else {
        os_printf("Read the %s flash data failed.\r\n", flash->name);
    }
    /* data check */
    for (i = 0; i < size; i++) {
        if (data[i] != i % 256) {
            os_printf("Read and check write data has an error. Write the %s flash data failed.\r\n", flash->name);
			break;
        }
    }
	
	flash_reset();
	
    if (i == size) {
        os_printf("The %s flash test is success.\r\n", flash->name);
    }
}
#endif

