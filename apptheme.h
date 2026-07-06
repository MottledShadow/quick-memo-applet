#ifndef APPTHEME_H
#define APPTHEME_H

#include "memostore.h"

#include <QColor>
#include <QString>

class QWidget;

enum class ElevationLevel {
    E0,
    E1,
    E2,
    E3
};

class AppTheme
{
public:
    static ThemeMode effectiveThemeMode(ThemeMode mode);
    static QString applicationStyleSheet(ThemeMode mode,
                                         FontSizeMode fontSize,
                                         DensityMode density);
    static QString memoWindowStyleSheet(ThemeMode mode,
                                        FontSizeMode fontSize,
                                        DensityMode density);
    static QString inputWindowStyleSheet(ThemeMode mode,
                                         FontSizeMode fontSize,
                                         DensityMode density);
    static QString dashboardStyleSheet(ThemeMode mode,
                                       FontSizeMode fontSize,
                                       DensityMode density);
    static void applyElevation(QWidget *widget, ThemeMode mode, ElevationLevel level);
    static QColor memoAccentColor(MemoType type, ThemeMode mode);
    static QColor memoPaperLineColor(ThemeMode mode);
};

#endif // APPTHEME_H
