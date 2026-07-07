# Contributing

感谢你关注 Quick Memo Applet。本文档面向开发者，说明如何搭建环境、构建项目、理解目录结构和提交改动。

## 开发环境

推荐环境：

- Windows 10/11
- Qt 6.11.x 或其他 Qt 6/Qt 5 Widgets Kit
- CMake 3.16+
- 支持 C++17 的编译器，当前主要使用 MinGW 64-bit
- Git
- 可选：Inno Setup 6，用于生成 Windows 安装包

项目保持 Windows-first、Qt Widgets/C++ + QSS 方向。不引入 QML、重型框架、复杂后台服务或云同步，除非维护者先接受方向。

## 获取和打开项目

```powershell
git clone https://github.com/MottledShadow/quick-memo-applet.git
cd quick-memo-applet
```

使用 Qt Creator：

1. 打开仓库根目录。
2. 选择可用的 Qt Widgets Kit。
3. 配置 CMake。
4. 构建并运行 `Quick_Memo_Applet`。

使用命令行：

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

如果 CMake 找不到 Qt，请使用 Qt 提供的开发环境终端，或设置 `CMAKE_PREFIX_PATH` 指向 Qt Kit，例如 `C:\Qt\6.11.1\mingw_64`。

## 目录结构

```text
src/
  app/       应用入口和主协调逻辑
  core/      本地数据模型、JSON 存储和设置
  platform/  Windows 全局快捷键等平台相关逻辑
  theme/     文案、主题、QSS 和视觉 token
  ui/        Dashboard、输入窗口、便签窗口和托盘控制
resources/  图标、qrc 和 Windows rc 资源
scripts/    本地工程化脚本
installer/  Inno Setup 安装包脚本
docs/       发布、数据兼容等维护文档
```

## 开发约束

- 保持 C++17 和 Qt Widgets。
- UI 视觉优先走 `src/theme/` 的 QSS 和 token。
- 用户可见文案需要同时维护中文和英文。
- 修改数据字段时必须更新 [docs/DATA_COMPATIBILITY.md](docs/DATA_COMPATIBILITY.md)。
- 不提交 `build/`、`build-*`、`.qtcreator/`、部署 DLL、用户数据、zip 或 exe。
- 不做无关格式化或大范围重排。

## 数据兼容

默认数据路径：

```text
%APPDATA%\QuickMemoApplet\data.json
```

当前 JSON schema 版本为 `1`。旧版没有 `schemaVersion` 的数据按 v0.x 数据读取。读取时会忽略未知字段，缺失字段使用默认值，非法记录会跳过。

详细策略见 [docs/DATA_COMPATIBILITY.md](docs/DATA_COMPATIBILITY.md)。

## 本地验证

提交前至少运行：

```powershell
cmake --build C:\Users\MOS\Documents\Quick_Memo_Applet\build\Desktop_Qt_6_11_1_MinGW_64_bit_Debug
git diff --check
```

如果你使用自己的构建目录，替换为对应路径即可。

涉及发布或打包时，还要运行 Release 构建：

```powershell
cmake --build C:\Users\MOS\Documents\Quick_Memo_Applet\build\Desktop_Qt_6_11_1_MinGW_64_bit_Release --config Release
```

## Windows 打包

可重复打包脚本：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package-windows.ps1 -Version 1.0.0
```

脚本会：

- 构建 Release 目标。
- 使用 `windeployqt --release` 收集 Qt DLL 和插件。
- 复制 `README.md`、`LICENSE`、`THIRD_PARTY_NOTICES.md`。
- 生成 `QuickMemoApplet-v1.0.0-win64.zip`。
- 使用 Inno Setup 生成 `QuickMemoApplet-v1.0.0-win64-setup.exe`。
- 输出 SHA256。

如果提示找不到 `ISCC.exe`，请安装 Inno Setup 6，或传入 `-InnoSetupPath`。

## PR Checklist

提交 PR 前确认：

- 改动范围和描述一致。
- 本地构建通过。
- `git diff --check` 通过。
- 用户可见行为变化已更新 README 或相关 docs。
- 数据格式变化已更新数据兼容文档。
- 没有提交本地构建产物、部署产物或个人配置。

## 许可证

提交贡献表示你同意你的贡献以本项目的 MIT License 发布。
