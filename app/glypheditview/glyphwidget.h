#ifndef RAWGLYPHWIDGET_H
#define RAWGLYPHWIDGET_H

#include <QGraphicsWidget>
#include <f2b.h>
#include <memory>
#include <unordered_map>
#include <optional>

struct PointHash {
    size_t operator()(const Font::Point &p) const {
        return std::hash<std::size_t>()(p.x) ^ std::hash<std::size_t>()(p.y);
    }
};

class GlyphWidget : public QGraphicsWidget
{
    Q_OBJECT

    enum class UpdateMode {
        UpdateFocus,
        UpdateFocusAndPixels
    };

public:
    explicit GlyphWidget(const Font::Glyph& glyph, QGraphicsItem* parent = nullptr);
    virtual ~GlyphWidget() = default;

    void load(const Font::Glyph& glyph);
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

signals:
    void pixelChanged(Font::Point pos, bool isSelected);

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

    Font::Point pointForEvent(QGraphicsSceneMouseEvent *event) const;

    Font::Glyph glyph_;
    std::unordered_map<Font::Point,bool,PointHash> affectedItems_;
    std::optional<Font::Point> focusedPixel_ {};

    bool isDuringMouseMove_ { false };
    bool penState_ { false };
};

#endif // RAWGLYPHWIDGET_H
