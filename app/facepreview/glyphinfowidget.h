#ifndef GLYPHINFOWIDGET_H
#define GLYPHINFOWIDGET_H

#include <QGraphicsWidget>
#include <QBitmap>
#include <f2b.h>

class GlyphInfoWidget : public QGraphicsWidget
{
public:
    GlyphInfoWidget(const Font::Glyph &glyph, char asciiCode, QGraphicsItem *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    QString description;
    QBitmap preview;
};

#endif // GLYPHINFOWIDGET_H
