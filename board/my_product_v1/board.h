#ifndef BOARD_H
#define BOARD_H

#include "board_adc.h"
#include "board_display.h"
#include "board_eeprom.h"
#include "board_hw_version.h"
#include "board_i2c.h"
#include "board_key.h"
#include "board_led.h"
#include "board_sensor.h"
#include "board_touch.h"
#include "board_uart.h"

void board_init(void);
void board_tick(void);

#endif
