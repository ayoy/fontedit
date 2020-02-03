#ifndef FOCUSWIDGET_H
#define FOCUSWIDGET_H

#include <QGraphicsWidget>

class QGraphicsLayoutItem;

class FocusWidget : public QGraphicsWidget
{
public:
    explicit FocusWidget(QGraphicsItem *parent = nullptr);
    virtual ~FocusWidget() = default;

    void setFocus(QGraphicsLayoutItem *item, bool isFocused = true);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    const QColor& color() const { return color_; }
    void setColor(const QColor &color) { color_ = color; update(); }

private:
    QColor color_ { Qt::red };
};

#endif // FOCUSWIDGET_H
