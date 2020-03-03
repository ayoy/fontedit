#ifndef COMMON_H
#define COMMON_H

#include <QRgb>

namespace Color {

static constexpr QRgb glyphMargin = 0xe2e2e2;

}

#if defined(Q_OS_MAC)
    static const QString consoleFontName = "Monaco";
#elif defined(Q_OS_WIN)
    static const QString consoleFontName = "Consolas";
#else
    static const QString consoleFontName = "Monaco";
#endif


#endif // COMMON_H
