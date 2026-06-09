# GD32H757 CMake 工程模板

这是一个按中大型嵌入式工程习惯组织的 GD32H757 CMake 模板。目录结构参考常见 SDK/产品工程，把芯片驱动、启动文件、板级代码、中间件、自研组件、业务服务和应用入口分开。

## 构建

```powershell
cmake --preset Debug
cmake --build --preset Debug

cmake --preset Release
cmake --build --preset Release
```

默认板卡是 `gd32h757_eval`。如果要临时切换到另一块板：

```powershell
cmake --preset Debug -DPROJECT_BOARD=my_product_v1
cmake --build --preset Debug
```

切回默认评估板：

```powershell
cmake --preset Debug -DPROJECT_BOARD=gd32h757_eval
cmake --build --preset Debug
```

## 分层关系

```text
app
  -> services
  -> board/gd32h757_eval
  -> middleware
  -> components
  -> osal

services
  -> board/gd32h757_eval
  -> components
  -> middleware

board/gd32h757_eval
  -> bsp
  -> osal

bsp
  -> drivers

startup
  -> osal

drivers
  -> CMSIS / GD32 StdPeriph
```

依赖尽量保持单向。底层不要调用上层；如果两个模块互相调用，优先考虑抽公共接口、callback、事件队列或让 `services/app_event` 做解耦。

## 顶层目录

| 路径 | 作用 | 放什么 |
| --- | --- | --- |
| `cmake/` | CMake 工程配置 | 工具链、芯片参数、公共 flags、烧录配置 |
| `assets/` | 原始资源文件 | 音频、视频、HTML/CSS/JS、UI 图片、字体、图标等未转换资源 |
| `board/` | 多板卡选择入口 | `PROJECT_BOARD` 选择具体板卡，并导出统一 `project_board` target |
| `board/gd32h757_eval/` | GD32H757 Eval 板适配 | 这块板子的时钟、引脚、LED、按键、ADC、UART、SDRAM |
| `board/my_product_v1/` | 自定义产品板示例 | 示例产品板的 LED、UART、I2C、ADC、显示、触摸映射 |
| `bsp/` | 通用板级外设抽象 | LED、KEY、UART、SPI、I2C、ADC、TIMER、USB 的通用驱动封装 |
| `drivers/` | 芯片厂商驱动 | GD32 标准外设库、CMSIS Core、GD32 device 头文件、USBHS 底层 driver |
| `startup/` | 启动和系统入口 | startup 汇编、`system_gd32h7xx.c`、中断入口、syscalls |
| `linker/` | 链接脚本 | Flash/SDRAM 链接脚本、公共 section 片段 |
| `middleware/` | 第三方中间件 | FreeRTOS、FatFs、lwIP、LVGL、USB device/host/common、MicroPython、cJSON |
| `components/` | 自研通用组件 | ringbuffer、protocol、shell、logger/debug、cli、storage、hardware_version |
| `services/` | 业务服务层 | 通信服务、传感器服务、显示服务、升级服务、应用事件 |
| `app/` | 应用入口 | `main()`、应用初始化、任务创建、版本、配置 |
| `osal/` | 操作系统抽象 | 裸机/FreeRTOS 的 delay、tick、临界区接口 |
| `config/` | 配置头文件 | `FreeRTOSConfig.h`、`lv_conf.h`、`ffconf.h`、`lwipopts.h`、项目配置 |
| `scripts/` | 构建/烧录/调试脚本 | OpenOCD、J-Link、GDB、PowerShell 构建脚本 |
| `tools/` | 辅助工具 | 资源转换、bin 转数组等脚本 |
| `docs/` | 文档 | 架构、构建、调试说明、文档图片和示例 |
| `build/` | 构建输出 | CMake 生成文件和 `.elf/.hex/.bin`，不提交 |

## CMake 组织

| 文件 | 作用 |
| --- | --- |
| `CMakeLists.txt` | 顶层工程入口，创建最终 `C8_GCC.elf` 并链接各层 target |
| `CMakePresets.json` | Debug/Release preset，记录 generator、build 目录、toolchain 文件 |
| `cmake/arm-none-eabi-gcc.cmake` | ARM GCC 交叉编译工具链 |
| `cmake/common_flags.cmake` | Debug/Release 公共编译定义 |
| `cmake/flash_jlink.cmake` | 烧录脚本路径变量和 `flash_openocd` target |

`CMakePresets.json` 是 CMake 官方推荐的现代配置方式之一。很多老项目没有使用，是因为创建时间较早、已有 IDE 脚本或团队习惯不同。

`cmake/common_flags.cmake` 当前被顶层 `CMakeLists.txt` 通过 `include(cmake/common_flags.cmake)` 引入，用来按构建类型自动添加公共编译宏：Debug 配置定义 `DEBUG`，Release 配置定义 `NDEBUG`。如果以后有全工程通用的 warning 选项、优化开关或诊断宏，也可以集中放在这里。

`cmake/flash_jlink.cmake` 当前也被顶层 `CMakeLists.txt` 引入，它定义了 `gd32_add_flash_targets()` 函数。顶层最后调用 `gd32_add_flash_targets(${CMAKE_PROJECT_NAME})` 后，CMake 会生成一个 `flash_openocd` target。固件构建完成后，可以这样烧录：

```powershell
cmake --build --preset Debug --target flash_openocd
```

这个 target 内部会调用 `openocd`，并使用 `scripts/jlink-swd.cfg` 和 `scripts/flash_openocd.cfg`。

原来的 `cmake/gd32h757.cmake` 已删除。它里面真正被使用的只有 `GD32H7XX` 和 `USE_STDPERIPH_DRIVER` 两个编译宏，现在已经移动到 `drivers/CMakeLists.txt`，其余芯片名/内核/FPU 变量没有被使用，保留文件反而会误导。

`config/project_config.h` 里的 `PROJECT_USE_FREERTOS` 用来选择 OSAL 后端：

| 值 | 作用 |
| --- | --- |
| `0` | 使用裸机 SysTick、主循环轮询、PRIMASK 临界区 |
| `1` | 使用 FreeRTOS 后端；接入真实 FreeRTOS 后在 `osal_freertos.c` 中对接 `xTaskCreate`、`vTaskDelay`、`vTaskStartScheduler` |

这个宏属于工程配置，不放在 preset 里，也不从 CMake 命令行覆盖。具体工程是否使用 RTOS 只由 `config/project_config.h` 决定；需要切换裸机/RTOS 时，直接修改该配置头文件。

## Board 层

`board/gd32h757_eval` 表示一块具体板子。

`board/CMakeLists.txt` 负责选择当前板卡。每块板提供同一套公开接口，例如 `board_init()`、`board_uart_write_buffer()`、`board_adc_read()`、`board_display_init()`。上层只链接 `project_board`，不直接关心当前编译的是哪块板。

当前示例板：

| 板卡名 | CMake target | 说明 |
| --- | --- | --- |
| `gd32h757_eval` | `board_gd32h757_eval` | 默认评估板 |
| `my_product_v1` | `board_my_product_v1` | 自定义产品板示例 |

两块板可以有不同硬件映射。例如默认板用 `USART0` 做 debug UART，`my_product_v1` 用 `USART1`；默认板用 `I2C0/I2C1` 连接 EEPROM/传感器，`my_product_v1` 用 `I2C2/I2C3`。

### 硬件版本识别

同一个产品板如果通过电阻分压/ADC 识别硬件版本号，放在同一个 board 目录里做运行时识别，不再新建 CMake 板卡。

硬件版本识别分两层：

| 层 | 职责 |
| --- | --- |
| `components/hardware_version/` | 通用识别算法，固定按 `0~3000mV`、每 `100mV` 一档输出版本码 `1~30` |
| `board/my_product_v1/board_hw_version.c` | 具体板子的适配，只配置 ADC2 channel 12、ADC raw 到 mV 的转换、不同版本码下的板级映射 |

`board/my_product_v1/board_hw_version.c` 当前示例通过 ADC2 channel 12 读取硬件版本 ADC，将 12bit ADC raw 按 `3000mV` 参考电压换算成 mV，再交给 `components/hardware_version` 分类：

| 电压范围 | 版本码 |
| --- | --- |
| `0..99mV` | `1` |
| `100..199mV` | `2` |
| `...` | `...` |
| `2900..3000mV` | `30` |
| `>= 3000mV` | 钳位为 `30` |

然后板级映射按版本码选择，下面只是示例：

| 功能 | 版本码 `1` | 版本码 `2` |
| --- | --- | --- |
| 状态 LED | `PA2` | `PA3` |
| GXHT30 地址 | `0x44` | `0x45` |

这种差异属于“同一块板的硬件版本码差异”，适合在 `board/my_product_v1` 里保留硬件连接差异；通用版本码算法不要写进 board。如果 PCB、外设资源、产品形态已经明显不同，再新建 `board/<new_board>/`。

| 文件 | 作用 |
| --- | --- |
| `board.c/.h` | 板级总初始化入口 |
| `board_system.c/.h` | Cache、NVIC 优先级组、MPU 等芯片/板级系统初始化 |
| `board_clock.c/.h` | 具体板子的时钟树、HXTAL、PLL、外设时钟策略 |
| `board_pin.c/.h` | 引脚复用和板级 pinmux |
| `board_sdram.c/.h` | SDRAM 初始化占位 |
| `board_hw_version.c/.h` | 硬件版本统一接口，对外暴露 `board_hw_version_get()` 和 `board_hw_version_is_valid()`；没有版本识别电路的板子返回 invalid/0 |
| `board_led.c/.h` | 板载 LED 映射，持有 `bsp_led_t` 实例 |
| `board_key.c/.h` | 板载按键映射 |
| `board_i2c.c/.h` | 板载 I2C 总线映射，例如 EEPROM 用 I2C0、传感器用 I2C1 |
| `board_adc.c/.h` | 板载 ADC 用途映射，例如 `BOARD_ADC_VBAT`、`BOARD_ADC_TEMP` |
| `board_uart.c/.h` | 板载调试 UART 映射，logger 输出走这里 |
| `board_display.c/.h` | 板载显示屏初始化和 flush 回调 |
| `board_touch.c/.h` | 板载触摸初始化和读取回调 |
| `board_eeprom.c/.h` | 板载 24C02 实例，绑定 AT24C02 驱动和 I2C 总线 |
| `board_sensor.c/.h` | 板载 GXHT30 实例，绑定 GXHT30 驱动和 I2C 总线 |

规则：`board` 写“这块板子上有什么、接在哪”，不要写通用外设驱动细节。

`board/` 下面再建 `gd32h757_eval/` 是为了支持多块板子共用同一套 `bsp`、`drivers`、`middleware`。如果项目永远只有一块板，也可以把 `board/gd32h757_eval` 扁平化成 `board/`；模板保留这一层是为了移植更方便。

## BSP 层

`bsp` 表示通用板级外设抽象。它不知道业务，也尽量不知道具体板子的用途。

| 子目录 | 作用 |
| --- | --- |
| `bsp/led/` | 通用 LED/GPIO 输出封装 |
| `bsp/key/` | 通用按键封装 |
| `bsp/uart/` | 通用 UART 封装，支持按 buffer 输出 |
| `bsp/spi/` | SPI 占位 |
| `bsp/i2c/` | I2C 多实例封装，提供普通读写和寄存器/存储器读写接口 |
| `bsp/adc/` | ADC 多实例封装 |
| `bsp/timer/` | Timer 占位 |
| `bsp/usb/` | USB 占位 |

同一种外设推荐使用“配置结构体 + 句柄结构体”，例如 `bsp_adc_config_t` + `bsp_adc_t`，这样可以支持多个 ADC、多个 UART、多个 LED。

## Middleware 层

`middleware` 放第三方中间件或第三方库的 port。

| 子目录 | 当前内容 |
| --- | --- |
| `freertos/` | FreeRTOS 占位封装，当前 delay 走 `osal_delay_ms()` |
| `fatfs/` | FatFs 占位 |
| `lwip/` | lwIP port 占位 |
| `lvgl/` | LVGL port 占位 |
| `usb_common/` | USB 公共协议定义占位，官方 USBHS ustd 暂存于此 |
| `usb_device/` | USB device 协议栈占位，官方 USBHS device stack 暂存于此 |
| `usb_host/` | USB host 协议栈占位，官方 USBHS host stack 暂存于此 |
| `micropython/` | MicroPython port 占位 |
| `cjson/` | cJSON 占位 |

`middleware/lvgl/lvgl/` 放 LVGL 官方源码，当前只有占位 stub；`middleware/lvgl/port/` 放 `lv_port_disp`、`lv_port_indev` 这类显示和输入 port；`middleware/lvgl/lvgl_port.c` 是本工程对 LVGL 的统一初始化和刷新入口。

LVGL port 不直接 include `bsp` 或 `board`。显示屏和触摸由 `display_service` 把 `board_display`、`board_touch` 的回调注册给 `lvgl_port`，这样依赖方向保持为 `services -> board + middleware`。

真实 FreeRTOS 接入后，通常由 FreeRTOS port 接管 SysTick。那时 `osal` 切换到 FreeRTOS 后端，App 不需要大改。

## Components 层

`components` 放自研、平台无关的通用组件。

| 子目录 | 当前内容 |
| --- | --- |
| `ringbuffer/` | 环形缓冲区接口占位 |
| `protocol/` | 协议编解码/轮询占位 |
| `shell/` | shell 占位 |
| `logger/` | 你的 `debug` 模块和一个 `log` 包装层 |
| `cli/` | CLI 占位 |
| `storage/` | 存储抽象占位 |
| `hardware_version/` | 通用硬件版本识别组件，板级代码提供 mV 采样回调 |
| `devices/eeprom/at24c02/` | 24C02 通用器件驱动，不绑定具体 I2C 外设 |
| `devices/sensors/gxht30/` | GXHT30 通用器件驱动，不绑定具体板子连接 |

`components/logger/debug.c/.h` 来自你的 `D:\code\ChipDriver\debug` 模块。平台适配在 `app/app_debug.c`，所以 debug 核心仍然保持可移植。

`components/hardware_version/hardware_version.c/.h` 来自你的 `D:\code\ChipDriver\hardware_version` 模块。当前接口只保留初始化、检测、版本码读取和有效性判断。

外部芯片驱动推荐放在 `components/devices/<category>/<chip>/`。这类驱动只写芯片协议，例如 24C02 的页写、随机读，GXHT30 的测量命令和数据换算；不要在这里写 `I2C0`、GPIO 引脚、具体板子的地址配置。

具体板子上的连接关系放在 `board/gd32h757_eval`。例如当前示例里，`board_eeprom.c` 把 AT24C02 绑定到 `BOARD_I2C_EEPROM`，`board_sensor.c` 把 GXHT30 绑定到 `BOARD_I2C_SENSOR`。

## Services 层

`services` 放较大的业务服务。它比 App 更具体，比底层驱动更抽象。

| 子目录 | 作用 |
| --- | --- |
| `comm_service/` | 通信服务，调用 `protocol` |
| `sensor_service/` | 传感器服务，读取 board ADC |
| `display_service/` | 显示服务，调用 LVGL port，并管理 UI 界面 |
| `upgrade_service/` | 升级服务占位 |
| `app_event/` | 应用事件发布占位 |
| `storage_service/` | 存储服务示例，通过 board EEPROM 访问 24C02 |

App 负责启动和调度，Services 负责承载业务模块。

`services/display_service/ui/` 是 LVGL 界面示例：

| 子目录 | 作用 |
| --- | --- |
| `ui/` | UI 总入口和界面切换 |
| `ui/screens/` | 页面，例如 home、settings |
| `ui/widgets/` | 可复用控件，例如状态栏 |

显示刷新由 `display_service_poll()` 调用 `lvgl_port_tick()`、`ui_tick()`、`lvgl_port_timer_handler()`。

## App 层

| 文件 | 作用 |
| --- | --- |
| `main.c` | 固件主入口，调用 `board_init()`、`osal_init()`、`app_init()`、`app_task_create()`，再进入 OSAL 调度 |
| `app_init.c/.h` | 应用初始化，初始化 services、middleware、logger |
| `app_task.c/.h` | 任务创建和裸机主循环示例 |
| `app_debug.c/.h` | debug 模块的平台适配 |
| `app_config.h` | 应用配置 |
| `app_version.h` | 应用版本 |

任务创建建议继续放在 `app_task.c` 或拆到 `app/<feature>_task.c`。FreeRTOS 模式下 App 通过 `osal_task_create()` 创建任务；裸机模式下不创建任务，`main()` 直接循环调用 `app_task_run()`。

## OSAL 层

| 文件 | 作用 |
| --- | --- |
| `osal.h` | 对上层暴露统一 OS 接口 |
| `osal.c` | 根据 `PROJECT_USE_FREERTOS` 转发到裸机或 FreeRTOS 后端 |
| `osal_baremetal.c/.h` | 裸机 SysTick、delay、millis、临界区 |
| `osal_freertos.c/.h` | FreeRTOS 后端占位，保留真实 FreeRTOS API 替换点 |

App、components、debug 适配层优先调用 OSAL，不直接依赖裸机或 FreeRTOS。

如果之前 FreeRTOS 代码里任务函数是 `while(1)`，切回裸机时不能把这些任务原样塞进裸机主循环。正确做法是把任务里的主体拆成一次执行函数，例如 `xxx_process()`，FreeRTOS 任务里 `while(1)` 调它，裸机主循环里按时间片调它。

## 新增代码放哪

| 想加的代码 | 推荐位置 |
| --- | --- |
| 新业务任务 | `app/app_task.c` 或 `app/<feature>_task.c` |
| 通信业务 | `services/comm_service/` |
| 传感器聚合 | `services/sensor_service/` |
| EEPROM 读写业务 | `services/storage_service/` |
| 板子上某个 ADC 通道用途 | `board/gd32h757_eval/board_adc.c` |
| 硬件版本识别算法 | `components/hardware_version/` |
| 某块板的硬件版本 ADC 通道、mV 换算和版本码映射 | `board/<board>/board_hw_version.c` |
| 板子上的 24C02/GXHT30 实例 | `board/gd32h757_eval/board_eeprom.c`、`board_sensor.c` |
| 通用 ADC 驱动能力 | `bsp/adc/` |
| 通用 I2C 驱动能力 | `bsp/i2c/` |
| 24C02/GXHT30 这类板外芯片驱动 | `components/devices/eeprom/at24c02/`、`components/devices/sensors/gxht30/` |
| 第三方库 | `middleware/<name>/` |
| LVGL 官方源码 | `middleware/lvgl/lvgl/` |
| LVGL 显示/触摸 port | `middleware/lvgl/port/` |
| LVGL 页面和控件 | `services/display_service/ui/` |
| 自研通用算法/工具组件 | `components/<name>/` |
| Cache、MPU、NVIC 分组 | `board/gd32h757_eval/board_system.c` |
| 中断入口 | `startup/gd32h7xx_it.c` |
| 链接脚本 | `linker/` |
| 烧录脚本 | `scripts/` |
| 原始音频资源 | `assets/media/audio/` |
| 原始视频资源 | `assets/media/video/` |
| 原始 HTML/CSS/JS 资源 | `assets/web/` |
| 原始 UI 图片、字体、图标 | `assets/ui/images/`、`assets/ui/fonts/`、`assets/ui/icons/` |
| 文档图片和附件 | `docs/assets/` |
| 文档示例文件 | `docs/examples/` |
