# Quick Memo Applet

Quick Memo Applet 是一个 Windows 快捷便签小工具。它常驻系统托盘，按一个快捷键就能打开输入框，把想法或待办保存到两张便签窗口里。

这个 README 面向第一次使用的用户，目标是 5 分钟内下载、运行、写下第一条便签。

## 5 分钟开始使用

### 1. 下载

打开 GitHub Releases 页面，下载最新版：

- 推荐：`QuickMemoApplet-v1.0.0-win64-setup.exe`
- 免安装备用：`QuickMemoApplet-v1.0.0-win64.zip`

如果 Windows 提示未知发布者，这是因为当前版本还没有代码签名。请只从本项目 GitHub Release 页面下载。

### 2. 安装或解压

使用安装包：

1. 双击 `QuickMemoApplet-v1.0.0-win64-setup.exe`。
2. 按安装向导完成安装。
3. 安装结束后可以直接启动 Quick Memo Applet。

使用 zip：

1. 右键 zip，选择“全部解压”。
2. 进入解压后的文件夹。
3. 双击 `Quick_Memo_Applet.exe`。

不要只把 exe 单独拷贝出来运行。Qt 程序需要和旁边的 DLL、`platforms/qwindows.dll` 一起放在同一个部署目录里。

### 3. 找到托盘图标

启动后，程序会在 Windows 系统托盘后台运行。托盘图标可能在任务栏右下角，也可能藏在“显示隐藏的图标”里。

- 单击或双击托盘图标：打开 Dashboard。
- 右键托盘图标：打开菜单，可以退出程序。

### 4. 写第一条便签

默认快捷键是：

```text
Ctrl + Alt + Space
```

按下快捷键后会出现输入小窗：

1. 输入内容。
2. 选择保存到 `Idea` 或 `ToDo`。
3. 按 Enter 保存。
4. 按 Esc 收起输入小窗。

如果快捷键没有反应，通常是这个组合键被其他软件占用了。打开 Dashboard，在“快捷输入”里修改快捷键。

## 主要功能

- 全局快捷键呼出输入小窗。
- 两张便签窗口：`Idea` 和 `ToDo`。
- Dashboard 管理记录、设置、便签显示和数据导入导出。
- 亮色、暗色、跟随系统主题。
- 字号、显示密度、语言设置。
- 便签窗口位置、大小、显示状态和置顶状态自动保存。
- 开机自启选项。
- JSON 导入和导出。
- 本地存储，无账号、无云同步。

## 数据保存在哪里

默认数据文件在：

```text
%APPDATA%\QuickMemoApplet\data.json
```

数据只保存在本机，包括记录内容、设置、窗口位置、窗口大小、置顶状态和显示状态。项目不会上传数据，也没有云同步。

Dashboard 的“数据与系统”里可以导出或导入 JSON。导入会替换当前数据，并会先尝试备份现有数据。

更详细的数据兼容说明见 [docs/DATA_COMPATIBILITY.md](docs/DATA_COMPATIBILITY.md)。

## 常见问题

### 关闭窗口后为什么程序还在运行？

这是正常行为。Quick Memo Applet 是托盘常驻工具，关闭可见窗口不会退出进程。请通过托盘菜单退出。

### 快捷键没有反应怎么办？

可能是快捷键被其他程序占用。打开 Dashboard，在“快捷输入”里设置一个新的快捷键。

### 为什么 zip 里的 exe 单独拷出去不能运行？

Qt 程序需要随 exe 一起部署 Qt DLL 和插件。请保留 zip 解压后的目录结构。

### 卸载会删除我的便签吗？

安装包卸载时不会主动删除 `%APPDATA%\QuickMemoApplet\data.json`。如果你想完全清理数据，需要手动删除这个目录。

### 支持 macOS 或 Linux 吗？

当前项目是 Windows-first。代码里有部分非 Windows fallback，但全局快捷键和开机自启主要面向 Windows。

## 给开发者

如果你想从源码构建、开发或提交 PR，请看 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 许可证与第三方依赖

本项目代码使用 MIT License，见 [LICENSE](LICENSE)。

本项目依赖 Qt。Qt 是商业和开源双许可项目；如果发布包含 Qt DLL 的二进制包，请确认所使用 Qt 版本、模块和分发方式满足对应许可证要求。更具体的第三方依赖和 Qt 许可说明见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

本 README 中的许可证说明不是法律意见。
