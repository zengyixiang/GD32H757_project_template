# 架构

本工程采用分层式嵌入式结构：

```text
app -> services -> board -> bsp -> drivers
app -> middleware/components
board -> middleware/freertos
startup -> middleware/freertos
```

依赖方向应尽量保持单向：上层调用下层，下层不反向依赖业务代码。需要跨层通知时，优先使用回调、事件队列或 `services/app_event` 这类解耦接口。
