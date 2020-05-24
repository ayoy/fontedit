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
    explicit GlyphWidget(const f2b::font::Glyph& glyph, f2b::font::Margins margins = {}, QGraphicsItem* parent = nullptr);
    virtual ~GlyphWidget() = default;

    void load(const f2b::font::Glyph& glyph, f2b::font::Margins margins = {});
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
    void updateIfNeeded(UpdateMode updateMode, std::optional<f2b::font::Point> previousFocusedPixel);

    void togglePixel(f2b::font::Point p);
    void setPixel(f2b::font::Point p, bool value);

    f2b::font::Point pointForEvent(QGraphicsSceneMouseEvent *event) const;

    f2b::font::Glyph glyph_;
    f2b::font::Margins margins_;
    BatchPixelChange affectedPixels_;
    std::optional<f2b::font::Point> focusedPixel_ {};

    bool isDuringMouseMove_ { false };
    bool penState_ { false };
};

#endif // RAWGLYPHWIDGET_H
