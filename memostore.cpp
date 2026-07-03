#include "memostore.h"

#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QUuid>

namespace {
constexpr auto kDefaultHotkey = "Ctrl+Alt+Space";
constexpr auto kDataFileName = "data.json";

QJsonObject geometryToJson(const QRect &geometry)
{
    return {
        {"x", geometry.x()},
        {"y", geometry.y()},
        {"w", geometry.width()},
        {"h", geometry.height()}
    };
}

QRect geometryFromJson(const QJsonObject &object, const QRect &fallback)
{
    const int x = object.value("x").toInt(fallback.x());
    const int y = object.value("y").toInt(fallback.y());
    const int width = object.value("w").toInt(fallback.width());
    const int height = object.value("h").toInt(fallback.height());

    if (width < 220 || height < 180) {
        return fallback;
    }

    return QRect(x, y, width, height);
}

QJsonObject windowStateToJson(const MemoWindowState &state)
{
    return {
        {"geometry", geometryToJson(state.geometry)},
        {"visible", state.visible},
        {"alwaysOnTop", state.alwaysOnTop}
    };
}

MemoWindowState windowStateFromJson(const QJsonObject &object, const MemoWindowState &fallback)
{
    MemoWindowState state = fallback;
    state.geometry = geometryFromJson(object.value("geometry").toObject(), fallback.geometry);
    state.visible = object.value("visible").toBool(fallback.visible);
    state.alwaysOnTop = object.value("alwaysOnTop").toBool(fallback.alwaysOnTop);
    return state;
}
}

MemoStore::MemoStore(QObject *parent)
    : QObject(parent)
    , activeType(MemoType::Question)
    , hotkeyText(kDefaultHotkey)
    , autostart(false)
    , questionState(defaultWindowState(MemoType::Question))
    , todoState(defaultWindowState(MemoType::Todo))
{
}

bool MemoStore::load()
{
    QFile file(dataFilePath());
    if (!file.exists()) {
        return save();
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject()) {
        return false;
    }

    const QJsonObject root = document.object();
    memoRecords.clear();
    activeType = typeFromString(root.value("currentType").toString(typeToString(MemoType::Question)));
    hotkeyText = root.value("hotkey").toString(kDefaultHotkey);
    autostart = root.value("autostart").toBool(false);

    const QJsonObject windows = root.value("windows").toObject();
    questionState = windowStateFromJson(windows.value(typeToString(MemoType::Question)).toObject(),
                                        defaultWindowState(MemoType::Question));
    todoState = windowStateFromJson(windows.value(typeToString(MemoType::Todo)).toObject(),
                                    defaultWindowState(MemoType::Todo));

    const QJsonArray records = root.value("records").toArray();
    for (const QJsonValue &value : records) {
        const QJsonObject object = value.toObject();
        MemoItem item;
        item.id = object.value("id").toString();
        item.type = typeFromString(object.value("type").toString());
        item.text = object.value("text").toString().trimmed();
        item.createdAt = QDateTime::fromString(object.value("createdAt").toString(), Qt::ISODate);

        if (item.id.isEmpty() || item.text.isEmpty()) {
            continue;
        }
        if (!item.createdAt.isValid()) {
            item.createdAt = QDateTime::currentDateTime();
        }
        memoRecords.append(item);
    }

    emit recordsChanged();
    emit settingsChanged();
    emit windowStateChanged(MemoType::Question);
    emit windowStateChanged(MemoType::Todo);
    return true;
}

bool MemoStore::save() const
{
    const QDir dir(appDataDir());
    if (!dir.exists() && !QDir().mkpath(dir.absolutePath())) {
        return false;
    }

    QJsonArray records;
    for (const MemoItem &item : memoRecords) {
        records.append(QJsonObject{
            {"id", item.id},
            {"type", typeToString(item.type)},
            {"text", item.text},
            {"createdAt", item.createdAt.toString(Qt::ISODate)}
        });
    }

    QJsonObject windows;
    windows.insert(typeToString(MemoType::Question), windowStateToJson(questionState));
    windows.insert(typeToString(MemoType::Todo), windowStateToJson(todoState));

    const QJsonObject root{
        {"currentType", typeToString(activeType)},
        {"hotkey", hotkeyText},
        {"autostart", autostart},
        {"windows", windows},
        {"records", records}
    };

    QFile file(dataFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

QVector<MemoItem> MemoStore::records() const
{
    return memoRecords;
}

QVector<MemoItem> MemoStore::records(MemoType type) const
{
    QVector<MemoItem> filtered;
    for (const MemoItem &item : memoRecords) {
        if (item.type == type) {
            filtered.append(item);
        }
    }
    return filtered;
}

MemoType MemoStore::currentType() const
{
    return activeType;
}

void MemoStore::setCurrentType(MemoType type)
{
    if (activeType == type) {
        return;
    }

    activeType = type;
    save();
    emit settingsChanged();
}

QString MemoStore::hotkey() const
{
    return hotkeyText;
}

void MemoStore::setHotkey(const QString &hotkey)
{
    if (hotkeyText == hotkey) {
        return;
    }

    hotkeyText = hotkey;
    save();
    emit settingsChanged();
}

bool MemoStore::autostartEnabled() const
{
    return autostart;
}

void MemoStore::setAutostartEnabled(bool enabled)
{
    if (autostart == enabled) {
        return;
    }

    autostart = enabled;
    save();
    emit settingsChanged();
}

MemoWindowState MemoStore::windowState(MemoType type) const
{
    return type == MemoType::Question ? questionState : todoState;
}

void MemoStore::setWindowState(MemoType type, const MemoWindowState &state)
{
    MemoWindowState &target = type == MemoType::Question ? questionState : todoState;
    if (target.geometry == state.geometry
        && target.visible == state.visible
        && target.alwaysOnTop == state.alwaysOnTop) {
        return;
    }

    target = state;
    save();
    emit windowStateChanged(type);
}

void MemoStore::addMemo(MemoType type, const QString &text)
{
    const QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        return;
    }

    memoRecords.append(MemoItem{
        QUuid::createUuid().toString(QUuid::WithoutBraces),
        type,
        trimmed,
        QDateTime::currentDateTime()
    });
    save();
    emit recordsChanged();
}

void MemoStore::deleteMemo(const QString &id)
{
    for (auto it = memoRecords.begin(); it != memoRecords.end(); ++it) {
        if (it->id == id) {
            memoRecords.erase(it);
            save();
            emit recordsChanged();
            return;
        }
    }
}

QString MemoStore::dataFilePath() const
{
    return QDir(appDataDir()).filePath(kDataFileName);
}

QString MemoStore::typeToString(MemoType type)
{
    return type == MemoType::Question ? "question" : "todo";
}

MemoType MemoStore::typeFromString(const QString &value)
{
    return value == "todo" ? MemoType::Todo : MemoType::Question;
}

QString MemoStore::displayName(MemoType type)
{
    return type == MemoType::Question ? QStringLiteral("问题") : QStringLiteral("待办");
}

MemoWindowState MemoStore::defaultWindowState(MemoType type) const
{
    MemoWindowState state;
    state.geometry = type == MemoType::Question ? QRect(80, 80, 340, 420)
                                                : QRect(440, 80, 340, 420);
    state.visible = true;
    state.alwaysOnTop = true;
    return state;
}

QString MemoStore::appDataDir() const
{
    const QString appData = qEnvironmentVariable("APPDATA");
    if (!appData.isEmpty()) {
        return QDir(appData).filePath("QuickMemoApplet");
    }

    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}
