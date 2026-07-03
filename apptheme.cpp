#include "apptheme.h"

namespace {
struct Palette {
    QString background;
    QString panel;
    QString panelAlt;
    QString text;
    QString strongText;
    QString mutedText;
    QString disabledText;
    QString border;
    QString accent;
    QString accentSoft;
    QString accentText;
    QString danger;
    QString dangerSoft;
    QString hover;
    QString pressed;
    QString tableAlt;
    QString inputBackground;
    QString noteCard;
    QString noteCardHover;
    QString noteBorder;
    QString questionAccent;
    QString questionSoft;
    QString todoAccent;
    QString todoSoft;
};

Palette palette(ThemeMode mode)
{
    if (mode == ThemeMode::Dark) {
        return {
            "#0d1117", // background
            "#161b22", // panel
            "#21262d", // panelAlt
            "#c9d1d9", // text
            "#f0f6fc", // strongText
            "#8b949e", // mutedText
            "#6e7681", // disabledText
            "#30363d", // border
            "#58a6ff", // accent
            "#1f6feb", // accentSoft
            "#0d1117", // accentText
            "#f85149", // danger
            "#3d2226", // dangerSoft
            "#21262d", // hover
            "#30363d", // pressed
            "#10161f", // tableAlt
            "#0d1117", // inputBackground
            "#161b22", // noteCard
            "#21262d", // noteCardHover
            "#30363d", // noteBorder
            "#58a6ff", // questionAccent
            "#102a43", // questionSoft
            "#3fb950", // todoAccent
            "#123321"  // todoSoft
        };
    }

    return {
        "#f6f8fa", // background
        "#ffffff", // panel
        "#f6f8fa", // panelAlt
        "#24292f", // text
        "#1f2328", // strongText
        "#57606a", // mutedText
        "#8c959f", // disabledText
        "#d0d7de", // border
        "#0969da", // accent
        "#ddf4ff", // accentSoft
        "#ffffff", // accentText
        "#cf222e", // danger
        "#ffebe9", // dangerSoft
        "#f3f4f6", // hover
        "#eaeef2", // pressed
        "#f6f8fa", // tableAlt
        "#ffffff", // inputBackground
        "#fff8c5", // noteCard
        "#fff1a6", // noteCardHover
        "#eadc86", // noteBorder
        "#0969da", // questionAccent
        "#ddf4ff", // questionSoft
        "#1a7f37", // todoAccent
        "#dafbe1"  // todoSoft
    };
}

QString scrollBars(const Palette &p)
{
    return QString(R"(
QScrollBar:vertical {
    background: transparent;
    width: 10px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: %1;
    min-height: 28px;
    border-radius: 5px;
}
QScrollBar::handle:vertical:hover {
    background: %2;
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
    background: %1;
    min-width: 28px;
    border-radius: 5px;
}
QScrollBar::handle:horizontal:hover {
    background: %2;
}
QScrollBar::add-line:horizontal,
QScrollBar::sub-line:horizontal {
    width: 0;
}
)").arg(p.border, p.mutedText);
}
}

QString AppTheme::applicationStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return QString(R"(
QToolTip {
    color: %1;
    background: %2;
    border: 1px solid %3;
    border-radius: 6px;
    padding: 6px 8px;
}
QMenu {
    color: %1;
    background: %2;
    border: 1px solid %3;
    padding: 4px;
}
QMenu::item {
    padding: 6px 22px;
    border-radius: 4px;
}
QMenu::item:selected {
    color: %4;
    background: %5;
}
QMenu::item:disabled {
    color: %6;
}
QMenu::separator {
    height: 1px;
    background: %3;
    margin: 4px 8px;
}
)")
        .arg(p.text, p.panel, p.border, p.strongText, p.hover, p.disabledText)
        + scrollBars(p);
}

QString AppTheme::memoWindowStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return QString(R"(
MemoWindow {
    background: %1;
    border: 1px solid %2;
    border-radius: 12px;
}
MemoWindow[memoKind="question"] {
    border-color: %15;
}
MemoWindow[memoKind="todo"] {
    border-color: %17;
}
MemoWindow QWidget#TitleBar {
    background: transparent;
    border-bottom: 1px solid %2;
    min-height: 34px;
}
MemoWindow QLabel#TitleLabel {
    color: %3;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 15px;
    font-weight: 700;
}
MemoWindow[memoKind="question"] QLabel#TitleLabel {
    color: %15;
}
MemoWindow[memoKind="todo"] QLabel#TitleLabel {
    color: %17;
}
MemoWindow QPushButton {
    color: %4;
    background: %5;
    border: 1px solid %2;
    border-radius: 6px;
    padding: 4px 10px;
    min-height: 22px;
    min-width: 36px;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
}
MemoWindow QPushButton#TopButton {
    min-width: 56px;
}
MemoWindow QPushButton#TopButton[active="true"] {
    color: %3;
    background: %16;
    border-color: %15;
}
MemoWindow[memoKind="todo"] QPushButton#TopButton[active="true"] {
    background: %18;
    border-color: %17;
}
MemoWindow QPushButton#HideButton {
    color: %12;
    max-width: 28px;
    min-width: 28px;
    padding: 0;
    font-size: 15px;
}
MemoWindow QPushButton:hover {
    color: %3;
    background: %6;
    border-color: %7;
}
MemoWindow QPushButton#TopButton[active="true"]:hover {
    color: %3;
    background: %16;
    border-color: %15;
}
MemoWindow[memoKind="todo"] QPushButton#TopButton[active="true"]:hover {
    background: %18;
    border-color: %17;
}
MemoWindow QPushButton#HideButton:hover {
    color: %13;
    background: %14;
    border-color: %13;
}
MemoWindow QPushButton:pressed {
    background: %8;
}
MemoWindow QScrollArea,
MemoWindow QScrollArea QWidget {
    background: transparent;
    border: none;
}
MemoWindow QFrame#MemoRecordCard {
    background: %9;
    border: 1px solid %10;
    border-left: 3px solid %15;
    border-radius: 8px;
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"] {
    border-left-color: %17;
}
MemoWindow QFrame#MemoRecordCard:hover {
    background: %11;
    border-color: %7;
}
MemoWindow QFrame#MemoRecordCard[memoKind="todo"]:hover {
    border-left-color: %17;
}
MemoWindow QLabel#RecordText {
    color: %4;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 13px;
}
MemoWindow QLabel#RecordTime {
    color: %12;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 10px;
}
MemoWindow QFrame#EmptyState {
    background: %16;
    border: 1px dashed %2;
    border-radius: 8px;
}
MemoWindow QFrame#EmptyState[memoKind="todo"] {
    background: %18;
}
MemoWindow QLabel#EmptyStateText {
    color: %12;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 13px;
}
)")
        .arg(p.background, p.border, p.strongText, p.text, p.panel, p.hover,
             p.accent, p.pressed, p.noteCard, p.noteBorder, p.noteCardHover, p.mutedText,
             p.danger, p.dangerSoft, p.questionAccent, p.questionSoft, p.todoAccent, p.todoSoft);
}

QString AppTheme::inputWindowStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return QString(R"(
InputWindow {
    background: transparent;
}
InputWindow QFrame#InputPanel {
    background: %1;
    border: 1px solid %2;
    border-radius: 14px;
}
InputWindow QLineEdit {
    color: %3;
    background: %4;
    border: 1px solid %2;
    border-radius: 8px;
    padding: 8px 10px;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 13px;
    selection-background-color: %5;
    min-height: 24px;
}
InputWindow QLineEdit:focus {
    border-color: %6;
}
InputWindow QLineEdit::placeholder {
    color: %7;
}
InputWindow QPushButton {
    color: %6;
    background: %4;
    border: 1px solid %2;
    border-radius: 8px;
    padding: 7px 12px;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-weight: 700;
}
InputWindow QPushButton#TypeButton {
    min-width: 72px;
    color: %6;
    background: %11;
    border-color: %6;
}
InputWindow QPushButton#TypeButton[memoKind="todo"] {
    color: %13;
    background: %14;
    border-color: %13;
}
InputWindow QPushButton:hover {
    color: %8;
    background: %9;
    border-color: %6;
}
InputWindow QPushButton#TypeButton:hover {
    color: %8;
    background: %11;
    border-color: %6;
}
InputWindow QPushButton#TypeButton[memoKind="todo"]:hover {
    color: %8;
    background: %14;
    border-color: %13;
}
InputWindow QPushButton:pressed {
    background: %10;
}
)")
        .arg(p.panel, p.border, p.text, p.inputBackground, p.accentSoft,
             p.accent, p.mutedText, p.strongText, p.hover, p.pressed,
             p.questionSoft, p.questionAccent, p.todoAccent, p.todoSoft);
}

QString AppTheme::dashboardStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return QString(R"(
DashboardWindow {
    color: %1;
    background: %2;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 13px;
}
DashboardWindow QLabel {
    color: %1;
    background: transparent;
}
DashboardWindow QLabel:disabled {
    color: %3;
}
DashboardWindow QFrame#RecordsPanel,
DashboardWindow QFrame#SidePanel {
    background: %5;
    border: 1px solid %6;
    border-radius: 12px;
}
DashboardWindow QFrame#StatusBar {
    background: transparent;
    border: none;
}
DashboardWindow QLabel#PageTitle {
    color: %4;
    font-size: 21px;
    font-weight: 700;
}
DashboardWindow QLabel#SideSectionTitle,
DashboardWindow QLabel#RecordColumnTitle {
    color: %4;
    font-size: 14px;
    font-weight: 700;
}
DashboardWindow QLabel#FieldLabel,
DashboardWindow QLabel#StatusLabel {
    color: %17;
}
DashboardWindow QLabel#MemoControlTitle {
    color: %4;
    font-weight: 700;
}
DashboardWindow QLabel#CountBadge {
    color: %8;
    background: %12;
    border: 1px solid %8;
    border-radius: 9px;
    padding: 1px 8px;
    min-width: 18px;
    font-weight: 700;
}
DashboardWindow QLabel#CountBadge[memoKind="todo"] {
    color: %23;
    background: %24;
    border-color: %23;
}
DashboardWindow QFrame#RecordColumn {
    background: %10;
    border: 1px solid %6;
    border-top: 3px solid %21;
    border-radius: 10px;
}
DashboardWindow QFrame#RecordColumn[memoKind="todo"] {
    border-top-color: %23;
}
DashboardWindow QScrollArea#RecordsScrollArea,
DashboardWindow QWidget#RecordsList {
    background: transparent;
    border: none;
}
DashboardWindow QFrame#DashboardRecordCard {
    background: %14;
    border: 1px solid %15;
    border-left: 3px solid %21;
    border-radius: 8px;
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"] {
    border-left-color: %23;
}
DashboardWindow QFrame#DashboardRecordCard:hover {
    background: %16;
    border-color: %8;
}
DashboardWindow QFrame#DashboardRecordCard[memoKind="todo"]:hover {
    border-left-color: %23;
}
DashboardWindow QLabel#DashboardRecordText {
    color: %1;
    font-size: 13px;
    line-height: 18px;
}
DashboardWindow QLabel#DashboardRecordTime {
    color: %17;
    font-size: 11px;
}
DashboardWindow QFrame#EmptyState {
    background: %22;
    border: 1px dashed %6;
    border-radius: 8px;
}
DashboardWindow QFrame#EmptyState[memoKind="todo"] {
    background: %24;
}
DashboardWindow QLabel#EmptyStateText {
    color: %17;
}
DashboardWindow QFrame#MemoControlCard {
    background: %10;
    border: 1px solid %6;
    border-left: 3px solid %21;
    border-radius: 8px;
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] {
    border-left-color: %23;
}
DashboardWindow QPushButton {
    color: %1;
    background: %11;
    border: 1px solid %6;
    border-radius: 6px;
    padding: 6px 12px;
    min-height: 22px;
    font-weight: 600;
}
DashboardWindow QPushButton:hover {
    color: %4;
    background: %7;
    border-color: %8;
}
DashboardWindow QPushButton:pressed {
    background: %9;
}
DashboardWindow QPushButton:disabled {
    color: %3;
    background: %10;
    border-color: %6;
}
DashboardWindow QPushButton#PrimaryButton {
    color: %18;
    background: %8;
    border-color: %8;
}
DashboardWindow QPushButton#PrimaryButton:hover,
DashboardWindow QPushButton#PrimaryButton:pressed {
    color: %18;
    background: %8;
    border-color: %8;
}
DashboardWindow QPushButton#SecondaryButton {
    min-width: 58px;
}
DashboardWindow QPushButton#SecondaryButton[active="true"] {
    color: %18;
    background: %8;
    border-color: %8;
}
DashboardWindow QFrame#MemoControlCard[memoKind="todo"] QPushButton#SecondaryButton[active="true"] {
    background: %23;
    border-color: %23;
}
DashboardWindow QPushButton#DangerButton {
    color: %19;
    background: transparent;
    border-color: %19;
}
DashboardWindow QPushButton#DangerButton:hover {
    color: %19;
    background: %20;
    border-color: %19;
}
DashboardWindow QLineEdit,
DashboardWindow QKeySequenceEdit {
    color: %1;
    background: %11;
    border: 1px solid %6;
    border-radius: 6px;
    padding: 7px 9px;
    selection-background-color: %12;
}
DashboardWindow QLineEdit:focus,
DashboardWindow QKeySequenceEdit:focus {
    border-color: %8;
}
DashboardWindow QLineEdit:disabled,
DashboardWindow QKeySequenceEdit:disabled {
    color: %3;
    background: %10;
    border-color: %6;
}
DashboardWindow QCheckBox {
    color: %1;
    spacing: 8px;
}
DashboardWindow QCheckBox:disabled {
    color: %3;
}
DashboardWindow QCheckBox::indicator {
    width: 16px;
    height: 16px;
    border: 1px solid %6;
    border-radius: 4px;
    background: %11;
}
DashboardWindow QCheckBox::indicator:hover {
    border-color: %8;
}
DashboardWindow QCheckBox::indicator:checked {
    background: %8;
    border-color: %8;
}
DashboardWindow QCheckBox::indicator:disabled {
    background: %10;
    border-color: %6;
}
DashboardWindow QComboBox {
    color: %1;
    background: %11;
    border: 1px solid %6;
    border-radius: 6px;
    padding: 6px 30px 6px 10px;
}
DashboardWindow QComboBox:hover,
DashboardWindow QComboBox:focus {
    border-color: %8;
}
DashboardWindow QComboBox:disabled {
    color: %3;
    background: %10;
    border-color: %6;
}
DashboardWindow QComboBox::drop-down {
    border: none;
    width: 28px;
}
DashboardWindow QComboBox QAbstractItemView {
    color: %1;
    background: %5;
    border: 1px solid %6;
    selection-color: %4;
    selection-background-color: %12;
    outline: none;
}
)")
        .arg(p.text, p.background, p.disabledText, p.strongText, p.panel, p.border,
             p.hover, p.accent, p.pressed, p.panelAlt, p.inputBackground, p.accentSoft,
             p.tableAlt, p.noteCard, p.noteBorder, p.noteCardHover, p.mutedText,
             p.accentText, p.danger, p.dangerSoft, p.questionAccent, p.questionSoft,
             p.todoAccent, p.todoSoft);
}
