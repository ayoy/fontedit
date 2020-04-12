#ifndef GLYPHINFOWIDGET_H
#define GLYPHINFOWIDGET_H

#include <QGraphicsWidget>
#include <QPixmap>
#include <f2b.h>

#include <optional>

class GlyphInfoWidget : public QGraphicsWidget
{
public:
    static constexpr auto cellMargin = 6.0;
    static constexpr auto descriptionHeight = 50.0;

    GlyphInfoWidget(const Font::Glyph& glyph, bool isExported,
                    unsigned char asciiCode, QSizeF imageSize,
                    Font::Margins margins = {}, QGraphicsItem *parent = nullptr);

    void updateGlyph(const Font::Glyph& glyph, std::optional<bool> isExported = {}, std::optional<Font::Margins> margins = {});

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    const QString description_;
    const QSizeF imageSize_;
    bool isExported_;
    QImage preview_;
    Font::Margins margins_;
};

#endif // GLYPHINFOWIDGET_H
