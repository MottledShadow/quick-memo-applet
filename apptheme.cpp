#include "apptheme.h"

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
    QString surfaceSunken;
    QString memoPanel;
    QString memoPanelTop;
    QString memoTitleGradient;
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
            "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #151F31, stop:1 #111827)", // surfaceGradient
            "#1F2937", // surfaceRaised
            "#0F172A", // surfaceSunken
            "#252113", // memoPanel
            "#3A3217", // memoPanelTop
            "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #3A3217, stop:1 #252113)", // memoTitleGradient
            "#302A18", // memoCard
            "#3A321C", // memoCardHover
            "#5B4E2A", // memoBorder
            "#8A7334", // memoBorderStrong
            "#FFF7D6", // memoText
            "#D6C99A", // memoTimeText

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
        "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFFFFF, stop:1 #F8FAFC)", // surfaceGradient
        "#F8FAFC", // surfaceRaised
        "#EEF2F7", // surfaceSunken
        "#FFF1A8", // memoPanel
        "#FFE680", // memoPanelTop
        "qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FFF2A3, stop:1 #FFE680)", // memoTitleGradient
        "#FFF9D8", // memoCard
        "#FFF4BD", // memoCardHover
        "#E8D486", // memoBorder
        "#C9A938", // memoBorderStrong
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

QString themed(QString style, const Palette &p)
{
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
        .replace("${surfaceSunken}", p.surfaceSunken)
        .replace("${memoPanel}", p.memoPanel)
        .replace("${memoPanelTop}", p.memoPanelTop)
        .replace("${memoTitleGradient}", p.memoTitleGradient)
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
        .replace("${infoSoft}", p.infoSoft);
}

QString scrollBars(const Palette &p)
{
    return themed(R"(
QScrollBar:vertical {
    background: transparent;
    width: 10px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: ${borderStrong};
    min-height: 28px;
    border-radius: 5px;
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
    height: 10px;
    margin: 0;
}
QScrollBar::handle:horizontal {
    background: ${borderStrong};
    min-width: 28px;
    border-radius: 5px;
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
}

QString AppTheme::applicationStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return themed(R"(
QToolTip {
    color: ${textPrimary};
    background: ${surfaceRaised};
    border: 1px solid ${borderStrong};
    border-radius: 6px;
    padding: 6px 8px;
}
QMenu {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    padding: 4px;
}
QMenu::item {
    padding: 6px 22px;
    border-radius: 4px;
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
    margin: 4px 8px;
}
)", p) + scrollBars(p);
}

QString AppTheme::memoWindowStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return themed(R"(
MemoWindow {
    color: ${memoText};
    background: ${memoPanel};
    border: 1px solid ${memoBorderStrong};
    border-radius: 10px;
}
MemoWindow[memoKind="question"] {
    border-color: ${memoBorderStrong};
}
MemoWindow[memoKind="todo"] {
    border-color: ${memoBorderStrong};
}
MemoWindow QWidget#TitleBar {
    background: ${memoTitleGradient};
    border-bottom: 1px solid ${memoBorderStrong};
    min-height: 30px;
}
MemoWindow QLabel#TitleLabel {
    color: ${memoText};
    background: transparent;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 16px;
    font-weight: 700;
}
MemoWindow[memoKind="question"] QLabel#TitleLabel {
    color: ${primaryHover};
}
MemoWindow[memoKind="todo"] QLabel#TitleLabel {
    color: ${secondaryHover};
}
MemoWindow QPushButton {
    color: ${memoText};
    background: ${memoCard};
    border: 1px solid ${memoBorder};
    border-radius: 7px;
    padding: 2px 8px;
    min-height: 22px;
    min-width: 30px;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
}
MemoWindow QPushButton#TopButton {
    min-width: 48px;
}
MemoWindow QPushButton#TopButton[active="true"] {
    color: ${primary};
    background: ${memoCard};
    border-color: ${primary};
}
MemoWindow[memoKind="todo"] QPushButton#TopButton[active="true"] {
    color: ${secondary};
    background: ${memoCard};
    border-color: ${secondary};
}
MemoWindow QPushButton#HideButton {
    color: ${memoTimeText};
    max-width: 26px;
    min-width: 26px;
    padding: 0;
    font-size: 14px;
}
MemoWindow QPushButton:hover {
    color: ${memoText};
    background: ${memoCardHover};
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
MemoWindow QScrollArea {
    background: ${memoPanel};
    border: none;
}
MemoWindow QScrollArea QWidget {
    background: ${memoPanel};
    border: none;
}
MemoWindow QFrame#MemoRecordCard {
    background: ${memoCard};
    border: 1px solid ${memoBorder};
    border-left: 2px solid ${primary};
    border-radius: 6px;
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"] {
    border-left-color: ${secondary};
}
MemoWindow QFrame#MemoRecordCard:hover {
    background: ${memoCardHover};
    border-color: ${memoBorderStrong};
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"]:hover {
    border-left-color: ${secondary};
}
MemoWindow QLabel#RecordText {
    color: ${memoText};
    background: transparent;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 14px;
    font-weight: 500;
}
MemoWindow QLabel#RecordTime {
    color: ${memoTimeText};
    background: transparent;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 11px;
}
MemoWindow QFrame#EmptyState {
    background: ${memoCard};
    border: 1px dashed ${memoBorderStrong};
    border-radius: 6px;
}
MemoWindow QFrame#EmptyState[memoKind="todo"] {
    background: ${memoCard};
}
MemoWindow QLabel#EmptyStateText {
    color: ${memoTimeText};
    background: transparent;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 13px;
}
MemoWindow QWidget#ResizeBar {
    background: ${memoPanel};
    min-height: 8px;
    max-height: 8px;
}
MemoWindow QSizeGrip#ResizeGrip {
    background: transparent;
    width: 12px;
    height: 12px;
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
    border-radius: 14px;
}
InputWindow QLineEdit {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: 8px;
    padding: 8px 10px;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 13px;
    selection-color: ${primaryText};
    selection-background-color: ${primary};
    min-height: 24px;
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
    border-radius: 8px;
    padding: 7px 12px;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-weight: 700;
}
InputWindow QPushButton#TypeButton {
    min-width: 72px;
    color: ${primary};
    background: ${primarySoft};
    border-color: ${primary};
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
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 13px;
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
    border-radius: 12px;
}
DashboardWindow QFrame#StatusBar {
    background: transparent;
    border: none;
}
DashboardWindow QLabel#PageTitle {
    color: ${textPrimary};
    font-size: 21px;
    font-weight: 700;
}
DashboardWindow QLabel#SideSectionTitle,
DashboardWindow QLabel#RecordColumnTitle {
    color: ${textPrimary};
    font-size: 14px;
    font-weight: 700;
}
DashboardWindow QLabel#FieldLabel,
DashboardWindow QLabel#StatusLabel {
    color: ${textSecondary};
}
DashboardWindow QLabel#MemoControlTitle {
    color: ${textPrimary};
    font-weight: 700;
}
DashboardWindow QLabel#CountBadge {
    color: ${primary};
    background: ${primarySoft};
    border: 1px solid ${primary};
    border-radius: 9px;
    padding: 1px 8px;
    min-width: 18px;
    font-weight: 700;
}
DashboardWindow QLabel#CountBadge[memoKind="todo"] {
    color: ${secondary};
    background: ${secondarySoft};
    border-color: ${secondary};
}
DashboardWindow QFrame#RecordColumn {
    background: ${surfaceSunken};
    border: 1px solid ${border};
    border-top: 3px solid ${primary};
    border-radius: 10px;
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
    border-radius: 8px;
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"] {
    border-left-color: ${secondary};
}
DashboardWindow QFrame#DashboardRecordCard:hover {
    background: ${surfaceRaised};
    border-color: ${primary};
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"]:hover {
    border-left-color: ${secondary};
}
DashboardWindow QLabel#DashboardRecordText {
    color: ${textPrimary};
    background: transparent;
    font-size: 13px;
    line-height: 18px;
}
DashboardWindow QLabel#DashboardRecordTime {
    color: ${textSecondary};
    background: transparent;
    font-size: 11px;
}
DashboardWindow QFrame#EmptyState {
    background: ${primarySoft};
    border: 1px dashed ${borderStrong};
    border-radius: 8px;
}
DashboardWindow QFrame#EmptyState[memoKind="todo"] {
    background: ${secondarySoft};
}
DashboardWindow QLabel#EmptyStateText {
    color: ${textSecondary};
    background: transparent;
}
DashboardWindow QFrame#MemoControlCard {
    background: ${surfaceSunken};
    border: 1px solid ${border};
    border-left: 3px solid ${primary};
    border-radius: 8px;
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] {
    border-left-color: ${secondary};
}
DashboardWindow QPushButton {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: 6px;
    padding: 6px 12px;
    min-height: 22px;
    font-weight: 600;
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
    min-width: 58px;
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
    border-radius: 6px;
    padding: 7px 9px;
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
}
DashboardWindow QCheckBox:disabled {
    color: ${textDisabled};
}
DashboardWindow QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid ${borderStrong};
    border-radius: 4px;
    background: ${surface};
}
DashboardWindow QCheckBox::indicator:hover {
    border-color: ${primary};
}
DashboardWindow QCheckBox::indicator:checked {
    background: ${primary};
    border-color: ${primary};
}
DashboardWindow QCheckBox::indicator:disabled {
    background: ${surfaceSunken};
    border-color: ${border};
}
DashboardWindow QComboBox {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    border-radius: 6px;
    padding: 6px 30px 6px 10px;
}
DashboardWindow QComboBox:hover,
DashboardWindow QComboBox:focus {
    border-color: ${primary};
}
DashboardWindow QComboBox:disabled {
    color: ${textDisabled};
    background: ${surfaceSunken};
    border-color: ${border};
}
DashboardWindow QComboBox::drop-down {
    border: none;
    width: 28px;
}
DashboardWindow QComboBox QAbstractItemView {
    color: ${textPrimary};
    background: ${surface};
    border: 1px solid ${border};
    selection-color: ${primaryText};
    selection-background-color: ${primary};
    outline: none;
}
)", p);
}
