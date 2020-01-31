#ifndef GLYPHGRAPHICSVIEW_H
#define GLYPHGRAPHICSVIEW_H

#include <QGraphicsView>
#include <optional>
#include "f2b.h"

class GlyphWidget;

class GlyphGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GlyphGraphicsView(QWidget *parent = nullptr);

    void displayGlyph(const Font::Glyph &glyph);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setScale(qreal factor);

    std::unique_ptr<QGraphicsScene> scene_;
    std::optional<Font::Glyph> glyph_ { std::nullopt };

    GlyphWidget *glyphWidget_ { nullptr };
};

#endif // GLYPHGRAPHICSVIEW_H
