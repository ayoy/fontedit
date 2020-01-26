#include "graphicsview.h"
#include <QTouchEvent>
#include <QDebug>
#include <QTimeLine>
#include <cmath>

static const auto max_zoom_level = 2.0;
static const auto min_zoom_level = 0.1;
static const auto zoom_factor = 1.01;

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
{
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    auto isZooming = event->modifiers().testFlag(Qt::ControlModifier);

    if (isZooming && !event->angleDelta().isNull()) {
        qreal factor = pow(zoom_factor, event->angleDelta().y());
        setScale(factor);
    } else {
        QGraphicsView::wheelEvent(event);
    }
}

void GraphicsView::setScale(qreal factor)
{
    auto transform = this->transform();

    bool isZoomingInBeyondLimit { factor > 1.0 && transform.m11() > max_zoom_level };
    bool isZoomingOutBeyondLimit { factor < 1.0 && transform.m11() < min_zoom_level };

    if (isZoomingInBeyondLimit || isZoomingOutBeyondLimit) {
        return;
    }

    auto targetTransform = transform.scale(factor, factor);

    if (targetTransform.m11() > max_zoom_level) {
        auto clippedFactor = max_zoom_level / transform.m11();
        targetTransform = transform.scale(clippedFactor, clippedFactor);
    } else if (targetTransform.m11() < min_zoom_level) {
        auto clippedFactor = min_zoom_level / transform.m11();
        targetTransform = transform.scale(clippedFactor, clippedFactor);
    }

    setTransform(targetTransform);
}
