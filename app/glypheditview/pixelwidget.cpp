#include "pixelwidget.h"
#include <QPainter>
#include <iostream>
#include <QDebug>

PixelWidget::PixelWidget(QGraphicsItem *parent) : QGraphicsWidget(parent)
{
}

void PixelWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->fillRect(rect(), QBrush(isSet() ? Qt::black : Qt::white));
    painter->setPen(QPen(QBrush(Qt::darkGray), 1));
    painter->drawRect(rect());
}
