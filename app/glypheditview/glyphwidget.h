#ifndef GLYPHWIDGET_H
#define GLYPHWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include "f2b.h"
#include "focuswidget.h"

#include <memory>
#include <unordered_map>

struct QPointHash {
    size_t operator()(const QPoint &p) const {
        return std::hash<int>()(p.x()) ^ std::hash<int>()(p.y());
    }
};

class GlyphWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit GlyphWidget(qreal pixel_size, QGraphicsItem *parent = nullptr);
    virtual ~GlyphWidget() = default;

    void load(const Font::Glyph &glyph);

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
    void updateLayout();
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);

    void setItemState(QGraphicsLayoutItem *item, bool isSelected);
    void setItemState(const QPoint &pos, bool isSelected);
    void toggleItemSet(const QPoint &pos);

    void moveFocus(const QPoint &to);

    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
    std::unique_ptr<FocusWidget> focus_widget_ { nullptr };
    QGraphicsLayoutItem *focused_item_ { nullptr };
    std::size_t width_ { 0 };
    std::size_t height_ { 0 };
    const qreal pixel_size_;
    qreal topMargin_;
    qreal leftMargin_;


    bool isDuringMouseMove_ { false };
    std::unordered_map<QPoint,bool,QPointHash> affectedItems_;
    bool penState_ { false };
};

#endif // GLYPHWIDGET_H
