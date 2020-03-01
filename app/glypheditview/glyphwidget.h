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
    explicit GlyphWidget(const Font::Glyph& glyph, Font::Margins margins = {}, QGraphicsItem* parent = nullptr);
    virtual ~GlyphWidget() = default;

    void load(const Font::Glyph& glyph, Font::Margins margins = {});
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
    void updateIfNeeded(UpdateMode updateMode, std::optional<Font::Point> previousFocusedPixel);

    void togglePixel(Font::Point p);
    void setPixel(Font::Point p, bool value);

    Font::Point pointForEvent(QGraphicsSceneMouseEvent *event) const;

    Font::Glyph glyph_;
    Font::Margins margins_;
    BatchPixelChange affectedPixels_;
    std::optional<Font::Point> focusedPixel_ {};

    bool isDuringMouseMove_ { false };
    bool penState_ { false };
};

#endif // RAWGLYPHWIDGET_H
