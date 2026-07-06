#include "apptext.h"

namespace {
QString text(AppLanguage language, const char *zhText, const char *enText)
{
    return QString::fromUtf8(language == AppLanguage::English ? enText : zhText);
}
}

QString AppText::memoTypeName(MemoType type, AppLanguage language)
{
    if (type == MemoType::Todo) {
        return QStringLiteral("ToDo");
    }
    return QStringLiteral("Idea");

    if (type == MemoType::Todo) {
        return text(language, "待办", "Todos");
    }
    return text(language, "问题", "Questions");
}

QString AppText::memoTypeSingular(MemoType type, AppLanguage language)
{
    if (type == MemoType::Todo) {
        return QStringLiteral("ToDo");
    }
    return QStringLiteral("Idea");

    if (type == MemoType::Todo) {
        return text(language, "待办", "todo");
    }
    return text(language, "问题", "question");
}

QString AppText::languageDisplayName(AppLanguage language)
{
    return language == AppLanguage::English ? QStringLiteral("English") : QString::fromUtf8("中文");
}

QString AppText::themeDisplayName(ThemeMode mode, AppLanguage language)
{
    switch (mode) {
    case ThemeMode::System:
        return text(language, "跟随系统", "System");
    case ThemeMode::Dark:
        return text(language, "暗色", "Dark");
    case ThemeMode::Light:
        return text(language, "亮色", "Light");
    }

    return text(language, "跟随系统", "System");
}

QString AppText::fontSizeDisplayName(FontSizeMode mode, AppLanguage language)
{
    switch (mode) {
    case FontSizeMode::Small:
        return text(language, "小", "Small");
    case FontSizeMode::Large:
        return text(language, "大", "Large");
    case FontSizeMode::Default:
        return text(language, "默认", "Default");
    }

    return text(language, "默认", "Default");
}

QString AppText::densityDisplayName(DensityMode mode, AppLanguage language)
{
    return mode == DensityMode::Compact ? text(language, "紧凑", "Compact")
                                        : text(language, "舒适", "Comfortable");
}

QString AppText::dashboardWindowTitle(AppLanguage language)
{
    return text(language, "Quick Memo 后台", "Quick Memo Dashboard");
}

QString AppText::recordsTitle(AppLanguage language)
{
    return text(language, "全部记录", "All Records");
}

QString AppText::memoWindowsTitle(AppLanguage language)
{
    return text(language, "便签窗口", "Memo Windows");
}

QString AppText::settingsTitle(AppLanguage language)
{
    return text(language, "设置", "Settings");
}

QString AppText::hotkeyTitle(AppLanguage language)
{
    return text(language, "快捷键", "Hotkey");
}

QString AppText::globalHotkeyLabel(AppLanguage language)
{
    return text(language, "全局快捷键", "Global hotkey");
}

QString AppText::applyButton(AppLanguage language)
{
    return text(language, "应用", "Apply");
}

QString AppText::applyHotkeyTooltip(AppLanguage language)
{
    return text(language, "应用新的全局快捷键", "Apply the new global hotkey");
}

QString AppText::personalizationTitle(AppLanguage language)
{
    return text(language, "个性化", "Personalization");
}

QString AppText::languageLabel(AppLanguage language)
{
    return text(language, "语言", "Language");
}

QString AppText::themeLabel(AppLanguage language)
{
    return text(language, "主题", "Theme");
}

QString AppText::fontSizeLabel(AppLanguage language)
{
    return text(language, "字号", "Font size");
}

QString AppText::densityLabel(AppLanguage language)
{
    return text(language, "密度", "Density");
}

QString AppText::themeTooltip(AppLanguage language)
{
    return text(language, "切换亮色、暗色或跟随系统主题",
                "Choose light, dark, or the system theme");
}

QString AppText::fontSizeTooltip(AppLanguage language)
{
    return text(language, "调整应用内文字大小", "Adjust in-app text size");
}

QString AppText::densityTooltip(AppLanguage language)
{
    return text(language, "调整控件和列表的紧凑程度", "Adjust control and list density");
}

QString AppText::inputTitle(AppLanguage language)
{
    return text(language, "输入", "Input");
}

QString AppText::hideInputAfterSave(AppLanguage language)
{
    return text(language, "保存后自动收起输入框", "Auto-hide input after saving");
}

QString AppText::hideInputAfterSaveTooltip(AppLanguage language)
{
    return text(language, "保存成功后短暂显示反馈并自动收起输入框",
                "Show brief feedback, then hide the input window after saving");
}

QString AppText::systemTitle(AppLanguage language)
{
    return text(language, "系统", "System");
}

QString AppText::autostart(AppLanguage language)
{
    return text(language, "开机自启", "Start with Windows");
}

QString AppText::autostartTooltip(AppLanguage language)
{
    return text(language, "开机后自动启动 Quick Memo", "Start Quick Memo automatically with Windows");
}

QString AppText::pin(AppLanguage language)
{
    return text(language, "置顶", "Pin");
}

QString AppText::pinTooltip(AppLanguage language)
{
    return text(language, "保持便签窗口置顶", "Keep the memo window on top");
}

QString AppText::show(AppLanguage language)
{
    return text(language, "显示", "Show");
}

QString AppText::hide(AppLanguage language)
{
    return text(language, "隐藏", "Hide");
}

QString AppText::showMemoTooltip(AppLanguage language)
{
    return text(language, "显示便签窗口", "Show memo window");
}

QString AppText::hideMemoTooltip(AppLanguage language)
{
    return text(language, "隐藏便签窗口", "Hide memo window");
}

QString AppText::clickToDelete(AppLanguage language)
{
    return text(language, "点击删除", "Click to delete");
}

QString AppText::ready(AppLanguage language)
{
    return text(language, "就绪", "Ready");
}

QString AppText::deletedRecord(MemoType type, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Deleted one %1.").arg(memoTypeSingular(type, language));
    }
    return QString::fromUtf8("已删除一条%1。").arg(memoTypeSingular(type, language));
}

QString AppText::deletedRecord(const QString &categoryName, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Deleted one %1.").arg(categoryName);
    }
    return QString::fromUtf8("已删除一条%1。").arg(categoryName);
}

QString AppText::inputWindowTitle(AppLanguage language)
{
    return text(language, "快速记录", "Quick Memo Input");
}

QString AppText::switchTypeTooltip(AppLanguage language)
{
    return text(language, "切换记录类型", "Switch memo type");
}

QString AppText::inputTooltip(AppLanguage language)
{
    return text(language, "输入后按 Enter 保存", "Press Enter to save");
}

QString AppText::questionPlaceholder(AppLanguage language)
{
    return text(language, "记下一个问题...", "Capture a question...");
}

QString AppText::todoPlaceholder(AppLanguage language)
{
    return text(language, "写下一件待办...", "Capture a todo...");
}

QString AppText::emptyInputWarning(AppLanguage language)
{
    return text(language, "请输入内容", "Enter something");
}

QString AppText::savedTo(MemoType type, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Saved to %1").arg(memoTypeName(type, language));
    }
    return QString::fromUtf8("已保存到 %1").arg(memoTypeName(type, language));
}

QString AppText::savedTo(const QString &categoryName, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Saved to %1").arg(categoryName);
    }
    return QString::fromUtf8("已保存到 %1").arg(categoryName);
}

QString AppText::enterSave(AppLanguage language)
{
    return text(language, "Enter 保存", "Enter Save");
}

QString AppText::escHide(AppLanguage language)
{
    return text(language, "Esc 收起", "Esc Hide");
}

QString AppText::renameCategoryTooltip(AppLanguage language)
{
    return text(language, "双击重命名分类", "Double-click to rename");
}

QString AppText::hideMemoButtonTooltip(AppLanguage language)
{
    return text(language, "隐藏便签", "Hide memo");
}

QString AppText::keepOnTopTooltip(AppLanguage language)
{
    return text(language, "保持置顶", "Keep on top");
}

QString AppText::cancelOnTopTooltip(AppLanguage language)
{
    return text(language, "取消置顶", "Cancel pin");
}

QString AppText::openDashboard(AppLanguage language)
{
    return text(language, "打开后台", "Open Dashboard");
}

QString AppText::exitApplication(AppLanguage language)
{
    return text(language, "退出程序", "Exit");
}

QString AppText::startupTitle(AppLanguage language)
{
    return text(language, "Quick Memo 已在后台运行", "Quick Memo is running");
}

QString AppText::startupBody(const QString &hotkeyText, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Press %1 to open input\nClick the tray icon to open Dashboard").arg(hotkeyText);
    }
    return QString::fromUtf8("按 %1 呼出输入框\n点击托盘图标可打开后台").arg(hotkeyText);
}

QString AppText::hotkeyUpdated(const QString &hotkeyText, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Hotkey updated: %1").arg(hotkeyText);
    }
    return QString::fromUtf8("快捷键已更新：%1").arg(hotkeyText);
}

QString AppText::hotkeyRegistrationFailed(quint32 errorCode, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Hotkey registration failed. It may already be used by another app. Windows error: %1")
            .arg(errorCode);
    }
    return QString::fromUtf8("快捷键注册失败，可能已被其他程序占用。Windows 错误码：%1").arg(errorCode);
}

QString AppText::hotkeyWindowsOnly(AppLanguage language)
{
    return text(language, "全局快捷键当前只支持 Windows。", "Global hotkeys are currently Windows-only.");
}

QString AppText::hotkeyNeedsSingleSequence(AppLanguage language)
{
    return text(language, "快捷键必须是一个组合键。", "The hotkey must be a single key sequence.");
}

QString AppText::hotkeyNeedsModifier(AppLanguage language)
{
    return text(language, "快捷键至少需要一个修饰键，比如 Ctrl 或 Alt。",
                "The hotkey needs at least one modifier, such as Ctrl or Alt.");
}

QString AppText::hotkeyUnsupportedKey(AppLanguage language)
{
    return text(language, "快捷键主键只支持字母、数字、空格或 F1-F24。",
                "The main hotkey must be a letter, number, Space, or F1-F24.");
}

QString AppText::themeChanged(ThemeMode mode, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Theme changed to: %1").arg(themeDisplayName(mode, language));
    }
    return QString::fromUtf8("主题已切换为：%1").arg(themeDisplayName(mode, language));
}

QString AppText::fontSizeChanged(FontSizeMode mode, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Font size changed to: %1").arg(fontSizeDisplayName(mode, language));
    }
    return QString::fromUtf8("字号已切换为：%1").arg(fontSizeDisplayName(mode, language));
}

QString AppText::densityChanged(DensityMode mode, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Density changed to: %1").arg(densityDisplayName(mode, language));
    }
    return QString::fromUtf8("密度已切换为：%1").arg(densityDisplayName(mode, language));
}

QString AppText::memoStartupDisplayTitle(AppLanguage language)
{
    return text(language, "便签显示", "Memo display");
}

QString AppText::memoStartupDisplayLabel(AppLanguage language)
{
    return text(language, "启动时", "On startup");
}

QString AppText::memoStartupDisplayTooltip(AppLanguage language)
{
    return text(language, "设置程序启动时便签窗口的显示方式",
                "Choose how memo windows appear when the app starts");
}

QString AppText::memoStartupDisplayName(MemoStartupDisplayMode mode, AppLanguage language)
{
    switch (mode) {
    case MemoStartupDisplayMode::ShowAll:
        return text(language, "全部显示", "Show all");
    case MemoStartupDisplayMode::HideAll:
        return text(language, "全部隐藏", "Hide all");
    case MemoStartupDisplayMode::Restore:
        return text(language, "恢复上次状态", "Restore previous");
    }

    return text(language, "恢复上次状态", "Restore previous");
}

QString AppText::memoStartupDisplayChanged(MemoStartupDisplayMode mode, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Memo startup display: %1").arg(memoStartupDisplayName(mode, language));
    }
    return QString::fromUtf8("便签启动显示：%1").arg(memoStartupDisplayName(mode, language));
}

QString AppText::categoryNameChanged(const QString &categoryName, AppLanguage language)
{
    if (language == AppLanguage::English) {
        return QStringLiteral("Category renamed: %1").arg(categoryName);
    }
    return QString::fromUtf8("分类已重命名：%1").arg(categoryName);
}

QString AppText::inputAutoHideEnabled(AppLanguage language)
{
    return text(language, "保存后将自动收起输入框。", "Input will auto-hide after saving.");
}

QString AppText::inputAutoHideDisabled(AppLanguage language)
{
    return text(language, "保存后将继续输入。", "Input will stay open after saving.");
}

QString AppText::autostartFailed(AppLanguage language)
{
    return text(language, "开机自启设置失败。", "Failed to update autostart.");
}

QString AppText::autostartEnabled(AppLanguage language)
{
    return text(language, "已开启开机自启。", "Autostart enabled.");
}

QString AppText::autostartDisabled(AppLanguage language)
{
    return text(language, "已关闭开机自启。", "Autostart disabled.");
}
