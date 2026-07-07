# Release Checklist

本文档记录 Quick Memo Applet 的 Windows 发布流程。v1.0.0 之后优先使用这里的流程生成 GitHub Release 附件。

## 发布前检查

1. 确认工作区干净。

```powershell
git status --short --branch
```

2. 确认版本号已经更新。

```powershell
rg -n "project\(Quick_Memo_Applet VERSION|v1\.0\.0" CMakeLists.txt README.md .github
```

3. 跑 Debug 和 Release 构建。

```powershell
cmake --build C:\Users\MOS\Documents\Quick_Memo_Applet\build\Desktop_Qt_6_11_1_MinGW_64_bit_Debug
cmake --build C:\Users\MOS\Documents\Quick_Memo_Applet\build\Desktop_Qt_6_11_1_MinGW_64_bit_Release --config Release
git diff --check
```

4. 用旧版数据做一次兼容检查。

- 准备一份没有 `schemaVersion` 的 v0.x `data.json`。
- 启动应用并确认记录、设置、窗口状态可以读取。
- 修改一个设置或新增一条记录，确认保存后的 JSON 包含 `schemaVersion: 1`。

## 生成发布包

需要：

- Qt Kit 和 `windeployqt.exe`
- Inno Setup 6 和 `ISCC.exe`

运行：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package-windows.ps1 -Version 1.0.0
```

默认会使用：

```text
build\Desktop_Qt_6_11_1_MinGW_64_bit_Release
```

输出目录：

```text
build\release-package
```

预期产物：

```text
QuickMemoApplet-v1.0.0-win64.zip
QuickMemoApplet-v1.0.0-win64-setup.exe
QuickMemoApplet-v1.0.0-win64-SHA256SUMS.txt
```

如果 `windeployqt.exe` 或 `ISCC.exe` 不在 PATH 中，可以传入显式路径：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package-windows.ps1 `
  -Version 1.0.0 `
  -WindeployQtPath C:\Qt\6.11.1\mingw_64\bin\windeployqt.exe `
  -InnoSetupPath "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
```

## 包内容检查

zip 解压目录和安装包安装目录都应至少包含：

- `Quick_Memo_Applet.exe`
- `Qt6Core.dll`
- `Qt6Gui.dll`
- `Qt6Widgets.dll`
- `platforms\qwindows.dll`
- `README.md`
- `LICENSE`
- `THIRD_PARTY_NOTICES.md`

安装包要求：

- 默认安装到用户 LocalAppData 下，不要求管理员权限。
- 创建开始菜单快捷方式。
- 可选创建桌面快捷方式。
- 卸载时不主动删除 `%APPDATA%\QuickMemoApplet\data.json`。

## GitHub Release

v1.0.0 发布附件建议同时上传：

- `QuickMemoApplet-v1.0.0-win64-setup.exe`
- `QuickMemoApplet-v1.0.0-win64.zip`
- `QuickMemoApplet-v1.0.0-win64-SHA256SUMS.txt`

Release notes 使用中文，包含：

- 版本定位
- 用户可见变化
- 数据兼容说明
- 安装包和 zip 的选择建议
- SHA256

## 回滚和补丁

- 已发布 tag 不移动、不覆盖。
- 如果安装包或 zip 有问题，优先发布 `v1.0.1`。
- 如果 Release 附件上传错但 tag 没问题，可以在 GitHub Release 页面替换附件，并在 notes 中说明。
- 如果发现数据兼容问题，先提醒用户备份 `%APPDATA%\QuickMemoApplet\data.json`，再准备补丁版本。
