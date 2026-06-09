# Debug

logger 组件位于 `components/logger`。App 在 `app/app_debug.c` 中完成平台适配，向 logger 模块提供 output、critical-section 和 time callback。

使用 `LOG_*` 的源文件应在包含 `log.h` 之前定义本文件自己的 `DEBUG_LEVEL`，不要依赖 `debug.h` 中的默认 `LVL_WARNING`。
