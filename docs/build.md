# 构建

```powershell
cmake --preset Debug
cmake --build --preset Debug

cmake --preset Release
cmake --build --preset Release
```

默认 board 由 `board/CMakeLists.txt` 中的 `PROJECT_BOARD` 选择。构建产物输出到 `build/<配置名>/`，该目录不应提交到版本库。
