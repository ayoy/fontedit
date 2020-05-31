#ifndef COMMON_H
#define COMMON_H

#include <QRgb>

namespace Color {

static constexpr QRgb activeGlyph = 0xff000000;
static constexpr QRgb inactiveGlyph = 0xffe2e2e2;
static constexpr QRgb glyphMargin = 0xffe2e2e2;

static constexpr QRgb inactiveText = 0xffc8c8c8;

}

#if defined(Q_OS_MAC)
    static const QString consoleFontName = "Monaco";
#elif defined(Q_OS_WIN)
    static const QString consoleFontName = "Consolas";
#else
    static const QString consoleFontName = "Monaco";
#endif


#endif // COMMON_H
