#ifndef F2B_QT_COMPAT_H
#define F2B_QT_COMPAT_H

#include "f2b.h"
#include <QPoint>
#include <QSize>
#include <QBitmap>
#include <QPainter>
#include <QDebug>

namespace Font {

inline Font::Point point_with_qpoint(const QPoint &p)
{
    return { static_cast<std::size_t>(qMax(0, p.x())),
                static_cast<std::size_t>(qMax(0, p.y())) };
}

inline QPoint qpoint_with_point(const Font::Point &p)
{
    return QPoint { static_cast<int>(p.x), static_cast<int>(p.y) };
}

inline QSize qsize_with_size(const Font::Size &s)
{
    return QSize { static_cast<int>(s.width), static_cast<int>(s.height) };
}

inline QBitmap glyph_bitmap_preview(const Font::Glyph &g)
{
    auto image_size = qsize_with_size(g.size());

    QImage image(image_size, QImage::Format_Mono);
    image.fill(Qt::color1);

    for (std::vector<bool>::size_type y = 0; y < g.size().height; ++y) {
        for (std::vector<bool>::size_type x = 0; x < g.size().width; ++x) {
            if (g.is_pixel_set({x, y})) {
                image.setPixel(x, y, Qt::color0);
            }
        }
    }

    QBitmap b(image_size);
    QPainter p(&b);
    p.drawImage(QPoint(), image);
    p.end();

    return b;
}

}

#endif // F2B_QT_COMPAT_H
