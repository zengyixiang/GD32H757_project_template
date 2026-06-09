本目录用于保存项目原始资源文件，不放参与固件编译的源码。

可以在这里保存音频、视频、网页文件、UI 图片、字体、图标等未转换资源。
这些资源被转换成适合固件使用的 C/H 文件后，应放到代码侧的生成资源目录：

- UI 图片、字体、图标：services/display_service/ui/generated/
- Web 静态资源：components/assets/generated/web/
- 音频资源：components/assets/generated/media/audio/
- 视频资源：components/assets/generated/media/video/
