#ifndef APPTHEME_H
#define APPTHEME_H

#include "memostore.h"

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
    static QString applicationStyleSheet(ThemeMode mode);
    static QString memoWindowStyleSheet(ThemeMode mode);
    static QString inputWindowStyleSheet(ThemeMode mode);
    static QString dashboardStyleSheet(ThemeMode mode);
    static void applyElevation(QWidget *widget, ThemeMode mode, ElevationLevel level);
};

#endif // APPTHEME_H
