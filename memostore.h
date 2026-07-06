#ifndef MEMOSTORE_H
#define MEMOSTORE_H

#include <QDateTime>
#include <QObject>
#include <QRect>
#include <QString>
#include <QVector>

enum class MemoType {
    Question,
    Todo
};

enum class ThemeMode {
    System,
    Light,
    Dark
};

enum class AppLanguage {
    ZhCn,
    English
};

enum class FontSizeMode {
    Small,
    Default,
    Large
};

enum class DensityMode {
    Compact,
    Comfortable
};

enum class MemoStartupDisplayMode {
    Restore,
    ShowAll,
    HideAll
};

struct MemoItem {
    QString id;
    MemoType type;
    QString text;
    QDateTime createdAt;
};

struct MemoWindowState {
    QRect geometry;
    bool visible = true;
    bool alwaysOnTop = true;
};

class MemoStore : public QObject
{
    Q_OBJECT

public:
    explicit MemoStore(QObject *parent = nullptr);

    bool load();
    bool save() const;

    QVector<MemoItem> records() const;
    QVector<MemoItem> records(MemoType type) const;

    MemoType currentType() const;
    void setCurrentType(MemoType type);

    QString hotkey() const;
    void setHotkey(const QString &hotkey);

    bool autostartEnabled() const;
    void setAutostartEnabled(bool enabled);

    bool hideInputAfterSave() const;
    void setHideInputAfterSave(bool enabled);

    ThemeMode themeMode() const;
    void setThemeMode(ThemeMode mode);

    AppLanguage language() const;
    void setLanguage(AppLanguage language);

    FontSizeMode fontSizeMode() const;
    void setFontSizeMode(FontSizeMode mode);

    DensityMode densityMode() const;
    void setDensityMode(DensityMode mode);

    QString categoryName(MemoType type) const;
    void setCategoryName(MemoType type, const QString &name);

    MemoStartupDisplayMode memoStartupDisplayMode() const;
    void setMemoStartupDisplayMode(MemoStartupDisplayMode mode);

    MemoWindowState windowState(MemoType type) const;
    void setWindowState(MemoType type, const MemoWindowState &state);

    void addMemo(MemoType type, const QString &text);
    void deleteMemo(const QString &id);

    QString dataFilePath() const;

    static QString typeToString(MemoType type);
    static MemoType typeFromString(const QString &value);
    static QString displayName(MemoType type);
    static QString themeToString(ThemeMode mode);
    static ThemeMode themeFromString(const QString &value);
    static QString themeDisplayName(ThemeMode mode);
    static QString languageToString(AppLanguage language);
    static AppLanguage languageFromString(const QString &value);
    static QString fontSizeToString(FontSizeMode mode);
    static FontSizeMode fontSizeFromString(const QString &value);
    static QString densityToString(DensityMode mode);
    static DensityMode densityFromString(const QString &value);
    static QString memoStartupDisplayToString(MemoStartupDisplayMode mode);
    static MemoStartupDisplayMode memoStartupDisplayFromString(const QString &value);

signals:
    void recordsChanged();
    void settingsChanged();
    void windowStateChanged(MemoType type);

private:
    MemoWindowState defaultWindowState(MemoType type) const;
    QString appDataDir() const;

    QVector<MemoItem> memoRecords;
    MemoType activeType;
    QString hotkeyText;
    bool autostart;
    bool hideInputOnSave;
    ThemeMode theme;
    AppLanguage appLanguage;
    FontSizeMode fontSize;
    DensityMode density;
    QString questionCategoryName;
    QString todoCategoryName;
    MemoStartupDisplayMode memoStartupDisplay;
    MemoWindowState questionState;
    MemoWindowState todoState;
};

#endif // MEMOSTORE_H
