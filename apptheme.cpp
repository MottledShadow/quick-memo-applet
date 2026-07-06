#include "apptheme.h"

#include <QColor>
#include <QGraphicsDropShadowEffect>
#include <QSettings>
#include <QWidget>

namespace {
struct Palette {
    QString primary;
    QString primaryHover;
    QString primarySoft;
    QString primaryText;

    QString secondary;
    QString secondaryHover;
    QString secondarySoft;
    QString secondaryText;

    QString appBg;
    QString appBgGradient;
    QString surface;
    QString surfaceGradient;
    QString surfaceRaised;
    QString surfaceHighlight;
    QString surfaceSunken;
    QString memoPanel;
    QString memoPanelTop;
    QString memoTitleGradient;
    QString memoList;
    QString memoCard;
    QString memoCardHover;
    QString memoPaper;
    QString memoPaperHover;
    QString memoPaperPressed;
    QString memoPaperBorder;
    QString memoPaperLine;
    QString memoPaperText;
    QString memoPaperTimeText;
    QString memoBorder;
    QString memoBorderStrong;
    QString memoText;
    QString memoTimeText;

    QString textPrimary;
    QString textSecondary;
    QString textMuted;
    QString textDisabled;

    QString border;
    QString borderStrong;
    QString overlayHover;
    QString overlayPressed;

    QString success;
    QString successSoft;
    QString warning;
    QString warningSoft;
    QString danger;
    QString dangerSoft;
    QString info;
    QString infoSoft;
};

struct TypeScale {
    QString fontFamily;
    QString captionSize;
    QString bodySize;
    QString bodyLargeSize;
    QString memoTitleSize;
    QString sectionSize;
    QString titleSize;
    QString pageTitleSize;
    QString weightRegular;
    QString weightMedium;
    QString weightControl;
    QString weightTitle;
    QString lineCaption;
    QString lineBody;
    QString lineBodyLarge;
    QString lineTitle;
};

struct SpacingScale {
    QString xs;
    QString s;
    QString m;
    QString l;
    QString xl;
    QString controlHeight;
    QString compactControlHeight;
    QString radiusS;
    QString radiusM;
    QString radiusL;
};

struct ElevationSpec {
    qreal blurRadius;
    qreal xOffset;
    qreal yOffset;
    QColor color;
};

ThemeMode resolveThemeMode(ThemeMode mode)
{
    if (mode != ThemeMode::System) {
        return mode;
    }

#ifdef Q_OS_WIN
    QSettings settings(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
                       QSettings::NativeFormat);
    const QVariant value = settings.value(QStringLiteral("AppsUseLightTheme"));
    if (value.isValid()) {
        return value.toInt() == 0 ? ThemeMode::Dark : ThemeMode::Light;
    }
#endif

    return ThemeMode::Light;
}

Palette palette(ThemeMode mode)
{
    mode = resolveThemeMode(mode);
    if (mode == ThemeMode::Dark) {
        return {
            "#60A5FA", // primary
            "#93C5FD", // primaryHover
            "#172A46", // primarySoft
            "#0B1220", // primaryText

            "#34D399", // secondary
            "#6EE7B7", // secondaryHover
            "#12372B", // secondarySoft
            "#052E1B", // secondaryText

            "#0B1220", // appBg
            "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #0F172A, stop:1 #0B1220)", // appBgGradient
            "#111827", // surface
            "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #182235, stop:0.55 #111827, stop:1 #0F172A)", // surfaceGradient
            "#1F2937", // surfaceRaised
            "#263449", // surfaceHighlight
            "#0F172A", // surfaceSunken
            "#0F172A", // memoPanel
            "#111827", // memoPanelTop
            "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #182235, stop:1 #111827)", // memoTitleGradient
            "#0B1220", // memoList
            "#1E293B", // memoCard
            "#263449", // memoCardHover
            "#FFF7D6", // memoPaper
            "#FFF1BC", // memoPaperHover
            "#FDE68A", // memoPaperPressed
            "#D6C479", // memoPaperBorder
            "#CDBB73", // memoPaperLine
            "#1F2937", // memoPaperText
            "#6B5F37", // memoPaperTimeText
            "#334155", // memoBorder
            "#475569", // memoBorderStrong
            "#F8FAFC", // memoText
            "#CBD5E1", // memoTimeText

            "#F9FAFB", // textPrimary
            "#CBD5E1", // textSecondary
            "#94A3B8", // textMuted
            "#64748B", // textDisabled

            "#334155", // border
            "#475569", // borderStrong
            "#1F2937", // overlayHover
            "#293548", // overlayPressed

            "#34D399", // success
            "#12372B", // successSoft
            "#FBBF24", // warning
            "#3A2A11", // warningSoft
            "#F87171", // danger
            "#3B1518", // dangerSoft
            "#60A5FA", // info
            "#172A46"  // infoSoft
        };
    }

    return {
        "#2563EB", // primary
        "#1D4ED8", // primaryHover
        "#DBEAFE", // primarySoft
        "#FFFFFF", // primaryText

        "#047857", // secondary
        "#065F46", // secondaryHover
        "#D1FAE5", // secondarySoft
        "#FFFFFF", // secondaryText

        "#F3F5F7", // appBg
        "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #F8FAFC, stop:1 #F3F5F7)", // appBgGradient
        "#FFFFFF", // surface
        "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:0.58 #FFFFFF, stop:1 #F4F7FB)", // surfaceGradient
        "#F8FAFC", // surfaceRaised
        "#FFFFFF", // surfaceHighlight
        "#EEF2F7", // surfaceSunken
        "#FFFFFF", // memoPanel
        "#F8FAFC", // memoPanelTop
        "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #F8FAFC)", // memoTitleGradient
        "#F1F5F9", // memoList
        "#FFFFFF", // memoCard
        "#F8FAFC", // memoCardHover
        "#FFFDF2", // memoPaper
        "#FFF8DD", // memoPaperHover
        "#FEF3C7", // memoPaperPressed
        "#E8DCA8", // memoPaperBorder
        "#E3D69B", // memoPaperLine
        "#111827", // memoPaperText
        "#7C6F3F", // memoPaperTimeText
        "#E2E8F0", // memoBorder
        "#CBD5E1", // memoBorderStrong
        "#111827", // memoText
        "#475569", // memoTimeText

        "#111827", // textPrimary
        "#4B5563", // textSecondary
        "#64748B", // textMuted
        "#9CA3AF", // textDisabled

        "#D7DEE8", // border
        "#A7B1C2", // borderStrong
        "#F1F5F9", // overlayHover
        "#E2E8F0", // overlayPressed

        "#047857", // success
        "#D1FAE5", // successSoft
        "#B45309", // warning
        "#FEF3C7", // warningSoft
        "#B91C1C", // danger
        "#FEE2E2", // dangerSoft
        "#2563EB", // info
        "#DBEAFE"  // infoSoft
    };
}

SpacingScale spacing(DensityMode density)
{
    if (density == DensityMode::Compact) {
        return {
            "4px", // xs
            "6px", // s
            "12px", // m
            "20px", // l
            "28px", // xl
            "30px", // controlHeight
            "26px", // compactControlHeight
            "8px", // radiusS
            "8px", // radiusM
            "8px"  // radiusL
        };
    }

    return {
        "4px", // xs
        "8px", // s
        "16px", // m
        "24px", // l
        "32px", // xl
        "32px", // controlHeight
        "28px", // compactControlHeight
        "8px", // radiusS
        "8px", // radiusM
        "8px"  // radiusL
    };
}

TypeScale typography(FontSizeMode fontSize)
{
    if (fontSize == FontSizeMode::Small) {
        return {
            "\"Segoe UI\", \"Microsoft YaHei UI\", \"Microsoft YaHei\", sans-serif", // fontFamily
            "10px", // captionSize
            "12px", // bodySize
            "13px", // bodyLargeSize
            "15px", // memoTitleSize
            "14px", // sectionSize
            "17px", // titleSize
            "20px", // pageTitleSize
            "400", // weightRegular
            "500", // weightMedium
            "600", // weightControl
            "700", // weightTitle
            "15px", // lineCaption
            "17px", // lineBody
            "19px", // lineBodyLarge
            "22px"  // lineTitle
        };
    }

    if (fontSize == FontSizeMode::Large) {
        return {
            "\"Segoe UI\", \"Microsoft YaHei UI\", \"Microsoft YaHei\", sans-serif", // fontFamily
            "12px", // captionSize
            "14px", // bodySize
            "15px", // bodyLargeSize
            "17px", // memoTitleSize
            "16px", // sectionSize
            "19px", // titleSize
            "23px", // pageTitleSize
            "400", // weightRegular
            "500", // weightMedium
            "600", // weightControl
            "700", // weightTitle
            "17px", // lineCaption
            "20px", // lineBody
            "22px", // lineBodyLarge
            "26px"  // lineTitle
        };
    }

    return {
        "\"Segoe UI\", \"Microsoft YaHei UI\", \"Microsoft YaHei\", sans-serif", // fontFamily
        "11px", // captionSize
        "13px", // bodySize
        "14px", // bodyLargeSize
        "16px", // memoTitleSize
        "15px", // sectionSize
        "18px", // titleSize
        "21px", // pageTitleSize
        "400", // weightRegular
        "500", // weightMedium
        "600", // weightControl
        "700", // weightTitle
        "16px", // lineCaption
        "18px", // lineBody
        "20px", // lineBodyLarge
        "24px"  // lineTitle
    };
}

QString themed(QString style, const Palette &p, FontSizeMode fontSize, DensityMode density)
{
    const TypeScale t = typography(fontSize);
    const SpacingScale s = spacing(density);
    return style
        .replace("${primary}", p.primary)
        .replace("${primaryHover}", p.primaryHover)
        .replace("${primarySoft}", p.primarySoft)
        .replace("${primaryText}", p.primaryText)
        .replace("${secondary}", p.secondary)
        .replace("${secondaryHover}", p.secondaryHover)
        .replace("${secondarySoft}", p.secondarySoft)
        .replace("${secondaryText}", p.secondaryText)
        .replace("${appBg}", p.appBg)
        .replace("${appBgGradient}", p.appBgGradient)
        .replace("${surface}", p.surface)
        .replace("${surfaceGradient}", p.surfaceGradient)
        .replace("${surfaceRaised}", p.surfaceRaised)
        .replace("${surfaceHighlight}", p.surfaceHighlight)
        .replace("${surfaceSunken}", p.surfaceSunken)
        .replace("${memoPanel}", p.memoPanel)
        .replace("${memoPanelTop}", p.memoPanelTop)
        .replace("${memoTitleGradient}", p.memoTitleGradient)
        .replace("${memoList}", p.memoList)
        .replace("${memoCard}", p.memoCard)
        .replace("${memoCardHover}", p.memoCardHover)
        .replace("${memoPaper}", p.memoPaper)
        .replace("${memoPaperHover}", p.memoPaperHover)
        .replace("${memoPaperPressed}", p.memoPaperPressed)
        .replace("${memoPaperBorder}", p.memoPaperBorder)
        .replace("${memoPaperLine}", p.memoPaperLine)
        .replace("${memoPaperText}", p.memoPaperText)
        .replace("${memoPaperTimeText}", p.memoPaperTimeText)
        .replace("${memoBorder}", p.memoBorder)
        .replace("${memoBorderStrong}", p.memoBorderStrong)
        .replace("${memoText}", p.memoText)
        .replace("${memoTimeText}", p.memoTimeText)
        .replace("${textPrimary}", p.textPrimary)
        .replace("${textSecondary}", p.textSecondary)
        .replace("${textMuted}", p.textMuted)
        .replace("${textDisabled}", p.textDisabled)
        .replace("${border}", p.border)
        .replace("${borderStrong}", p.borderStrong)
        .replace("${overlayHover}", p.overlayHover)
        .replace("${overlayPressed}", p.overlayPressed)
        .replace("${success}", p.success)
        .replace("${successSoft}", p.successSoft)
        .replace("${warning}", p.warning)
        .replace("${warningSoft}", p.warningSoft)
        .replace("${danger}", p.danger)
        .replace("${dangerSoft}", p.dangerSoft)
        .replace("${info}", p.info)
        .replace("${infoSoft}", p.infoSoft)
        .replace("${fontFamily}", t.fontFamily)
        .replace("${captionSize}", t.captionSize)
        .replace("${bodySize}", t.bodySize)
        .replace("${bodyLargeSize}", t.bodyLargeSize)
        .replace("${memoTitleSize}", t.memoTitleSize)
        .replace("${sectionSize}", t.sectionSize)
        .replace("${titleSize}", t.titleSize)
        .replace("${pageTitleSize}", t.pageTitleSize)
        .replace("${weightRegular}", t.weightRegular)
        .replace("${weightMedium}", t.weightMedium)
        .replace("${weightControl}", t.weightControl)
        .replace("${weightTitle}", t.weightTitle)
        .replace("${lineCaption}", t.lineCaption)
        .replace("${lineBody}", t.lineBody)
        .replace("${lineBodyLarge}", t.lineBodyLarge)
        .replace("${lineTitle}", t.lineTitle)
        .replace("${spaceXs}", s.xs)
        .replace("${spaceS}", s.s)
        .replace("${spaceM}", s.m)
        .replace("${spaceL}", s.l)
        .replace("${spaceXl}", s.xl)
        .replace("${controlHeight}", s.controlHeight)
        .replace("${compactControlHeight}", s.compactControlHeight)
        .replace("${radiusS}", s.radiusS)
        .replace("${radiusM}", s.radiusM)
        .replace("${radiusL}", s.radiusL);
}

QString scrollBars(const Palette &p, FontSizeMode fontSize, DensityMode density)
{
    return themed(R"(
QScrollBar:vertical {
    background: transparent;
    width: ${spaceS};
    margin: 0;
}
QScrollBar::handle:vertical {
    background: ${borderStrong};
    min-height: ${compactControlHeight};
    border-radius: ${radiusS};
}
QScrollBar::handle:vertical:hover {
    background: ${textMuted};
}
QScrollBar::handle:vertical:pressed {
    background: ${overlayPressed};
}
QScrollBar::handle:vertical:disabled {
    background: transparent;
}
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
}
QScrollBar::add-page:vertical,
QScrollBar::sub-page:vertical {
    background: transparent;
}
QScrollBar:horizontal {
    background: transparent;
    height: ${spaceS};
    margin: 0;
}
QScrollBar::handle:horizontal {
    background: ${borderStrong};
    min-width: ${compactControlHeight};
    border-radius: ${radiusS};
}
QScrollBar::handle:horizontal:hover {
    background: ${textMuted};
}
QScrollBar::handle:horizontal:pressed {
    background: ${overlayPressed};
}
QScrollBar::handle:horizontal:disabled {
    background: transparent;
}
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal {
    width: 0;
}
QScrollBar::add-page:horizontal,
QScrollBar::sub-page:horizontal {
    background: transparent;
}
QAbstractScrollArea::corner {
    background: transparent;
}
)", p, fontSize, density);
}

ElevationSpec elevationSpec(ThemeMode mode, ElevationLevel level)
{
    mode = resolveThemeMode(mode);
    if (level == ElevationLevel::E0) {
        return {0, 0, 0, QColor(0, 0, 0, 0)};
    }

    if (mode == ThemeMode::Dark) {
        switch (level) {
        case ElevationLevel::E1:
            return {12, 0, 2, QColor(8, 13, 24, 78)};
        case ElevationLevel::E2:
            return {18, 0, 5, QColor(8, 13, 24, 96)};
        case ElevationLevel::E3:
            return {24, 0, 7, QColor(2, 6, 23, 118)};
        case ElevationLevel::E0:
            break;
        }
    }

    switch (level) {
    case ElevationLevel::E1:
        return {10, 0, 2, QColor(71, 85, 105, 28)};
    case ElevationLevel::E2:
        return {16, 0, 4, QColor(71, 85, 105, 38)};
    case ElevationLevel::E3:
        return {22, 0, 6, QColor(51, 65, 85, 48)};
    case ElevationLevel::E0:
        break;
    }

    return {0, 0, 0, QColor(0, 0, 0, 0)};
}
}

ThemeMode AppTheme::effectiveThemeMode(ThemeMode mode)
{
    return resolveThemeMode(mode);
}

void AppTheme::applyElevation(QWidget *widget, ThemeMode mode, ElevationLevel level)
{
    if (widget == nullptr || level == ElevationLevel::E0) {
        if (widget != nullptr) {
            widget->setGraphicsEffect(nullptr);
        }
        return;
    }

    const ElevationSpec spec = elevationSpec(mode, level);
    auto *effect = new QGraphicsDropShadowEffect(widget);
    effect->setBlurRadius(spec.blurRadius);
    effect->setOffset(spec.xOffset, spec.yOffset);
    effect->setColor(spec.color);
    widget->setGraphicsEffect(effect);
}

QColor AppTheme::memoAccentColor(MemoType type, ThemeMode mode)
{
    const Palette p = palette(mode);
    return QColor(type == MemoType::Todo ? p.secondary : p.primary);
}

QColor AppTheme::memoPaperLineColor(ThemeMode mode)
{
    return QColor(palette(mode).memoPaperLine);
}

QString AppTheme::applicationStyleSheet(ThemeMode mode, FontSizeMode fontSize, DensityMode density)
{
    const Palette p = palette(mode);
    return themed(R"(
QToolTip {
    color: ${textPrimary};
    background: ${surfaceRaised};
    border: 1px solid ${borderStrong};
    border-radius: ${radiusM};
    padding: ${spaceS};
    font-family: ${fontFamily};
    font-size: ${bodySize};
}
QMenu {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    padding: ${spaceXs};
    font-family: ${fontFamily};
    font-size: ${bodySize};
}
QMenu::item {
    padding: ${spaceS} ${spaceL};
    border-radius: ${radiusS};
}
QMenu::item:selected {
    color: ${textPrimary};
    background: ${primarySoft};
}
QMenu::item:disabled {
    color: ${textDisabled};
}
QMenu::separator {
    height: 1px;
    background: ${border};
    margin: ${spaceXs} ${spaceS};
}
)", p, fontSize, density) + scrollBars(p, fontSize, density);
}

QString AppTheme::memoWindowStyleSheet(ThemeMode mode, FontSizeMode fontSize, DensityMode density)
{
    const Palette p = palette(mode);
    return themed(R"(
MemoWindow {
    color: ${memoText};
    background: transparent;
    border: none;
}
MemoWindow QFrame#MemoPanel {
    color: ${memoPaperText};
    background: ${memoPaper};
    border: 1px solid ${memoPaperBorder};
    border-radius: ${radiusM};
}
MemoWindow QWidget#TitleBar {
    background: transparent;
    border: none;
    border-top-left-radius: ${radiusM};
    border-top-right-radius: ${radiusM};
    border-bottom-left-radius: 0;
    border-bottom-right-radius: 0;
    min-height: 46px;
}
MemoWindow QLabel#TitleLabel {
    color: ${memoPaperText};
    background: transparent;
    font-family: ${fontFamily};
    font-size: ${bodyLargeSize};
    font-weight: ${weightControl};
    line-height: ${lineBodyLarge};
}
MemoWindow[memoKind="question"] QLabel#TitleLabel {
    color: ${primary};
}
MemoWindow[memoKind="todo"] QLabel#TitleLabel {
    color: ${secondary};
}
MemoWindow QLabel#TitleCount {
    color: ${memoPaperTimeText};
    background: rgba(255, 255, 255, 70);
    border: 1px solid ${memoPaperBorder};
    border-radius: 7px;
    padding: 0 5px;
    min-width: 12px;
    min-height: 16px;
    font-family: ${fontFamily};
    font-size: ${captionSize};
    font-weight: ${weightControl};
    line-height: ${lineCaption};
}
MemoWindow QPushButton {
    color: ${memoPaperTimeText};
    background: transparent;
    border: 1px solid transparent;
    border-radius: ${radiusS};
    padding: 0;
    min-height: 24px;
    min-width: 24px;
    font-family: ${fontFamily};
    font-size: ${captionSize};
    font-weight: ${weightControl};
    line-height: ${lineBody};
}
MemoWindow QPushButton#TopButton {
    color: ${memoPaperTimeText};
    max-width: 24px;
    min-width: 24px;
}
MemoWindow QPushButton#TopButton[active="true"] {
    color: ${primary};
    background: transparent;
    border-color: transparent;
}
MemoWindow[memoKind="todo"] QPushButton#TopButton[active="true"] {
    color: ${secondary};
    background: transparent;
    border-color: transparent;
}
MemoWindow QPushButton#HideButton {
    color: ${memoPaperTimeText};
    max-width: 24px;
    min-width: 24px;
    padding: 0;
    font-size: ${captionSize};
}
MemoWindow QPushButton:hover {
    color: ${memoPaperText};
    background: rgba(255, 255, 255, 82);
    border-color: ${memoPaperBorder};
}
MemoWindow QPushButton:focus {
    color: ${memoPaperText};
    background: rgba(255, 255, 255, 82);
    border-color: ${primary};
}
MemoWindow QPushButton#TopButton[active="true"]:hover {
    color: ${primaryHover};
    background: rgba(255, 255, 255, 82);
    border-color: ${memoPaperBorder};
}
MemoWindow[memoKind="todo"] QPushButton#TopButton[active="true"]:hover {
    color: ${secondaryHover};
    background: rgba(255, 255, 255, 82);
    border-color: ${memoPaperBorder};
}
MemoWindow[memoKind="todo"] QPushButton:focus {
    border-color: ${secondary};
}
MemoWindow QPushButton#HideButton:hover {
    color: ${danger};
    background: ${dangerSoft};
    border-color: ${danger};
}
MemoWindow QPushButton#HideButton:focus {
    color: ${danger};
    background: ${dangerSoft};
    border-color: ${danger};
}
MemoWindow QPushButton:pressed {
    background: rgba(253, 230, 138, 110);
}
MemoWindow QScrollArea#MemoScrollArea {
    background: transparent;
    border: none;
}
MemoWindow QWidget#MemoList {
    background: transparent;
    border: none;
}
MemoWindow QFrame#MemoRecordCard {
    background: transparent;
    border: 1px solid transparent;
    border-radius: ${radiusS};
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"] {
    border-color: transparent;
}
MemoWindow QFrame#MemoRecordCard:hover {
    background: transparent;
    border-color: transparent;
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"]:hover {
    border-color: transparent;
}
MemoWindow QFrame#MemoRecordCard[pressed="true"] {
    background: rgba(253, 230, 138, 42);
    border-color: transparent;
}
MemoWindow QFrame#StrikeLine {
    background: ${primary};
    border: none;
    min-height: 2px;
    max-height: 2px;
}
MemoWindow QFrame#StrikeLine[memoKind="todo"] {
    background: ${secondary};
}
MemoWindow QLabel#RecordText {
    color: ${memoPaperText};
    background: transparent;
    font-family: ${fontFamily};
    font-size: ${bodyLargeSize};
    font-weight: ${weightMedium};
    line-height: ${lineBodyLarge};
}
MemoWindow QSizeGrip#ResizeGrip {
    background: transparent;
    width: ${spaceM};
    height: ${spaceM};
}
)", p, fontSize, density);
}

QString AppTheme::inputWindowStyleSheet(ThemeMode mode, FontSizeMode fontSize, DensityMode density)
{
    const Palette p = palette(mode);
    return themed(R"(
InputWindow {
    background: transparent;
}
InputWindow QFrame#InputPanel {
    background: ${surfaceGradient};
    border: 1px solid ${borderStrong};
    border-left: 3px solid ${primary};
    border-top-color: ${surfaceHighlight};
    border-radius: ${radiusL};
}
InputWindow QFrame#InputPanel[memoKind="todo"] {
    border-left-color: ${secondary};
}
InputWindow QFrame#InputPanel[focused="true"] {
    border-color: ${primary};
    border-left-color: ${primary};
}
InputWindow QFrame#InputPanel[memoKind="todo"][focused="true"] {
    border-color: ${secondary};
    border-left-color: ${secondary};
}
InputWindow QLineEdit {
    color: ${textPrimary};
    background: transparent;
    border: none;
    border-radius: 0;
    padding: 0 ${spaceS};
    font-family: ${fontFamily};
    font-size: ${bodyLargeSize};
    font-weight: ${weightRegular};
    line-height: ${lineBodyLarge};
    selection-color: ${primaryText};
    selection-background-color: ${primary};
    min-height: 32px;
}
InputWindow QLineEdit:focus {
    border: none;
}
InputWindow QLineEdit::placeholder {
    color: ${textMuted};
}
InputWindow QPushButton {
    color: ${primary};
    background: ${primarySoft};
    border: 1px solid ${primary};
    border-radius: 16px;
    padding: 0 12px;
    font-family: ${fontFamily};
    font-size: ${bodySize};
    font-weight: ${weightControl};
    line-height: ${lineBody};
    min-height: 32px;
}
InputWindow QPushButton#TypeButton {
    min-width: 78px;
    max-height: 32px;
    color: ${primary};
    background: ${primarySoft};
    border-color: ${primary};
    font-weight: ${weightControl};
}
InputWindow QPushButton#TypeButton[memoKind="todo"] {
    color: ${secondary};
    background: ${secondarySoft};
    border-color: ${secondary};
}
InputWindow QPushButton:hover {
    color: ${primaryHover};
    background: ${primarySoft};
    border-color: ${primaryHover};
}
InputWindow QPushButton:focus {
    color: ${primaryHover};
    background: ${primarySoft};
    border-color: ${primaryHover};
}
InputWindow QPushButton#TypeButton:hover {
    color: ${primaryHover};
    background: ${primarySoft};
    border-color: ${primaryHover};
}
InputWindow QPushButton#TypeButton:focus {
    color: ${primaryHover};
    background: ${primarySoft};
    border-color: ${primaryHover};
}
InputWindow QPushButton#TypeButton[memoKind="todo"]:hover {
    color: ${secondaryHover};
    background: ${secondarySoft};
    border-color: ${secondaryHover};
}
InputWindow QPushButton#TypeButton[memoKind="todo"]:focus {
    color: ${secondaryHover};
    background: ${secondarySoft};
    border-color: ${secondaryHover};
}
InputWindow QPushButton:pressed {
    background: ${overlayPressed};
}
InputWindow QLabel#ShortcutHint {
    color: ${textMuted};
    background: ${surfaceRaised};
    border: 1px solid ${border};
    border-radius: ${radiusS};
    padding: 2px 6px;
    font-family: ${fontFamily};
    font-size: ${captionSize};
    font-weight: ${weightRegular};
    line-height: ${lineCaption};
}
InputWindow QLabel#FeedbackLabel {
    color: ${success};
    background: ${successSoft};
    border: 1px solid ${success};
    border-radius: ${radiusS};
    padding: 2px 6px;
    font-family: ${fontFamily};
    font-size: ${captionSize};
    font-weight: ${weightControl};
    line-height: ${lineCaption};
    min-height: 18px;
}
InputWindow QLabel#FeedbackLabel[feedbackKind="warning"] {
    color: ${warning};
    background: ${warningSoft};
    border-color: ${warning};
}
)", p, fontSize, density);
}

QString AppTheme::dashboardStyleSheet(ThemeMode mode, FontSizeMode fontSize, DensityMode density)
{
    const Palette p = palette(mode);
    return themed(R"(
DashboardWindow {
    color: ${textPrimary};
    background: ${appBgGradient};
    font-family: ${fontFamily};
    font-size: ${bodySize};
    font-weight: ${weightRegular};
}
DashboardWindow QLabel {
    color: ${textPrimary};
    background: transparent;
}
DashboardWindow QLabel:disabled {
    color: ${textDisabled};
}
DashboardWindow QFrame#RecordsPanel {
    background: ${surfaceGradient};
    border: 1px solid ${border};
    border-top-color: ${surfaceHighlight};
    border-left-color: ${surfaceHighlight};
    border-radius: ${radiusL};
}
DashboardWindow QFrame#SidePanel {
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusL};
}
DashboardWindow QFrame#StatusBar {
    background: transparent;
    border: none;
}
DashboardWindow QScrollArea#SideContentScrollArea {
    background: transparent;
    border: none;
}
DashboardWindow QWidget#SideContent {
    background: transparent;
    border: none;
}
DashboardWindow QFrame#SettingsGroup {
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#SettingsGroup:hover {
    background: ${surfaceRaised};
    border-color: ${border};
}
DashboardWindow QLabel#PageTitle {
    color: ${textPrimary};
    font-size: ${pageTitleSize};
    font-weight: ${weightTitle};
    line-height: ${lineTitle};
}
DashboardWindow QLabel#SideSectionTitle,
DashboardWindow QLabel#RecordColumnTitle {
    color: ${textPrimary};
    font-size: ${sectionSize};
    font-weight: ${weightTitle};
    line-height: ${lineTitle};
}
DashboardWindow QFrame#RecordColumn[memoKind="question"] QLabel#RecordColumnTitle {
    color: ${primary};
}
DashboardWindow QFrame#RecordColumn[memoKind="todo"] QLabel#RecordColumnTitle {
    color: ${secondary};
}
DashboardWindow QLabel#FieldLabel,
DashboardWindow QLabel#StatusLabel {
    color: ${textSecondary};
    font-size: ${captionSize};
    font-weight: ${weightRegular};
    line-height: ${lineCaption};
}
DashboardWindow QLabel#SettingGroupTitle {
    color: ${textPrimary};
    font-size: ${bodySize};
    font-weight: ${weightControl};
    line-height: ${lineBody};
}
DashboardWindow QLabel#MemoControlTitle {
    color: ${textPrimary};
    font-size: ${bodySize};
    font-weight: ${weightTitle};
    line-height: ${lineBody};
}
DashboardWindow QLabel#CountBadge {
    color: ${primary};
    background: ${primarySoft};
    border: 1px solid ${primary};
    border-radius: ${radiusM};
    padding: 0 ${spaceS};
    min-width: ${spaceL};
    font-size: ${captionSize};
    font-weight: ${weightTitle};
    line-height: ${lineCaption};
}
DashboardWindow QLabel#CountBadge[memoKind="todo"] {
    color: ${secondary};
    background: ${secondarySoft};
    border-color: ${secondary};
}
DashboardWindow QFrame#RecordColumn {
    background: ${surfaceSunken};
    border: 1px solid ${border};
    border-top: 2px solid ${primary};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#RecordColumn[memoKind="todo"] {
    border-top-color: ${secondary};
}
DashboardWindow QFrame#RecordColumn:hover {
    background: ${surfaceSunken};
    border-color: ${border};
    border-top-color: ${primary};
}
DashboardWindow QFrame#RecordColumn[memoKind="todo"]:hover {
    border-top-color: ${secondary};
}
DashboardWindow QScrollArea#RecordsScrollArea {
    background: transparent;
    border: none;
}
DashboardWindow QWidget#RecordsList {
    background: transparent;
    border: none;
}
DashboardWindow QFrame#DashboardRecordCard {
    background: ${surface};
    border: 1px solid ${border};
    border-top-color: ${surfaceHighlight};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"] {
    border-color: ${border};
}
DashboardWindow QFrame#DashboardRecordCard:hover {
    background: ${surfaceRaised};
    border-color: ${borderStrong};
}
DashboardWindow QFrame#DashboardRecordCard[pressed="true"] {
    background: ${overlayPressed};
    border-color: ${primary};
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"]:hover {
    border-color: ${borderStrong};
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"][pressed="true"] {
    border-color: ${secondary};
}
DashboardWindow QFrame#DashboardRecordCard:hover QLabel#DashboardRecordTime {
    color: ${textSecondary};
}
DashboardWindow QFrame#DashboardRecordCard:hover QFrame#RecordAccentDot {
    background: ${primaryHover};
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"]:hover QFrame#RecordAccentDot {
    background: ${secondaryHover};
}
DashboardWindow QFrame#RecordAccentDot {
    background: ${primary};
    border: none;
    border-radius: 3px;
}
DashboardWindow QFrame#RecordAccentDot[memoKind="todo"] {
    background: ${secondary};
}
DashboardWindow QLabel#DashboardRecordText {
    color: ${textPrimary};
    background: transparent;
    font-size: ${bodyLargeSize};
    font-weight: ${weightMedium};
    line-height: ${lineBodyLarge};
}
DashboardWindow QLabel#DashboardRecordTime {
    color: ${textMuted};
    background: transparent;
    font-size: ${captionSize};
    font-weight: ${weightRegular};
    line-height: ${lineCaption};
}
DashboardWindow QFrame#MemoControlCard {
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] {
    border-color: ${border};
}
DashboardWindow QFrame#MemoControlCard:hover {
    background: ${surface};
    border-color: ${border};
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"]:hover {
    border-color: ${border};
}
DashboardWindow QPushButton {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: ${spaceS} ${spaceM};
    min-height: ${compactControlHeight};
    font-size: ${bodySize};
    font-weight: ${weightControl};
    line-height: ${lineBody};
}
DashboardWindow QPushButton:hover {
    color: ${textPrimary};
    background: ${overlayHover};
    border-color: ${primary};
}
DashboardWindow QPushButton:focus {
    color: ${textPrimary};
    background: ${overlayHover};
    border-color: ${primary};
}
DashboardWindow QPushButton:pressed {
    background: ${overlayPressed};
}
DashboardWindow QPushButton:disabled {
    color: ${textDisabled};
    background: ${surfaceSunken};
    border-color: ${border};
}
DashboardWindow QPushButton#PrimaryButton {
    color: ${primaryText};
    background: ${primary};
    border-color: ${primary};
    min-height: 32px;
    padding: 0 ${spaceM};
}
DashboardWindow QPushButton#PrimaryButton:hover,
DashboardWindow QPushButton#PrimaryButton:pressed {
    color: ${primaryText};
    background: ${primaryHover};
    border-color: ${primaryHover};
}
DashboardWindow QPushButton#PrimaryButton:focus {
    color: ${primaryText};
    background: ${primaryHover};
    border-color: ${primaryHover};
}
DashboardWindow QPushButton#SecondaryButton {
    color: ${textSecondary};
    background: transparent;
    border-color: ${border};
    min-width: 52px;
    min-height: ${compactControlHeight};
    padding: ${spaceXs} ${spaceS};
}
DashboardWindow QPushButton#SecondaryButton[active="true"] {
    color: ${primary};
    background: ${primarySoft};
    border-color: ${primary};
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] QPushButton#SecondaryButton[active="true"] {
    color: ${secondary};
    background: ${secondarySoft};
    border-color: ${secondary};
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] QPushButton:focus {
    border-color: ${secondary};
}
DashboardWindow QLineEdit,
DashboardWindow QKeySequenceEdit {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: 0 12px;
    min-height: 34px;
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
    selection-color: ${primaryText};
    selection-background-color: ${primary};
}
DashboardWindow QLineEdit:focus,
DashboardWindow QKeySequenceEdit:focus {
    border-color: ${primary};
}
DashboardWindow QLineEdit:disabled,
DashboardWindow QKeySequenceEdit:disabled {
    color: ${textDisabled};
    background: ${surfaceSunken};
    border-color: ${border};
}
DashboardWindow QKeySequenceEdit QLineEdit {
    color: ${textPrimary};
    background: transparent;
    border: none;
    border-radius: 0;
    padding: 0;
    min-height: 0;
}
DashboardWindow QKeySequenceEdit QLineEdit:focus {
    background: transparent;
    border: none;
}
DashboardWindow QCheckBox {
    color: ${textPrimary};
    background: transparent;
    border: none;
    padding: 0;
    spacing: 8px;
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
}
DashboardWindow QCheckBox#SystemToggle,
DashboardWindow QCheckBox#InputToggle {
    color: ${textSecondary};
    background: transparent;
    border: none;
    padding: 0;
}
DashboardWindow QCheckBox#PinToggle {
    color: ${textSecondary};
    background: transparent;
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: ${spaceXs} ${spaceS};
    font-size: ${captionSize};
}
DashboardWindow QCheckBox#PinToggle:checked {
    color: ${primary};
    background: ${primarySoft};
    border-color: ${primary};
}
DashboardWindow QCheckBox#PinToggle[memoKind="todo"]:checked {
    color: ${secondary};
    background: ${secondarySoft};
    border-color: ${secondary};
}
DashboardWindow QCheckBox:disabled {
    color: ${textDisabled};
}
DashboardWindow QCheckBox::indicator {
    width: 18px;
    height: 18px;
    border: 1px solid ${borderStrong};
    border-radius: ${radiusM};
    background: ${surface};
}
DashboardWindow QCheckBox#PinToggle::indicator {
    width: 14px;
    height: 14px;
    border-radius: 7px;
}
DashboardWindow QCheckBox::indicator:hover {
    border-color: ${primary};
    background: ${primarySoft};
}
DashboardWindow QCheckBox:focus::indicator {
    border-color: ${primary};
    background: ${primarySoft};
}
DashboardWindow QCheckBox::indicator:checked {
    background: ${primary};
    border-color: ${primary};
}
DashboardWindow QCheckBox::indicator:checked:hover {
    background: ${primaryHover};
    border-color: ${primaryHover};
}
DashboardWindow QCheckBox[memoKind="todo"]::indicator:hover {
    border-color: ${secondary};
    background: ${secondarySoft};
}
DashboardWindow QCheckBox[memoKind="todo"]:focus::indicator {
    border-color: ${secondary};
    background: ${secondarySoft};
}
DashboardWindow QCheckBox[memoKind="todo"]::indicator:checked {
    background: ${secondary};
    border-color: ${secondary};
}
DashboardWindow QCheckBox[memoKind="todo"]::indicator:checked:hover {
    background: ${secondaryHover};
    border-color: ${secondaryHover};
}
DashboardWindow QCheckBox::indicator:disabled {
    background: ${surfaceSunken};
    border-color: ${border};
}
DashboardWindow QComboBox {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: 0 ${spaceXl} 0 12px;
    min-height: 34px;
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
    selection-color: ${primaryText};
    selection-background-color: ${primary};
}
DashboardWindow QComboBox:hover {
    border-color: ${borderStrong};
    background: ${surfaceRaised};
}
DashboardWindow QComboBox:focus {
    border-color: ${primary};
    background: ${surfaceRaised};
}
DashboardWindow QComboBox:disabled {
    color: ${textDisabled};
    background: ${surfaceSunken};
    border-color: ${border};
}
DashboardWindow QComboBox::drop-down {
    background: ${surfaceRaised};
    border: none;
    border-left: 1px solid ${border};
    border-top-right-radius: ${radiusM};
    border-bottom-right-radius: ${radiusM};
    width: ${controlHeight};
}
DashboardWindow QComboBox::drop-down:hover {
    background: ${overlayHover};
}
DashboardWindow QComboBox::down-arrow {
    width: 0;
    height: 0;
    border-left: 4px solid transparent;
    border-right: 4px solid transparent;
    border-top: 5px solid ${textSecondary};
}
DashboardWindow QComboBox::down-arrow:disabled {
    border-top-color: ${textDisabled};
}
DashboardWindow QComboBox QAbstractItemView {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: ${spaceXs};
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    selection-color: ${primaryText};
    selection-background-color: ${primary};
    outline: none;
}
)", p, fontSize, density);
}
