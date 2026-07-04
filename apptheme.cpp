#include "apptheme.h"

#include <QColor>
#include <QGraphicsDropShadowEffect>
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

Palette palette(ThemeMode mode)
{
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

SpacingScale spacing()
{
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

TypeScale typography()
{
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

QString themed(QString style, const Palette &p)
{
    const TypeScale t = typography();
    const SpacingScale s = spacing();
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

QString scrollBars(const Palette &p)
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
QScrollBar::add-line:vertical,
QScrollBar::sub-line:vertical {
    height: 0;
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
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal {
    width: 0;
}
)", p);
}

ElevationSpec elevationSpec(ThemeMode mode, ElevationLevel level)
{
    if (level == ElevationLevel::E0) {
        return {0, 0, 0, QColor(0, 0, 0, 0)};
    }

    if (mode == ThemeMode::Dark) {
        switch (level) {
        case ElevationLevel::E1:
            return {16, 0, 4, QColor(8, 13, 24, 110)};
        case ElevationLevel::E2:
            return {26, 0, 10, QColor(8, 13, 24, 140)};
        case ElevationLevel::E3:
            return {34, 0, 14, QColor(2, 6, 23, 165)};
        case ElevationLevel::E0:
            break;
        }
    }

    switch (level) {
    case ElevationLevel::E1:
        return {14, 0, 4, QColor(71, 85, 105, 42)};
    case ElevationLevel::E2:
        return {24, 0, 8, QColor(71, 85, 105, 58)};
    case ElevationLevel::E3:
        return {32, 0, 12, QColor(51, 65, 85, 70)};
    case ElevationLevel::E0:
        break;
    }

    return {0, 0, 0, QColor(0, 0, 0, 0)};
}
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

QString AppTheme::applicationStyleSheet(ThemeMode mode)
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
)", p) + scrollBars(p);
}

QString AppTheme::memoWindowStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return themed(R"(
MemoWindow {
    color: ${memoText};
    background: transparent;
    border: none;
}
MemoWindow QFrame#MemoPanel {
    color: ${memoText};
    background: ${memoPanel};
    border: 1px solid ${memoBorderStrong};
    border-top-color: ${surfaceHighlight};
    border-left-color: ${surfaceHighlight};
    border-radius: ${radiusM};
}
MemoWindow QWidget#TitleBar {
    background: ${memoTitleGradient};
    border-bottom: 1px solid ${memoBorder};
    border-top-left-radius: ${radiusM};
    border-top-right-radius: ${radiusM};
    border-bottom-left-radius: 0;
    border-bottom-right-radius: 0;
    min-height: ${controlHeight};
}
MemoWindow QLabel#TitleLabel {
    color: ${memoText};
    background: transparent;
    font-family: ${fontFamily};
    font-size: ${memoTitleSize};
    font-weight: ${weightTitle};
    line-height: ${lineTitle};
}
MemoWindow[memoKind="question"] QLabel#TitleLabel {
    color: ${primary};
}
MemoWindow[memoKind="todo"] QLabel#TitleLabel {
    color: ${secondary};
}
MemoWindow QPushButton {
    color: ${memoText};
    background: ${memoPanelTop};
    border: 1px solid ${memoBorder};
    border-radius: ${radiusM};
    padding: ${spaceXs} ${spaceS};
    min-height: ${compactControlHeight};
    min-width: ${compactControlHeight};
    font-family: ${fontFamily};
    font-size: ${bodySize};
    font-weight: ${weightControl};
    line-height: ${lineBody};
}
MemoWindow QPushButton#TopButton {
    min-width: 56px;
}
MemoWindow QPushButton#TopButton[active="true"] {
    color: ${primary};
    background: ${memoPanelTop};
    border-color: ${primary};
}
MemoWindow[memoKind="todo"] QPushButton#TopButton[active="true"] {
    color: ${secondary};
    background: ${memoPanelTop};
    border-color: ${secondary};
}
MemoWindow QPushButton#HideButton {
    color: ${memoTimeText};
    max-width: ${compactControlHeight};
    min-width: ${compactControlHeight};
    padding: 0;
    font-size: ${bodySize};
}
MemoWindow QPushButton:hover {
    color: ${memoText};
    background: ${memoCard};
    border-color: ${memoBorderStrong};
}
MemoWindow QPushButton#TopButton[active="true"]:hover {
    color: ${primaryHover};
    background: ${memoCardHover};
    border-color: ${primaryHover};
}
MemoWindow[memoKind="todo"] QPushButton#TopButton[active="true"]:hover {
    color: ${secondaryHover};
    background: ${memoCardHover};
    border-color: ${secondaryHover};
}
MemoWindow QPushButton#HideButton:hover {
    color: ${danger};
    background: ${dangerSoft};
    border-color: ${danger};
}
MemoWindow QPushButton:pressed {
    background: ${overlayPressed};
}
MemoWindow QScrollArea#MemoScrollArea {
    background: ${memoList};
    border: none;
}
MemoWindow QWidget#MemoList {
    background: ${memoList};
    border: none;
}
MemoWindow QFrame#MemoRecordCard {
    background: ${memoCard};
    border: 1px solid ${memoBorder};
    border-left: 3px solid ${primary};
    border-top-color: ${surfaceHighlight};
    border-radius: ${radiusM};
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"] {
    border-left-color: ${secondary};
}
MemoWindow QFrame#MemoRecordCard:hover {
    background: ${memoCardHover};
    border-color: ${memoBorderStrong};
    border-left-color: ${primaryHover};
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"]:hover {
    border-left-color: ${secondaryHover};
}
MemoWindow QLabel#RecordText {
    color: ${memoText};
    background: transparent;
    font-family: ${fontFamily};
    font-size: ${bodyLargeSize};
    font-weight: ${weightMedium};
    line-height: ${lineBodyLarge};
}
MemoWindow QLabel#RecordTime {
    color: ${memoTimeText};
    background: transparent;
    font-family: ${fontFamily};
    font-size: ${captionSize};
    font-weight: ${weightRegular};
    line-height: ${lineCaption};
}
MemoWindow QFrame#EmptyState {
    background: ${memoCard};
    border: 1px dashed ${memoBorderStrong};
    border-radius: ${radiusM};
}
MemoWindow QFrame#EmptyState[memoKind="todo"] {
    background: ${memoCard};
}
MemoWindow QLabel#EmptyStateText {
    color: ${memoTimeText};
    background: transparent;
    font-family: ${fontFamily};
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
}
MemoWindow QSizeGrip#ResizeGrip {
    background: transparent;
    width: ${spaceM};
    height: ${spaceM};
}
)", p);
}

QString AppTheme::inputWindowStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return themed(R"(
InputWindow {
    background: transparent;
}
InputWindow QFrame#InputPanel {
    background: ${surfaceGradient};
    border: 1px solid ${borderStrong};
    border-top-color: ${surfaceHighlight};
    border-left-color: ${surfaceHighlight};
    border-radius: ${radiusL};
}
InputWindow QLineEdit {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: ${spaceS} 12px;
    font-family: ${fontFamily};
    font-size: ${bodyLargeSize};
    font-weight: ${weightRegular};
    line-height: ${lineBodyLarge};
    selection-color: ${primaryText};
    selection-background-color: ${primary};
    min-height: ${compactControlHeight};
}
InputWindow QLineEdit:focus {
    border-color: ${primary};
}
InputWindow QLineEdit::placeholder {
    color: ${textMuted};
}
InputWindow QPushButton {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: ${spaceS} ${spaceM};
    font-family: ${fontFamily};
    font-size: ${bodySize};
    font-weight: ${weightControl};
    line-height: ${lineBody};
}
InputWindow QPushButton#TypeButton {
    min-width: 72px;
    color: ${primary};
    background: ${primarySoft};
    border-color: ${primary};
    font-weight: ${weightTitle};
}
InputWindow QPushButton#TypeButton[memoKind="todo"] {
    color: ${secondary};
    background: ${secondarySoft};
    border-color: ${secondary};
}
InputWindow QPushButton:hover {
    color: ${textPrimary};
    background: ${overlayHover};
    border-color: ${primary};
}
InputWindow QPushButton#TypeButton:hover {
    color: ${primaryHover};
    background: ${primarySoft};
    border-color: ${primaryHover};
}
InputWindow QPushButton#TypeButton[memoKind="todo"]:hover {
    color: ${secondaryHover};
    background: ${secondarySoft};
    border-color: ${secondaryHover};
}
InputWindow QPushButton:pressed {
    background: ${overlayPressed};
}
)", p);
}

QString AppTheme::dashboardStyleSheet(ThemeMode mode)
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
DashboardWindow QFrame#RecordsPanel,
DashboardWindow QFrame#SidePanel {
    background: ${surfaceGradient};
    border: 1px solid ${border};
    border-top-color: ${surfaceHighlight};
    border-left-color: ${surfaceHighlight};
    border-radius: ${radiusL};
}
DashboardWindow QFrame#StatusBar {
    background: transparent;
    border: none;
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
DashboardWindow QLabel#FieldLabel,
DashboardWindow QLabel#StatusLabel {
    color: ${textSecondary};
    font-size: ${captionSize};
    font-weight: ${weightRegular};
    line-height: ${lineCaption};
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
    background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 ${surfaceSunken}, stop:1 ${appBg});
    border: 1px solid ${border};
    border-top: 3px solid ${primary};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#RecordColumn[memoKind="todo"] {
    border-top-color: ${secondary};
}
DashboardWindow QScrollArea#RecordsScrollArea {
    background: ${surfaceSunken};
    border: none;
}
DashboardWindow QWidget#RecordsList {
    background: ${surfaceSunken};
    border: none;
}
DashboardWindow QFrame#DashboardRecordCard {
    background: ${surface};
    border: 1px solid ${border};
    border-left: 3px solid ${primary};
    border-top-color: ${surfaceHighlight};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"] {
    border-left-color: ${secondary};
}
DashboardWindow QFrame#DashboardRecordCard:hover {
    background: ${surfaceRaised};
    border-color: ${borderStrong};
    border-left-color: ${primaryHover};
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"]:hover {
    border-left-color: ${secondaryHover};
}
DashboardWindow QLabel#DashboardRecordText {
    color: ${textPrimary};
    background: transparent;
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
}
DashboardWindow QLabel#DashboardRecordTime {
    color: ${textSecondary};
    background: transparent;
    font-size: ${captionSize};
    font-weight: ${weightRegular};
    line-height: ${lineCaption};
}
DashboardWindow QFrame#EmptyState {
    background: ${primarySoft};
    border: 1px dashed ${borderStrong};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#EmptyState[memoKind="todo"] {
    background: ${secondarySoft};
}
DashboardWindow QLabel#EmptyStateText {
    color: ${textSecondary};
    background: transparent;
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
}
DashboardWindow QFrame#MemoControlCard {
    background: ${surfaceSunken};
    border: 1px solid ${border};
    border-left: 3px solid ${primary};
    border-radius: ${radiusM};
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] {
    border-left-color: ${secondary};
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
}
DashboardWindow QPushButton#PrimaryButton:hover,
DashboardWindow QPushButton#PrimaryButton:pressed {
    color: ${primaryText};
    background: ${primaryHover};
    border-color: ${primaryHover};
}
DashboardWindow QPushButton#SecondaryButton {
    min-width: 64px;
}
DashboardWindow QPushButton#SecondaryButton[active="true"] {
    color: ${primaryText};
    background: ${primary};
    border-color: ${primary};
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] QPushButton#SecondaryButton[active="true"] {
    color: ${secondaryText};
    background: ${secondary};
    border-color: ${secondary};
}
DashboardWindow QPushButton#DangerButton {
    color: ${danger};
    background: ${surface};
    border-color: ${danger};
}
DashboardWindow QPushButton#DangerButton:hover {
    color: ${danger};
    background: ${dangerSoft};
    border-color: ${danger};
}
DashboardWindow QLineEdit,
DashboardWindow QKeySequenceEdit {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: ${radiusM};
    padding: ${spaceS} 12px;
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
DashboardWindow QCheckBox {
    color: ${textPrimary};
    spacing: 8px;
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
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
DashboardWindow QCheckBox::indicator:hover {
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
    padding: ${spaceS} ${spaceXl} ${spaceS} 12px;
    font-size: ${bodySize};
    font-weight: ${weightRegular};
    line-height: ${lineBody};
    selection-color: ${primaryText};
    selection-background-color: ${primary};
}
DashboardWindow QComboBox:hover,
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
)", p);
}
