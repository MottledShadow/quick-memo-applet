# Third Party Notices

本文档记录 Quick Memo Applet 目前明确依赖的第三方运行时和许可证注意事项。它不是法律意见。

## Qt

Quick Memo Applet 使用 Qt Widgets 构建桌面界面。项目源码本身使用仓库根目录中的 MIT License；该 MIT License 不改变 Qt 自身的许可条款。

Qt 是商业和开源双许可项目。使用开源 Qt 构建或分发二进制包时，需要确认所使用 Qt 版本、模块、链接方式和分发方式满足对应许可证要求。

当前 Windows Release 包通常会通过 `windeployqt --release` 复制 Qt 运行时和插件，例如：

- `Qt6Core.dll`
- `Qt6Gui.dll`
- `Qt6Widgets.dll`
- `platforms/qwindows.dll`
- 其他由 `windeployqt` 判定需要的 Qt 插件和运行时文件

如果你重新打包或再分发本项目二进制文件，请自行确认 Qt 许可义务，包括但不限于：

- 保留 Qt 相关版权和许可声明
- 明确说明二进制包包含 Qt 运行时
- 在适用的开源 Qt 许可下，遵守 LGPL/GPL 对用户权利、替换/重新链接能力和源码可获得性的要求
- 对 MSVC 构建使用官方 Visual C++ Redistributable，而不是随意分发开发机上的单个运行时 DLL

参考资料：

- [Qt for Windows - Deployment](https://doc.qt.io/qt-6/windows-deployment.html)
- [Qt Open Source LGPL Obligations](https://www.qt.io/licensing/open-source-lgpl-obligations)
