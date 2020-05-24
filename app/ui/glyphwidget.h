#ifndef RAWGLYPHWIDGET_H
#define RAWGLYPHWIDGET_H

#include <QGraphicsWidget>
#include <f2b.h>
#include <memory>
#include <optional>

#include "batchpixelchange.h"


class GlyphWidget : public QGraphicsWidget
{
    Q_OBJECT

    enum class UpdateMode {
        UpdateFocus,
        UpdateFocusAndPixels
    };

public:
    explicit GlyphWidget(const f2b::Font::Glyph& glyph, f2b::Font::Margins margins = {}, QGraphicsItem* parent = nullptr);
    virtual ~GlyphWidget() = default;

    void load(const f2b::Font::Glyph& glyph, f2b::Font::Margins margins = {});
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    void applyChange(const BatchPixelChange &change,
                     BatchPixelChange::ChangeType changeType = BatchPixelChange::ChangeType::Normal);

signals:
    void pixelsChanged(const BatchPixelChange& changes);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void handleMousePress(QGraphicsSceneMouseEvent *event);
    void updateIfNeeded(UpdateMode updateMode, std::optional<f2b::Font::Point> previousFocusedPixel);

    void togglePixel(f2b::Font::Point p);
    void setPixel(f2b::Font::Point p, bool value);

    f2b::Font::Point pointForEvent(QGraphicsSceneMouseEvent *event) const;

    f2b::Font::Glyph glyph_;
    f2b::Font::Margins margins_;
    BatchPixelChange affectedPixels_;
    std::optional<f2b::Font::Point> focusedPixel_ {};

    bool isDuringMouseMove_ { false };
    bool penState_ { false };
};

#endif // RAWGLYPHWIDGET_H
