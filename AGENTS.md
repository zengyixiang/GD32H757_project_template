# 仓库指南

## 项目结构与模块组织
这是一个面向 GD32H757 的 C11/CMake 固件模板，最终目标名为 `C8_GCC`。应用入口和任务创建位于 `app/`。业务服务放在 `services/`；可复用的自研模块放在 `components/`；板级引脚、时钟、外设和硬件版本映射放在 `board/<board>/`。通用外设封装属于 `bsp/`，厂商驱动和 CMSIS 代码属于 `drivers/`，启动代码在 `startup/`，链接脚本在 `linker/`，项目配置头文件在 `config/`。第三方中间件和移植层放在 `middleware/`；除非是明确的移植或集成修复，避免直接修改第三方源码。原始 UI、Web 和媒体资源放在 `assets/`；辅助脚本在 `scripts/` 和 `tools/`；设计和使用说明在 `docs/`。

## 构建、测试与开发命令
- `cmake --preset Debug` 使用 `cmake/arm-none-eabi-gcc.cmake` 配置 Ninja Debug 构建，输出目录为 `build/Debug`。
- `cmake --build --preset Debug` 构建 Debug 固件，并生成 `.elf`、`.hex` 和 `.bin` 文件。
- `cmake --preset Release` 和 `cmake --build --preset Release` 生成优化后的 Release 构建。
- `cmake --preset Debug -DPROJECT_BOARD=my_product_v1` 在当前构建目录中从默认 `gd32h757_eval` 切换到 `my_product_v1`。
- `cmake --build --preset Debug --target flash_openocd` 使用 `scripts/` 中的 OpenOCD/J-Link 配置进行烧录。

## 编码风格与命名约定
C 和 CMake 使用 4 空格缩进。C API 使用小写加下划线命名，例如 `board_init()` 和 `ringbuffer_write()`。结构体 typedef 使用 `_t` 后缀；宏使用大写，并在名称中体现单位或用途，例如 `APP_START_TASK_STACK_WORDS`。头文件保护宏使用简单的大写文件名。保持依赖单向向下：`app -> services -> board/components/middleware -> bsp -> drivers`。

## 测试指南
本仓库目前没有自动化单元测试框架，也没有 `CTest` 配置。最低验证要求是 Debug 和 Release 都能成功构建。涉及硬件的改动还应在烧录后执行板级冒烟测试，并在 PR 中记录使用的板卡、工具以及观察到的 UART/log 行为。

## Agent 验证流程
AI 修改代码后必须自行完成构建、下载、复位和串口验证；只有文档类改动可以跳过固件验证，并在最终回复中说明。默认验证流程如下：

1. 编译 Debug 固件：
   ```powershell
   cmake --preset Debug
   cmake --build --preset Debug
   ```
2. 下载和复位前，先查看 `.vscode/tasks.json`，按其中的下载/复位任务执行，不要凭记忆手写命令。当前 Debug 任务为 `OpenOCD: Flash Debug`，使用 `build/Debug/C8_GCC.elf` 并执行 `verify reset exit`。
3. 查看 log 和控制台使用 `D:\serial-AI-portable\serial-ai-cli.exe`，串口固定为 `COM4`，波特率 `115200`。使用前先确保 `serial-ai-gui` 已启动。
4. 常用串口命令：
   ```powershell
   & 'D:\serial-AI-portable\serial-ai-cli.exe' open --port COM4 --baud 115200
   & 'D:\serial-AI-portable\serial-ai-cli.exe' clear --port COM4
   & 'D:\serial-AI-portable\serial-ai-cli.exe' tail --port COM4
   & 'D:\serial-AI-portable\serial-ai-cli.exe' read --port COM4 --last 50
   & 'D:\serial-AI-portable\serial-ai-cli.exe' send --port COM4 --text 'help'
   & 'D:\serial-AI-portable\serial-ai-cli.exe' close --port COM4
   ```
5. 最终回复必须说明编译结果、下载/复位结果、串口 log 或控制台观察结果；如果硬件、OpenOCD 或串口服务不可用，要给出实际失败命令和错误信息。

## 提交与拉取请求指南
近期提交使用简洁的中文摘要，通常为祈使式或结果导向，例如 `修复了log打印乱码问题` 或 `新增硬件版本识别`。每个提交应聚焦一个变更。PR 应说明影响的层级、选择的板卡、已运行的构建命令、相关硬件测试结果，以及关联的问题或需求。不要提交 `build/` 输出或生成的二进制文件。

## 配置与 Agent 注意事项
`.gitattributes` 负责统一换行：源码和 Markdown 文件使用 LF，PowerShell 脚本保留 CRLF。保留已有用户改动，并将编辑范围限制在当前请求涉及的层级内。
