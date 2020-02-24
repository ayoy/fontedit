#ifndef F2B_QT_COMPAT_H
#define F2B_QT_COMPAT_H

#include "f2b.h"
#include <QPoint>
#include <QSize>
#include <QBitmap>
#include <QPainter>
#include <QDebug>
#include "common.h"

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

inline QPixmap glyph_preview_pixmap(const Font::Glyph &g, Font::Margins m)
{
    auto glyph_size = g.size();
    glyph_size.height -= m.top + m.bottom;

    auto image_size = qsize_with_size(g.size());
    auto useful_image_size = qsize_with_size(glyph_size);

    QImage image(useful_image_size, QImage::Format_Grayscale8);
    image.fill(Qt::white);

    for (std::vector<bool>::size_type y = 0; y < glyph_size.height; ++y) {
        for (std::vector<bool>::size_type x = 0; x < glyph_size.width; ++x) {
            if (g.is_pixel_set({x, y + m.top})) {
                image.setPixel(x, y, Qt::black);
            }
        }
    }

    QPixmap b(image_size);
    QPainter p(&b);
    p.fillRect(b.rect(), Color::glyphMargin);
    p.drawImage(QPoint(0, m.top), image);
    p.end();

    return b;
}

}

#endif // F2B_QT_COMPAT_H
