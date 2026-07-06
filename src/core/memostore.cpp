#include "memostore.h"

#include <algorithm>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLocale>
#include <QSaveFile>
#include <QStandardPaths>
#include <QUuid>

namespace {
constexpr auto kDefaultHotkey = "Ctrl+Alt+Space";
constexpr auto kDataFileName = "data.json";
constexpr int kMaxCategoryNameLength = 24;

QString defaultCategoryName(MemoType type)
{
    return type == MemoType::Question ? QStringLiteral("Idea") : QStringLiteral("ToDo");
}

QString normalizedCategoryName(MemoType type, const QString &name)
{
    const QString trimmed = name.trimmed();
    if (trimmed.isEmpty()) {
        return defaultCategoryName(type);
    }
    return trimmed.left(kMaxCategoryNameLength);
}

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

AppLanguage defaultLanguage()
{
    return QLocale::system().language() == QLocale::Chinese ? AppLanguage::ZhCn : AppLanguage::English;
}

bool isActiveRecord(const MemoItem &item)
{
    return !item.completedAt.isValid();
}

void sortRecords(QVector<MemoItem> *records, RecordSortOrder order)
{
    std::stable_sort(records->begin(), records->end(), [order](const MemoItem &left, const MemoItem &right) {
        if (left.createdAt == right.createdAt) {
            return left.id < right.id;
        }

        if (order == RecordSortOrder::OldestFirst) {
            return left.createdAt < right.createdAt;
        }

        return left.createdAt > right.createdAt;
    });
}
}

MemoStore::MemoStore(QObject *parent)
    : QObject(parent)
    , activeType(MemoType::Question)
    , hotkeyText(kDefaultHotkey)
    , autostart(false)
    , hideInputOnSave(false)
    , theme(ThemeMode::System)
    , appLanguage(defaultLanguage())
    , fontSize(FontSizeMode::Default)
    , density(DensityMode::Comfortable)
    , questionCategoryName(defaultCategoryName(MemoType::Question))
    , todoCategoryName(defaultCategoryName(MemoType::Todo))
    , memoStartupDisplay(MemoStartupDisplayMode::Restore)
    , defaultInputType(DefaultInputTypeMode::LastUsed)
    , clickAction(RecordClickAction::Delete)
    , sortOrder(RecordSortOrder::NewestFirst)
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

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return false;
    }
    if (!document.isObject()) {
        return false;
    }

    if (!applyJsonObject(document.object())) {
        return false;
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

    QSaveFile file(dataFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
    return file.commit();
}

QVector<MemoItem> MemoStore::records() const
{
    QVector<MemoItem> activeRecords;
    for (const MemoItem &item : memoRecords) {
        if (isActiveRecord(item)) {
            activeRecords.append(item);
        }
    }
    sortRecords(&activeRecords, sortOrder);
    return activeRecords;
}

QVector<MemoItem> MemoStore::records(MemoType type) const
{
    QVector<MemoItem> filtered;
    for (const MemoItem &item : memoRecords) {
        if (item.type == type && isActiveRecord(item)) {
            filtered.append(item);
        }
    }
    sortRecords(&filtered, sortOrder);
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

bool MemoStore::hideInputAfterSave() const
{
    return hideInputOnSave;
}

void MemoStore::setHideInputAfterSave(bool enabled)
{
    if (hideInputOnSave == enabled) {
        return;
    }

    hideInputOnSave = enabled;
    save();
    emit settingsChanged();
}

ThemeMode MemoStore::themeMode() const
{
    return theme;
}

void MemoStore::setThemeMode(ThemeMode mode)
{
    if (theme == mode) {
        return;
    }

    theme = mode;
    save();
    emit settingsChanged();
}

AppLanguage MemoStore::language() const
{
    return appLanguage;
}

void MemoStore::setLanguage(AppLanguage language)
{
    if (appLanguage == language) {
        return;
    }

    appLanguage = language;
    save();
    emit settingsChanged();
}

FontSizeMode MemoStore::fontSizeMode() const
{
    return fontSize;
}

void MemoStore::setFontSizeMode(FontSizeMode mode)
{
    if (fontSize == mode) {
        return;
    }

    fontSize = mode;
    save();
    emit settingsChanged();
}

DensityMode MemoStore::densityMode() const
{
    return density;
}

void MemoStore::setDensityMode(DensityMode mode)
{
    if (density == mode) {
        return;
    }

    density = mode;
    save();
    emit settingsChanged();
}

QString MemoStore::categoryName(MemoType type) const
{
    return type == MemoType::Question ? questionCategoryName : todoCategoryName;
}

void MemoStore::setCategoryName(MemoType type, const QString &name)
{
    QString &target = type == MemoType::Question ? questionCategoryName : todoCategoryName;
    const QString normalized = normalizedCategoryName(type, name);
    if (target == normalized) {
        return;
    }

    target = normalized;
    save();
    emit settingsChanged();
}

MemoStartupDisplayMode MemoStore::memoStartupDisplayMode() const
{
    return memoStartupDisplay;
}

void MemoStore::setMemoStartupDisplayMode(MemoStartupDisplayMode mode)
{
    if (memoStartupDisplay == mode) {
        return;
    }

    memoStartupDisplay = mode;
    save();
    emit settingsChanged();
}

DefaultInputTypeMode MemoStore::defaultInputTypeMode() const
{
    return defaultInputType;
}

void MemoStore::setDefaultInputTypeMode(DefaultInputTypeMode mode)
{
    if (defaultInputType == mode) {
        return;
    }

    defaultInputType = mode;
    save();
    emit settingsChanged();
}

RecordClickAction MemoStore::recordClickAction() const
{
    return clickAction;
}

void MemoStore::setRecordClickAction(RecordClickAction action)
{
    if (clickAction == action) {
        return;
    }

    clickAction = action;
    save();
    emit settingsChanged();
}

RecordSortOrder MemoStore::recordSortOrder() const
{
    return sortOrder;
}

void MemoStore::setRecordSortOrder(RecordSortOrder order)
{
    if (sortOrder == order) {
        return;
    }

    sortOrder = order;
    save();
    emit settingsChanged();
    emit recordsChanged();
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
        QDateTime::currentDateTime(),
        QDateTime()
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

void MemoStore::completeMemo(const QString &id)
{
    for (MemoItem &item : memoRecords) {
        if (item.id == id) {
            if (!item.completedAt.isValid()) {
                item.completedAt = QDateTime::currentDateTime();
                save();
                emit recordsChanged();
            }
            return;
        }
    }
}

QString MemoStore::dataFilePath() const
{
    return QDir(appDataDir()).filePath(kDataFileName);
}

bool MemoStore::exportToFile(const QString &filePath, QString *errorMessage) const
{
    if (filePath.trimmed().isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("No export path selected.");
        }
        return false;
    }

    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        if (errorMessage != nullptr) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    file.write(QJsonDocument(toJson()).toJson(QJsonDocument::Indented));
    if (!file.commit()) {
        if (errorMessage != nullptr) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    return true;
}

bool MemoStore::importFromFile(const QString &filePath, QString *errorMessage)
{
    const QDir dir(appDataDir());
    if (!dir.exists() && !QDir().mkpath(dir.absolutePath())) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Failed to create app data directory.");
        }
        return false;
    }

    QFile currentData(dataFilePath());
    if (currentData.exists() && !QFile::copy(dataFilePath(), nextBackupFilePath())) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Failed to back up current data.");
        }
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        if (errorMessage != nullptr) {
            *errorMessage = file.errorString();
        }
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        if (errorMessage != nullptr) {
            *errorMessage = parseError.error == QJsonParseError::NoError
                                ? QStringLiteral("Selected file is not a JSON object.")
                                : parseError.errorString();
        }
        return false;
    }

    MemoStore imported;
    if (!imported.applyJsonObject(document.object(), errorMessage)) {
        return false;
    }

    QString writeError;
    if (!imported.exportToFile(dataFilePath(), &writeError)) {
        if (errorMessage != nullptr) {
            *errorMessage = writeError.isEmpty() ? QStringLiteral("Failed to write imported data.") : writeError;
        }
        return false;
    }

    memoRecords = imported.memoRecords;
    activeType = imported.activeType;
    hotkeyText = imported.hotkeyText;
    autostart = imported.autostart;
    hideInputOnSave = imported.hideInputOnSave;
    theme = imported.theme;
    appLanguage = imported.appLanguage;
    fontSize = imported.fontSize;
    density = imported.density;
    questionCategoryName = imported.questionCategoryName;
    todoCategoryName = imported.todoCategoryName;
    memoStartupDisplay = imported.memoStartupDisplay;
    defaultInputType = imported.defaultInputType;
    clickAction = imported.clickAction;
    sortOrder = imported.sortOrder;
    questionState = imported.questionState;
    todoState = imported.todoState;

    emit recordsChanged();
    emit settingsChanged();
    emit windowStateChanged(MemoType::Question);
    emit windowStateChanged(MemoType::Todo);
    return true;
}

QString MemoStore::typeToString(MemoType type)
{
    return type == MemoType::Question ? "question" : "todo";
}

MemoType MemoStore::typeFromString(const QString &value)
{
    return value == "todo" ? MemoType::Todo : MemoType::Question;
}

QString MemoStore::themeToString(ThemeMode mode)
{
    switch (mode) {
    case ThemeMode::System:
        return "system";
    case ThemeMode::Dark:
        return "dark";
    case ThemeMode::Light:
        return "light";
    }

    return "system";
}

ThemeMode MemoStore::themeFromString(const QString &value)
{
    if (value == "dark") {
        return ThemeMode::Dark;
    }
    if (value == "light") {
        return ThemeMode::Light;
    }
    return ThemeMode::System;
}

QString MemoStore::languageToString(AppLanguage language)
{
    return language == AppLanguage::English ? "en" : "zh-CN";
}

AppLanguage MemoStore::languageFromString(const QString &value)
{
    return value == "en" ? AppLanguage::English : AppLanguage::ZhCn;
}

QString MemoStore::fontSizeToString(FontSizeMode mode)
{
    switch (mode) {
    case FontSizeMode::Small:
        return "small";
    case FontSizeMode::Large:
        return "large";
    case FontSizeMode::Default:
        return "default";
    }

    return "default";
}

FontSizeMode MemoStore::fontSizeFromString(const QString &value)
{
    if (value == "small") {
        return FontSizeMode::Small;
    }
    if (value == "large") {
        return FontSizeMode::Large;
    }
    return FontSizeMode::Default;
}

QString MemoStore::densityToString(DensityMode mode)
{
    return mode == DensityMode::Compact ? "compact" : "comfortable";
}

DensityMode MemoStore::densityFromString(const QString &value)
{
    return value == "compact" ? DensityMode::Compact : DensityMode::Comfortable;
}

QString MemoStore::memoStartupDisplayToString(MemoStartupDisplayMode mode)
{
    switch (mode) {
    case MemoStartupDisplayMode::ShowAll:
        return "showAll";
    case MemoStartupDisplayMode::HideAll:
        return "hideAll";
    case MemoStartupDisplayMode::Restore:
        return "restore";
    }

    return "restore";
}

MemoStartupDisplayMode MemoStore::memoStartupDisplayFromString(const QString &value)
{
    if (value == "showAll") {
        return MemoStartupDisplayMode::ShowAll;
    }
    if (value == "hideAll") {
        return MemoStartupDisplayMode::HideAll;
    }
    return MemoStartupDisplayMode::Restore;
}

QString MemoStore::defaultInputTypeToString(DefaultInputTypeMode mode)
{
    switch (mode) {
    case DefaultInputTypeMode::Question:
        return "question";
    case DefaultInputTypeMode::Todo:
        return "todo";
    case DefaultInputTypeMode::LastUsed:
        return "lastUsed";
    }

    return "lastUsed";
}

DefaultInputTypeMode MemoStore::defaultInputTypeFromString(const QString &value)
{
    if (value == "question") {
        return DefaultInputTypeMode::Question;
    }
    if (value == "todo") {
        return DefaultInputTypeMode::Todo;
    }
    return DefaultInputTypeMode::LastUsed;
}

QString MemoStore::recordClickActionToString(RecordClickAction action)
{
    switch (action) {
    case RecordClickAction::Complete:
        return "complete";
    case RecordClickAction::DashboardOnly:
        return "dashboardOnly";
    case RecordClickAction::Delete:
        return "delete";
    }

    return "delete";
}

RecordClickAction MemoStore::recordClickActionFromString(const QString &value)
{
    if (value == "complete") {
        return RecordClickAction::Complete;
    }
    if (value == "dashboardOnly") {
        return RecordClickAction::DashboardOnly;
    }
    return RecordClickAction::Delete;
}

QString MemoStore::recordSortOrderToString(RecordSortOrder order)
{
    return order == RecordSortOrder::OldestFirst ? "oldestFirst" : "newestFirst";
}

RecordSortOrder MemoStore::recordSortOrderFromString(const QString &value)
{
    return value == "oldestFirst" ? RecordSortOrder::OldestFirst : RecordSortOrder::NewestFirst;
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

QJsonObject MemoStore::toJson() const
{
    QJsonArray records;
    for (const MemoItem &item : memoRecords) {
        QJsonObject object{
            {"id", item.id},
            {"type", typeToString(item.type)},
            {"text", item.text},
            {"createdAt", item.createdAt.toString(Qt::ISODate)}
        };
        if (item.completedAt.isValid()) {
            object.insert("completedAt", item.completedAt.toString(Qt::ISODate));
        }
        records.append(object);
    }

    QJsonObject windows;
    windows.insert(typeToString(MemoType::Question), windowStateToJson(questionState));
    windows.insert(typeToString(MemoType::Todo), windowStateToJson(todoState));

    QJsonObject categoryNames;
    categoryNames.insert(typeToString(MemoType::Question), questionCategoryName);
    categoryNames.insert(typeToString(MemoType::Todo), todoCategoryName);

    return {
        {"currentType", typeToString(activeType)},
        {"hotkey", hotkeyText},
        {"autostart", autostart},
        {"hideInputAfterSave", hideInputOnSave},
        {"theme", themeToString(theme)},
        {"language", languageToString(appLanguage)},
        {"fontSize", fontSizeToString(fontSize)},
        {"density", densityToString(density)},
        {"categoryNames", categoryNames},
        {"memoStartupDisplay", memoStartupDisplayToString(memoStartupDisplay)},
        {"defaultInputType", defaultInputTypeToString(defaultInputType)},
        {"recordClickAction", recordClickActionToString(clickAction)},
        {"recordSortOrder", recordSortOrderToString(sortOrder)},
        {"windows", windows},
        {"records", records}
    };
}

bool MemoStore::applyJsonObject(const QJsonObject &root, QString *errorMessage)
{
    const bool looksLikeMemoData = root.contains("records")
                                   || root.contains("windows")
                                   || root.contains("currentType")
                                   || root.contains("hotkey")
                                   || root.contains("categoryNames");
    if (!looksLikeMemoData) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("Selected file does not look like Quick Memo data.");
        }
        return false;
    }

    if (root.contains("records") && !root.value("records").isArray()) {
        if (errorMessage != nullptr) {
            *errorMessage = QStringLiteral("The records field must be an array.");
        }
        return false;
    }

    QVector<MemoItem> parsedRecords;
    const QJsonArray records = root.value("records").toArray();
    for (const QJsonValue &value : records) {
        if (!value.isObject()) {
            continue;
        }

        const QJsonObject object = value.toObject();
        MemoItem item;
        item.id = object.value("id").toString();
        item.type = typeFromString(object.value("type").toString());
        item.text = object.value("text").toString().trimmed();
        item.createdAt = QDateTime::fromString(object.value("createdAt").toString(), Qt::ISODate);
        item.completedAt = QDateTime::fromString(object.value("completedAt").toString(), Qt::ISODate);

        if (item.id.isEmpty() || item.text.isEmpty()) {
            continue;
        }
        if (!item.createdAt.isValid()) {
            item.createdAt = QDateTime::currentDateTime();
        }
        parsedRecords.append(item);
    }

    const QJsonObject categoryNames = root.value("categoryNames").toObject();
    const QJsonObject windows = root.value("windows").toObject();
    const QString importedHotkey = root.value("hotkey").toString(kDefaultHotkey).trimmed();

    memoRecords = parsedRecords;
    activeType = typeFromString(root.value("currentType").toString(typeToString(MemoType::Question)));
    hotkeyText = importedHotkey.isEmpty() ? QString::fromLatin1(kDefaultHotkey) : importedHotkey;
    autostart = root.value("autostart").toBool(false);
    hideInputOnSave = root.value("hideInputAfterSave").toBool(false);
    theme = themeFromString(root.value("theme").toString(themeToString(ThemeMode::System)));
    appLanguage = languageFromString(root.value("language").toString(languageToString(defaultLanguage())));
    fontSize = fontSizeFromString(root.value("fontSize").toString(fontSizeToString(FontSizeMode::Default)));
    density = densityFromString(root.value("density").toString(densityToString(DensityMode::Comfortable)));
    memoStartupDisplay = memoStartupDisplayFromString(
        root.value("memoStartupDisplay").toString(memoStartupDisplayToString(MemoStartupDisplayMode::Restore)));
    defaultInputType = defaultInputTypeFromString(
        root.value("defaultInputType").toString(defaultInputTypeToString(DefaultInputTypeMode::LastUsed)));
    clickAction = recordClickActionFromString(
        root.value("recordClickAction").toString(recordClickActionToString(RecordClickAction::Delete)));
    sortOrder = recordSortOrderFromString(
        root.value("recordSortOrder").toString(recordSortOrderToString(RecordSortOrder::NewestFirst)));

    questionCategoryName = normalizedCategoryName(
        MemoType::Question,
        categoryNames.value(typeToString(MemoType::Question)).toString(defaultCategoryName(MemoType::Question)));
    todoCategoryName = normalizedCategoryName(
        MemoType::Todo,
        categoryNames.value(typeToString(MemoType::Todo)).toString(defaultCategoryName(MemoType::Todo)));

    questionState = windowStateFromJson(windows.value(typeToString(MemoType::Question)).toObject(),
                                        defaultWindowState(MemoType::Question));
    todoState = windowStateFromJson(windows.value(typeToString(MemoType::Todo)).toObject(),
                                    defaultWindowState(MemoType::Todo));
    return true;
}

QString MemoStore::nextBackupFilePath() const
{
    const QDir dir(appDataDir());
    const QString stamp = QDateTime::currentDateTime().toString("yyyyMMdd-HHmmss");
    QString path = dir.filePath(QStringLiteral("data.backup-%1.json").arg(stamp));
    int suffix = 1;
    while (QFile::exists(path)) {
        path = dir.filePath(QStringLiteral("data.backup-%1-%2.json").arg(stamp).arg(suffix));
        ++suffix;
    }
    return path;
}
