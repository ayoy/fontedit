#ifndef FOCUSWIDGET_H
#define FOCUSWIDGET_H

#include <QGraphicsWidget>

class QGraphicsLayoutItem;

class FocusWidget : public QGraphicsWidget
{
public:
    explicit FocusWidget(QGraphicsItem *parent = nullptr);
    virtual ~FocusWidget() = default;

    void setFocus(QGraphicsLayoutItem *item, bool isFocused);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
};

#endif // FOCUSWIDGET_H
