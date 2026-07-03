#ifndef APPTHEME_H
#define APPTHEME_H

#include "memostore.h"

#include <QString>

class AppTheme
{
public:
    static QString applicationStyleSheet(ThemeMode mode);
    static QString memoWindowStyleSheet(ThemeMode mode);
    static QString inputWindowStyleSheet(ThemeMode mode);
    static QString dashboardStyleSheet(ThemeMode mode);
};

#endif // APPTHEME_H
