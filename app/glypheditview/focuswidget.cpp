#include "focuswidget.h"
#include <QPainter>
#include <QDebug>

FocusWidget::FocusWidget(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{
    setZValue(1);
}

void FocusWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(QPen(QBrush(color_), 2));
    painter->drawRect(rect());
}

void FocusWidget::setFocus(QGraphicsLayoutItem *item, bool isFocused)
{
    if (item != nullptr) {
        if (isFocused) {
            setVisible(true);
            setGeometry(item->geometry());
        } else {
            setVisible(false);
        }
    }
}
