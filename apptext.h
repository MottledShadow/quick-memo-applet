#ifndef APPTEXT_H
#define APPTEXT_H

#include "memostore.h"

#include <QString>
#include <QtGlobal>

class AppText
{
public:
    static QString memoTypeName(MemoType type, AppLanguage language);
    static QString memoTypeSingular(MemoType type, AppLanguage language);

    static QString languageDisplayName(AppLanguage language);
    static QString themeDisplayName(ThemeMode mode, AppLanguage language);
    static QString fontSizeDisplayName(FontSizeMode mode, AppLanguage language);
    static QString densityDisplayName(DensityMode mode, AppLanguage language);

    static QString dashboardWindowTitle(AppLanguage language);
    static QString recordsTitle(AppLanguage language);
    static QString memoWindowsTitle(AppLanguage language);
    static QString settingsTitle(AppLanguage language);
    static QString hotkeyTitle(AppLanguage language);
    static QString globalHotkeyLabel(AppLanguage language);
    static QString applyButton(AppLanguage language);
    static QString applyHotkeyTooltip(AppLanguage language);
    static QString personalizationTitle(AppLanguage language);
    static QString languageLabel(AppLanguage language);
    static QString themeLabel(AppLanguage language);
    static QString fontSizeLabel(AppLanguage language);
    static QString densityLabel(AppLanguage language);
    static QString themeTooltip(AppLanguage language);
    static QString fontSizeTooltip(AppLanguage language);
    static QString densityTooltip(AppLanguage language);
    static QString inputTitle(AppLanguage language);
    static QString hideInputAfterSave(AppLanguage language);
    static QString hideInputAfterSaveTooltip(AppLanguage language);
    static QString systemTitle(AppLanguage language);
    static QString autostart(AppLanguage language);
    static QString autostartTooltip(AppLanguage language);
    static QString pin(AppLanguage language);
    static QString pinTooltip(AppLanguage language);
    static QString show(AppLanguage language);
    static QString hide(AppLanguage language);
    static QString showMemoTooltip(AppLanguage language);
    static QString hideMemoTooltip(AppLanguage language);
    static QString clickToDelete(AppLanguage language);
    static QString ready(AppLanguage language);
    static QString deletedRecord(MemoType type, AppLanguage language);
    static QString deletedRecord(const QString &categoryName, AppLanguage language);

    static QString inputWindowTitle(AppLanguage language);
    static QString switchTypeTooltip(AppLanguage language);
    static QString inputTooltip(AppLanguage language);
    static QString questionPlaceholder(AppLanguage language);
    static QString todoPlaceholder(AppLanguage language);
    static QString emptyInputWarning(AppLanguage language);
    static QString savedTo(MemoType type, AppLanguage language);
    static QString savedTo(const QString &categoryName, AppLanguage language);
    static QString enterSave(AppLanguage language);
    static QString escHide(AppLanguage language);
    static QString renameCategoryTooltip(AppLanguage language);

    static QString hideMemoButtonTooltip(AppLanguage language);
    static QString keepOnTopTooltip(AppLanguage language);
    static QString cancelOnTopTooltip(AppLanguage language);

    static QString openDashboard(AppLanguage language);
    static QString exitApplication(AppLanguage language);
    static QString startupTitle(AppLanguage language);
    static QString startupBody(const QString &hotkeyText, AppLanguage language);

    static QString hotkeyUpdated(const QString &hotkeyText, AppLanguage language);
    static QString hotkeyRegistrationFailed(quint32 errorCode, AppLanguage language);
    static QString hotkeyWindowsOnly(AppLanguage language);
    static QString hotkeyNeedsSingleSequence(AppLanguage language);
    static QString hotkeyNeedsModifier(AppLanguage language);
    static QString hotkeyUnsupportedKey(AppLanguage language);
    static QString themeChanged(ThemeMode mode, AppLanguage language);
    static QString fontSizeChanged(FontSizeMode mode, AppLanguage language);
    static QString densityChanged(DensityMode mode, AppLanguage language);
    static QString memoStartupDisplayTitle(AppLanguage language);
    static QString memoStartupDisplayLabel(AppLanguage language);
    static QString memoStartupDisplayTooltip(AppLanguage language);
    static QString memoStartupDisplayName(MemoStartupDisplayMode mode, AppLanguage language);
    static QString memoStartupDisplayChanged(MemoStartupDisplayMode mode, AppLanguage language);
    static QString categoryNameChanged(const QString &categoryName, AppLanguage language);
    static QString inputAutoHideEnabled(AppLanguage language);
    static QString inputAutoHideDisabled(AppLanguage language);
    static QString autostartFailed(AppLanguage language);
    static QString autostartEnabled(AppLanguage language);
    static QString autostartDisabled(AppLanguage language);
};

#endif // APPTEXT_H
