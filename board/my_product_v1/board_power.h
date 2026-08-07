#ifndef BOARD_POWER_H
#define BOARD_POWER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void board_power_init(void);
/* 产品未使用软件电源锁存，保留空实现以统一板级接口。 */
void board_power_early_init(void);
void board_power_sensor_3v3_set(bool on);
void board_power_digital_set(bool on);
void board_power_temp_set(bool on);
void board_power_sensor_all_set(bool on);
void board_power_system_set(bool on);

#ifdef __cplusplus
}
#endif

#endif
