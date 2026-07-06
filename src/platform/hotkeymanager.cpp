#include "hotkeymanager.h"

#include "theme/apptext.h"

#include <QApplication>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

HotkeyManager::HotkeyManager(QObject *parent)
    : QObject(parent)
    , hotkeyId(0x514d)
    , registered(false)
    , appLanguage(AppLanguage::ZhCn)
{
    qApp->installNativeEventFilter(this);
}

HotkeyManager::~HotkeyManager()
{
    unregisterHotkey();
    qApp->removeNativeEventFilter(this);
}

bool HotkeyManager::registerHotkey(const QKeySequence &sequence, QString *errorMessage)
{
    quint32 modifiers = 0;
    quint32 virtualKey = 0;
    if (!sequenceToNative(sequence, &modifiers, &virtualKey, errorMessage)) {
        return false;
    }

    unregisterHotkey();

#ifdef Q_OS_WIN
    if (!RegisterHotKey(nullptr, hotkeyId, modifiers, virtualKey)) {
        if (errorMessage != nullptr) {
            *errorMessage = AppText::hotkeyRegistrationFailed(GetLastError(), appLanguage);
        }
        return false;
    }

    registered = true;
    return true;
#else
    Q_UNUSED(modifiers)
    Q_UNUSED(virtualKey)
    if (errorMessage != nullptr) {
        *errorMessage = AppText::hotkeyWindowsOnly(appLanguage);
    }
    return false;
#endif
}

void HotkeyManager::setLanguage(AppLanguage language)
{
    appLanguage = language;
}

void HotkeyManager::unregisterHotkey()
{
#ifdef Q_OS_WIN
    if (registered) {
        UnregisterHotKey(nullptr, hotkeyId);
        registered = false;
    }
#endif
}

bool HotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType)

#ifdef Q_OS_WIN
    auto *msg = static_cast<MSG *>(message);
    if (msg != nullptr && msg->message == WM_HOTKEY && msg->wParam == static_cast<WPARAM>(hotkeyId)) {
        if (result != nullptr) {
            *result = 0;
        }
        emit activated();
        return true;
    }
#else
    Q_UNUSED(message)
    Q_UNUSED(result)
#endif

    return false;
}

bool HotkeyManager::sequenceToNative(const QKeySequence &sequence,
                                     quint32 *modifiers,
                                     quint32 *virtualKey,
                                     QString *errorMessage) const
{
    if (sequence.isEmpty() || sequence.count() != 1) {
        if (errorMessage != nullptr) {
            *errorMessage = AppText::hotkeyNeedsSingleSequence(appLanguage);
        }
        return false;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const int combined = sequence[0].toCombined();
#else
    const int combined = sequence[0];
#endif

    const Qt::KeyboardModifiers qtModifiers = Qt::KeyboardModifiers(combined & Qt::KeyboardModifierMask);
    const int key = combined & ~Qt::KeyboardModifierMask;

    if (!qtModifiers.testFlag(Qt::ControlModifier)
        && !qtModifiers.testFlag(Qt::AltModifier)
        && !qtModifiers.testFlag(Qt::ShiftModifier)
        && !qtModifiers.testFlag(Qt::MetaModifier)) {
        if (errorMessage != nullptr) {
            *errorMessage = AppText::hotkeyNeedsModifier(appLanguage);
        }
        return false;
    }

#ifdef Q_OS_WIN
    quint32 nativeModifiers = 0;
    if (qtModifiers.testFlag(Qt::ControlModifier)) {
        nativeModifiers |= MOD_CONTROL;
    }
    if (qtModifiers.testFlag(Qt::AltModifier)) {
        nativeModifiers |= MOD_ALT;
    }
    if (qtModifiers.testFlag(Qt::ShiftModifier)) {
        nativeModifiers |= MOD_SHIFT;
    }
    if (qtModifiers.testFlag(Qt::MetaModifier)) {
        nativeModifiers |= MOD_WIN;
    }
#ifdef MOD_NOREPEAT
    nativeModifiers |= MOD_NOREPEAT;
#endif

    quint32 nativeKey = 0;
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        nativeKey = 'A' + (key - Qt::Key_A);
    } else if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        nativeKey = '0' + (key - Qt::Key_0);
    } else if (key == Qt::Key_Space) {
        nativeKey = VK_SPACE;
    } else if (key >= Qt::Key_F1 && key <= Qt::Key_F24) {
        nativeKey = VK_F1 + (key - Qt::Key_F1);
    }

    if (nativeKey == 0) {
        if (errorMessage != nullptr) {
            *errorMessage = AppText::hotkeyUnsupportedKey(appLanguage);
        }
        return false;
    }

    *modifiers = nativeModifiers;
    *virtualKey = nativeKey;
    return true;
#else
    Q_UNUSED(key)
    *modifiers = 0;
    *virtualKey = 0;
    return false;
#endif
}
