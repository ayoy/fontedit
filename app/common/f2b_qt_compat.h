#ifndef F2B_QT_COMPAT_H
#define F2B_QT_COMPAT_H

#include "f2b.h"
#include <QPoint>

namespace Font {

inline Font::Point point_with_qpoint(const QPoint &p)
{
    return { static_cast<std::size_t>(qMax(0, p.x())),
                static_cast<std::size_t>(qMax(0, p.y())) };
}

}

#endif // F2B_QT_COMPAT_H
