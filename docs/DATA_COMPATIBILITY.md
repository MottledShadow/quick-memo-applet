# Data Compatibility

Quick Memo Applet 使用本地 JSON 文件保存记录和设置。本文档说明 v1.0.0 起的数据格式和兼容策略。

## 默认数据位置

```text
%APPDATA%\QuickMemoApplet\data.json
```

项目不会上传数据，也没有云同步。用户可以在 Dashboard 的“数据与系统”中导出或导入 JSON。

## 当前 schema

当前写出格式包含：

```json
{
  "schemaVersion": 1,
  "currentType": "question",
  "hotkey": "Ctrl+Alt+Space",
  "autostart": false,
  "hideInputAfterSave": false,
  "theme": "system",
  "language": "zh-cn",
  "fontSize": "default",
  "density": "comfortable",
  "categoryNames": {
    "question": "Idea",
    "todo": "ToDo"
  },
  "memoStartupDisplay": "restore",
  "defaultInputType": "lastUsed",
  "recordClickAction": "delete",
  "recordSortOrder": "newestFirst",
  "windows": {
    "question": {
      "geometry": {
        "x": 0,
        "y": 0,
        "w": 320,
        "h": 420
      },
      "visible": true,
      "alwaysOnTop": true
    },
    "todo": {
      "geometry": {
        "x": 0,
        "y": 0,
        "w": 320,
        "h": 420
      },
      "visible": true,
      "alwaysOnTop": true
    }
  },
  "records": [
    {
      "id": "uuid",
      "type": "question",
      "text": "memo text",
      "createdAt": "2026-07-07T12:00:00",
      "completedAt": "2026-07-07T12:05:00"
    }
  ]
}
```

`completedAt` 只在记录完成时写出。

## v0.x 数据兼容

v0.x 数据没有 `schemaVersion` 字段。v1.0.0 会把缺失该字段的数据当作旧版数据读取。

读取策略：

- 未知字段会被忽略。
- 缺失字段会使用当前默认值。
- `records` 不是数组时会拒绝导入。
- 单条记录不是对象、缺少 `id` 或内容为空时会跳过。
- 无效的 `createdAt` 会用当前时间兜底。
- 无效窗口尺寸会使用默认窗口位置和大小。

保存策略：

- v1.0.0 之后写出的 JSON 会包含 `schemaVersion: 1`。
- 现有字段含义保持稳定。
- 不改变默认数据路径。

## 导入和备份

导入 JSON 前，应用会尝试备份当前数据。

备份文件位于同一个目录：

```text
%APPDATA%\QuickMemoApplet\data.backup-YYYYMMDD-HHMMSS.json
```

如果备份失败，导入会停止，不会覆盖当前数据。

## 未来变更规则

兼容性优先级：

1. 优先新增字段，并为旧数据提供默认值。
2. 避免重命名或删除现有字段。
3. 必须改变字段含义时，提升 `schemaVersion` 并写清楚迁移策略。
4. 破坏性迁移必须先备份原始数据。
5. 修改数据格式时同步更新 README、CONTRIBUTING 和本文档中相关部分。

## 手动备份

用户可以直接复制这个目录作为完整备份：

```text
%APPDATA%\QuickMemoApplet
```

也可以在 Dashboard 中导出 JSON，保存到其他位置。
