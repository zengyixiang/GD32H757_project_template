#include "board.h"

#include "board_clock.h"
#include "board_pin.h"
#include "board_sdram.h"
#include "board_system.h"

void board_hw_version_init(void);

void board_init(void)
{
    board_system_init();
    board_clock_init();
    board_hw_version_init();
    board_pin_init();
    board_sdram_init();
    board_led_init();
    board_key_init();
    board_i2c_init();
    board_adc_init();
    board_uart_init();
    board_eeprom_init();
    board_sensor_init();
}

void board_tick(void)
{
}
