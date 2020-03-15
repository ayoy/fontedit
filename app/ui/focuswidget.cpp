#include "focuswidget.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

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
            ensureVisible();
        } else {
            setVisible(false);
        }
    } else {
        setVisible(false);
    }
}

void FocusWidget::ensureVisible()
{
    QGraphicsView *graphicsView = scene()->views().first();
    if (graphicsView != nullptr)
        graphicsView->ensureVisible(this);
}
