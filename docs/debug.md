# Debug

日志使用 `middleware/log/EasyLogger`，该目录作为第三方子仓库管理，不修改其中任何文件。

本工程自己的 EasyLogger 配置在 `config/elog_cfg.h`，FreeRTOS 异步输出任务和 port 在 `middleware/log/`。`middleware/log/CMakeLists.txt` 把 `config/` 放在 EasyLogger 官方 include 目录前面，因此 `elog.h` 中的 `<elog_cfg.h>` 会优先解析到本工程配置。

业务源文件在包含 `log.h` 或 `elog.h` 前定义自己的 `LOG_TAG` 和 `LOG_LVL`，然后直接使用 EasyLogger 的 `log_i()`、`log_d()` 等接口。`app/app_debug.c` 只负责把输出回调注册到当前 board 的调试 UART。
