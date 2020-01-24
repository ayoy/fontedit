#include "graphicsview.h"
#include <QTouchEvent>
#include <QDebug>
#include <QTimeLine>
#include <cmath>

static const auto maxZoomLevel = 2.0;
static const auto minZoomLevel = 0.1;

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    auto isZooming = event->modifiers().testFlag(Qt::ControlModifier);

    if (isZooming && !event->angleDelta().isNull()) {
        qreal factor = pow(1.01, event->angleDelta().y());
        setScale(factor);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void GraphicsView::setScale(qreal factor)
{
    auto transform = this->transform();

    bool isZoomingInBeyondLimit { factor > 1.0 && transform.m11() > maxZoomLevel };
    bool isZoomingOutBeyondLimit { factor < 1.0 && transform.m11() < minZoomLevel };

    if (isZoomingInBeyondLimit || isZoomingOutBeyondLimit) {
        return;
    }

    auto targetTransform = transform.scale(factor, factor);

    if (targetTransform.m11() > maxZoomLevel) {
        auto clippedFactor = maxZoomLevel / transform.m11();
        targetTransform = transform.scale(clippedFactor, clippedFactor);
    } else if (targetTransform.m11() < minZoomLevel) {
        auto clippedFactor = minZoomLevel / transform.m11();
        targetTransform = transform.scale(clippedFactor, clippedFactor);
    }

    setTransform(targetTransform);
}
