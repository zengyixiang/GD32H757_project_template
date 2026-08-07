#ifndef BOARD_POWER_H
#define BOARD_POWER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void board_power_init(void);
/* 尽早锁住 PF15 系统电源，不提前开启其他外围电源。 */
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
