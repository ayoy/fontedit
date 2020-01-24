#include "pixelitem.h"
#include <QPainter>
#include <iostream>
#include <QDebug>

PixelItem::PixelItem(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
    , m_set { false }
{
}

void PixelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->fillRect(rect(), QBrush(isSet() ? Qt::black : Qt::white));
    painter->setPen(QPen(QBrush(Qt::darkGray), 1));
    painter->drawRect(rect());
}
