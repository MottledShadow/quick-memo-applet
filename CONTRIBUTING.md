# Contributing

感谢你关注 Quick Memo Applet。这个项目目前是一个轻量级个人桌面工具，欢迎 issue、bug report、小型修复和清晰边界内的 pull request。

## 贡献原则

- 大改动先开 issue 讨论，包括架构调整、数据格式变化、大范围 UI 重做和新依赖。
- 提交 issue 时优先使用 GitHub issue template，方便保留复现步骤、环境和期望行为。
- 保持项目轻量，优先使用现有 Qt Widgets/C++ + QSS 结构。
- 不引入 QML、重型框架或复杂后台服务，除非维护者已经明确接受方向。
- 不提交本地构建产物、Qt Creator 配置、部署 DLL、用户数据或临时文件。
- 改动越靠近用户体验，PR 描述越要写清楚可见变化。

## 本地开发

推荐环境：

- Windows 10/11
- Qt 6 或 Qt 5，包含 Widgets 模块
- CMake 3.16+
- 支持 C++17 的编译器

命令行构建：

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

也可以直接用 Qt Creator 打开仓库根目录并选择合适的 Qt Kit。

## 代码风格

- 继续使用 C++17 和 Qt Widgets。
- UI 样式优先放在 `src/theme/` 的主题和 QSS 体系里。
- 保持命名和周围代码一致，不做无关重排。
- 小函数、小改动优先；大文件拆分应单独讨论。
- 新增行为需要考虑中英文文案和当前设置持久化方式。

## 文档要求

以下改动通常需要同步更新 README：

- 新增或移除用户可见功能
- 修改构建、运行或打包方式
- 修改数据文件位置或 JSON 数据含义
- 修改 Qt 依赖、平台支持或发布流程

如果 PR 涉及 UI 视觉效果，请在 PR 描述中说明影响的窗口和状态。能够提供截图时，请提供截图。

## 提交前检查

提交 PR 前至少运行：

```powershell
cmake --build build --config Debug
git diff --check
```

如果你使用的是 Qt Creator 自动生成的构建目录，也可以运行对应构建目录的 `cmake --build`。

## PR Checklist

提交 PR 时请确认：

- 改动范围和 issue 或 PR 描述一致
- 没有提交 `build/`、`.qtcreator/`、`.agents/`、`.codex/` 或部署 DLL
- 本地构建通过
- `git diff --check` 通过
- 用户可见行为变化已更新 README 或 PR 描述
- 数据格式变化已说明兼容性影响

## 许可证

提交贡献表示你同意你的贡献以本项目的 MIT License 发布。
