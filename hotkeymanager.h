#ifndef HOTKEYMANAGER_H
#define HOTKEYMANAGER_H

#include "memostore.h"

#include <QAbstractNativeEventFilter>
#include <QObject>
#include <QKeySequence>

class HotkeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit HotkeyManager(QObject *parent = nullptr);
    ~HotkeyManager() override;

    bool registerHotkey(const QKeySequence &sequence, QString *errorMessage = nullptr);
    void setLanguage(AppLanguage language);
    void unregisterHotkey();

    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

signals:
    void activated();

private:
    bool sequenceToNative(const QKeySequence &sequence, quint32 *modifiers, quint32 *virtualKey, QString *errorMessage) const;

    int hotkeyId;
    bool registered;
    AppLanguage appLanguage;
};

#endif // HOTKEYMANAGER_H
