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
    QString hover;
    QString pressed;
    QString tableAlt;
    QString inputBackground;
    QString noteCard;
    QString noteCardHover;
    QString noteBorder;
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
            "#21262d", // hover
            "#30363d", // pressed
            "#10161f", // tableAlt
            "#0d1117", // inputBackground
            "#161b22", // noteCard
            "#21262d", // noteCardHover
            "#30363d"  // noteBorder
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
        "#f3f4f6", // hover
        "#eaeef2", // pressed
        "#f6f8fa", // tableAlt
        "#ffffff", // inputBackground
        "#fff8c5", // noteCard
        "#fff1a6", // noteCardHover
        "#eadc86"  // noteBorder
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
    padding: 6px;
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
)")
        .arg(p.text, p.panel, p.border, p.strongText, p.hover)
        + scrollBars(p);
}

QString AppTheme::memoWindowStyleSheet(ThemeMode mode)
{
    const Palette p = palette(mode);
    return QString(R"(
MemoWindow {
    background: %1;
    border: 1px solid %2;
    border-radius: 10px;
}
MemoWindow QWidget#TitleBar {
    background: transparent;
}
MemoWindow QLabel#TitleLabel {
    color: %3;
    font-family: "Segoe UI", "Microsoft YaHei UI", sans-serif;
    font-size: 15px;
    font-weight: 700;
}
MemoWindow QPushButton {
    color: %4;
    background: %5;
    border: 1px solid %2;
    border-radius: 6px;
    padding: 4px 9px;
    min-width: 34px;
}
MemoWindow QPushButton:hover {
    color: %3;
    background: %6;
    border-color: %7;
}
MemoWindow QPushButton:pressed {
    background: %8;
}
MemoWindow QScrollArea,
MemoWindow QScrollArea QWidget {
    background: transparent;
    border: none;
}
MemoWindow QFrame[record="true"] {
    background: %9;
    border: 1px solid %10;
    border-radius: 8px;
}
MemoWindow QFrame[record="true"]:hover {
    background: %11;
    border-color: %7;
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
)")
        .arg(p.background, p.border, p.strongText, p.text, p.panel, p.hover,
             p.accent, p.pressed, p.noteCard, p.noteBorder, p.noteCardHover, p.mutedText);
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
    border-radius: 12px;
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
InputWindow QPushButton:hover {
    color: %8;
    background: %9;
    border-color: %6;
}
InputWindow QPushButton:pressed {
    background: %10;
}
)")
        .arg(p.panel, p.border, p.text, p.inputBackground, p.accentSoft,
             p.accent, p.mutedText, p.strongText, p.hover, p.pressed);
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
DashboardWindow QGroupBox {
    color: %4;
    background: %5;
    border: 1px solid %6;
    border-radius: 8px;
    margin-top: 16px;
    padding: 14px 10px 10px 10px;
    font-weight: 700;
}
DashboardWindow QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    left: 12px;
    padding: 0 6px;
    color: %4;
    background: %2;
}
DashboardWindow QPushButton {
    color: %1;
    background: %5;
    border: 1px solid %6;
    border-radius: 6px;
    padding: 6px 12px;
    min-height: 22px;
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
DashboardWindow QTableWidget {
    color: %1;
    background: %11;
    alternate-background-color: %13;
    border: 1px solid %6;
    border-radius: 6px;
    gridline-color: %6;
    selection-color: %4;
    selection-background-color: %12;
}
DashboardWindow QTableWidget::item {
    color: %1;
    padding: 4px;
}
DashboardWindow QTableWidget::item:selected {
    color: %4;
    background: %12;
}
DashboardWindow QHeaderView::section {
    color: %4;
    background: %10;
    border: none;
    border-right: 1px solid %6;
    border-bottom: 1px solid %6;
    padding: 7px;
    font-weight: 700;
}
DashboardWindow QTableCornerButton::section {
    background: %10;
    border: none;
    border-right: 1px solid %6;
    border-bottom: 1px solid %6;
}
)")
        .arg(p.text, p.background, p.disabledText, p.strongText, p.panel, p.border,
             p.hover, p.accent, p.pressed, p.panelAlt, p.inputBackground, p.accentSoft,
             p.tableAlt);
}
