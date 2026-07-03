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

    MemoWindowState windowState(MemoType type) const;
    void setWindowState(MemoType type, const MemoWindowState &state);

    void addMemo(MemoType type, const QString &text);
    void deleteMemo(const QString &id);

    QString dataFilePath() const;

    static QString typeToString(MemoType type);
    static MemoType typeFromString(const QString &value);
    static QString displayName(MemoType type);

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
    MemoWindowState questionState;
    MemoWindowState todoState;
};

#endif // MEMOSTORE_H
