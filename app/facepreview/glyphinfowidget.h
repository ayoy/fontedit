#ifndef GLYPHINFOWIDGET_H
#define GLYPHINFOWIDGET_H

#include <QGraphicsWidget>
#include <QBitmap>
#include <f2b.h>

#include <optional>

class GlyphInfoWidget : public QGraphicsWidget
{
public:
    static constexpr auto cellMargin = 6.0;
    static constexpr auto descriptionHeight = 50.0;

    GlyphInfoWidget(const Font::Glyph &glyph, char asciiCode, QSizeF imageSize, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    const QString description_;
    const QBitmap preview_;
    const QSizeF imageSize_;
};

#endif // GLYPHINFOWIDGET_H
