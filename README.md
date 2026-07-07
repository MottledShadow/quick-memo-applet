# Quick Memo Applet

Quick Memo Applet 是一个 Windows-first 的快捷便签小工具，使用 C++、Qt Widgets 和 CMake 编写。它面向日常快速记录：按全局快捷键呼出输入小窗，把内容保存到两张常驻便签窗口中，并通过后台窗口管理记录、外观和工作流偏好。

项目当前处于 MVP 阶段，核心功能已经可用，后续会继续补充截图、图标、Release 包和更完整的发布流程。

## 功能

- 全局快捷键呼出输入窗口，默认快捷键为 `Ctrl+Alt+Space`
- 两类便签窗口：`Idea` 和 `ToDo`
- 后台 Dashboard，用于查看记录、显示或隐藏便签窗口、修改设置
- 亮色、暗色、跟随系统主题
- 字号、显示密度、语言等基础个性化设置
- 便签窗口位置、大小、显示状态和置顶状态持久化
- 开机自启选项
- 输入后是否自动收起输入窗口
- 记录点击行为可配置：删除、完成、仅后台删除
- 记录排序：最新优先或最旧优先
- JSON 导入和导出
- 本地 JSON 存储，无云同步、无账号系统
- 自定义应用图标，显示在窗口、托盘和 Windows 可执行文件中

## 系统要求

主要支持 Windows 10/11。

开发和构建需要：

- CMake 3.16 或更新版本
- C++17 编译器
- Qt 6 或 Qt 5，需包含 Widgets 模块
- Windows 构建需要可用的 Qt/MinGW 或 Qt/MSVC 工具链

项目代码保留了部分非 Windows fallback，但全局热键和开机自启目前是 Windows-first 能力，其他平台不是首要支持目标。

## 从源码构建

使用 Qt Creator：

1. 打开本仓库根目录。
2. 选择已安装 Qt Kit。
3. 配置 CMake。
4. 构建并运行 `Quick_Memo_Applet`。

使用命令行：

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

如果 CMake 找不到 Qt，请先使用 Qt 提供的开发环境终端，或把 Qt 的 CMake 包路径加入 `CMAKE_PREFIX_PATH`。

## 使用方式

1. 启动程序后，应用会驻留在系统托盘。
2. 按 `Ctrl+Alt+Space` 呼出输入窗口。
3. 输入内容后按 Enter 保存。
4. 在输入窗口中切换保存类型，或在后台窗口中调整默认输入类型。
5. 点击托盘图标可打开 Dashboard。
6. 通过托盘菜单退出程序。

如果快捷键注册失败，通常是因为该组合键已被其他应用占用。可以在 Dashboard 中修改快捷键。

## 数据存储

默认数据文件位置：

```text
%APPDATA%\QuickMemoApplet\data.json
```

数据包括记录内容、设置、窗口位置、窗口大小、置顶状态和显示状态。项目不会上传数据，也没有远程同步功能。

Dashboard 中提供 JSON 导出和导入：

- 导出会写出完整本地数据。
- 导入会替换当前数据，并在导入前尝试备份现有数据。

## Windows 打包发布

Release 包建议使用 Release 构建，不要直接发布 Debug 构建目录。

典型流程：

```powershell
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --config Release
windeployqt --release path\to\Quick_Memo_Applet.exe
```

部署包至少需要包含：

- `Quick_Memo_Applet.exe`
- Qt 运行时 DLL，例如 `Qt6Core.dll`、`Qt6Gui.dll`、`Qt6Widgets.dll`
- Windows 平台插件：`platforms\qwindows.dll`
- 编译器运行时依赖和其他由 `windeployqt` 复制的插件
- `LICENSE`
- `THIRD_PARTY_NOTICES.md`

如果直接运行 exe 时提示缺少 `Qt6Core.dll` 或找不到 platform plugin，优先检查部署目录是否由 `windeployqt` 正确生成，以及 `platforms\qwindows.dll` 是否在正确位置。

Qt 官方 Windows 部署文档见：[Qt for Windows - Deployment](https://doc.qt.io/qt-6/windows-deployment.html)。

## 项目结构

```text
src/
  app/       应用入口和主协调逻辑
  core/      本地数据模型、JSON 存储和设置
  platform/  Windows 全局快捷键等平台相关逻辑
  theme/     文案、主题、QSS 和视觉 token
  ui/        Dashboard、输入窗口、便签窗口和托盘控制
```

## 常见问题

### 为什么程序关闭窗口后仍在运行？

这是预期行为。应用是托盘常驻工具，关闭可见窗口不会退出进程。请通过托盘菜单退出。

### 为什么快捷键没有反应？

可能是快捷键被其他应用占用，或程序未成功注册全局快捷键。请打开 Dashboard 修改快捷键后再试。

### 为什么直接拷贝 exe 到别的机器后无法运行？

Qt 应用需要随 exe 一起部署 Qt DLL 和插件。请使用 `windeployqt` 生成部署目录，再打包发布。

### 数据是否会同步到云端？

不会。当前版本只使用本地 JSON 文件。

## 路线图

短期路线图见 [ROADMAP.md](ROADMAP.md)。

当前优先级：

- 补充 README 截图和更完整的发布检查清单
- 增加 Windows CI 构建检查
- 继续整理 Dashboard 设置和用户体验

## 贡献

欢迎提交 issue 和 pull request。较大的功能、架构或视觉方向调整，请先开 issue 讨论，避免重复劳动。

详细说明见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 许可证与第三方依赖

本项目代码使用 MIT License，见 [LICENSE](LICENSE)。

本项目依赖 Qt。Qt 是商业和开源双许可项目；如果发布包含 Qt DLL 的二进制包，请确认所使用 Qt 版本、模块和分发方式满足对应许可证要求。更具体的第三方依赖和 Qt 许可说明见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

Qt 官方资料：

- [Qt for Windows - Deployment](https://doc.qt.io/qt-6/windows-deployment.html)
- [Qt Open Source LGPL Obligations](https://www.qt.io/licensing/open-source-lgpl-obligations)

本 README 中的许可证说明不是法律意见。
