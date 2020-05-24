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
    explicit GlyphWidget(const f2b::font::glyph& glyph, f2b::font::margins margins = {}, QGraphicsItem* parent = nullptr);
    virtual ~GlyphWidget() = default;

    void load(const f2b::font::glyph& glyph, f2b::font::margins margins = {});
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
    void updateIfNeeded(UpdateMode updateMode, std::optional<f2b::font::point> previousFocusedPixel);

    void togglePixel(f2b::font::point p);
    void setPixel(f2b::font::point p, bool value);

    f2b::font::point pointForEvent(QGraphicsSceneMouseEvent *event) const;

    f2b::font::glyph glyph_;
    f2b::font::margins margins_;
    BatchPixelChange affectedPixels_;
    std::optional<f2b::font::point> focusedPixel_ {};

    bool isDuringMouseMove_ { false };
    bool penState_ { false };
};

#endif // RAWGLYPHWIDGET_H
