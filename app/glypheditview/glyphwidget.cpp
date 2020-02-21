#include "glyphwidget.h"
#include <QPainter>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QStyleOptionGraphicsItem>
#include <QElapsedTimer>

#include <iostream>

static constexpr qreal gridSize = 20;

QRectF rectForPoint(const Font::Point& point)
{
    return QRectF(QPointF(point.x * gridSize, point.y * gridSize),
                  QSizeF(gridSize, gridSize));
}


GlyphWidget::GlyphWidget(const Font::Glyph& glyph, QGraphicsItem* parent) :
    QGraphicsWidget(parent),
    glyph_ { glyph }
{
    setFocusPolicy(Qt::ClickFocus);
    setPreferredSize({ gridSize * static_cast<qreal>(glyph.size().width),
                       gridSize * static_cast<qreal>(glyph.size().height) });
}

void GlyphWidget::load(const Font::Glyph &glyph)
{
    glyph_ = glyph;
    setPreferredSize({ gridSize * static_cast<qreal>(glyph.size().width),
                       gridSize * static_cast<qreal>(glyph.size().height) });
    update();
}

QRectF GlyphWidget::boundingRect() const
{
    return QRectF(-0.25,
                  -0.25,
                  glyph_.size().width * gridSize + 0.25,
                  glyph_.size().height * gridSize + 0.25);
}

void GlyphWidget::togglePixel(Font::Point p)
{
    setPixel(p, !glyph_.is_pixel_set(p));
}

void GlyphWidget::setPixel(Font::Point p, bool value)
{
    if (glyph_.is_pixel_set(p) != value) {
        glyph_.set_pixel_set(p, value);
        affectedPixels_.add(p, value);

        if (!isDuringMouseMove_) {
            emit pixelsChanged(affectedPixels_);
            affectedPixels_.changes.clear();
        }
    }
}

void GlyphWidget::applyChange(const BatchPixelChange &change, BatchPixelChange::ChangeType changeType)
{
    //
    // Apply only if there's no affected pixels (no operation in progress)
    // - this is the initial call to redo() action.
    //
    if (affectedPixels_.changes.size() == 0) {
        change.apply(glyph_, changeType);
        update();
    }
}


void GlyphWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

//    QElapsedTimer timer;
//    timer.start();

    QRectF rect = QRectF(0, 0,
                         glyph_.size().width * gridSize,
                         glyph_.size().height * gridSize);

    painter->setPen(QPen(QBrush(Qt::darkGray), 0.5));

    painter->fillRect(rect, Qt::white);
    painter->drawRect(rect);

    for (std::size_t row = 0; row < glyph_.size().height; ++row) {
        for (std::size_t col = 0; col < glyph_.size().width; ++col) {
            Font::Point p { col, row };
            if (glyph_.is_pixel_set(p)) {
                painter->fillRect(rectForPoint(p), Qt::black);
            }
        }
    }

    for (qreal w = 0; w < rect.width(); w += gridSize) {
        painter->drawLine(QLineF(QPointF(w, rect.top()), QPointF(w, rect.bottom())));
    }
    for (qreal h = 0; h < rect.height(); h += gridSize) {
        painter->drawLine(QLineF(QPointF(rect.left(), h), QPointF(rect.right(), h)));
    }
    if (focusedPixel_.has_value()) {
        painter->setPen(QPen(QBrush(Qt::red), 1));
        painter->drawRect(rectForPoint(focusedPixel_.value()));
    }

//    qDebug() << __FUNCTION__ << option->exposedRect << boundingRect()
//             << option->exposedRect.toRect().contains(boundingRect().toRect())
//             << QString("(%1ms)").arg(timer.elapsed());
}

void GlyphWidget::keyPressEvent(QKeyEvent *event)
{
    if (!focusedPixel_.has_value()) {
        return;
    }

    auto previousPixel = focusedPixel_;
    auto updateMode = UpdateMode::UpdateFocus;

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_H:
        if (focusedPixel_->x > 0)
            --focusedPixel_->x;
        break;
    case Qt::Key_Right:
    case Qt::Key_L:
        if (focusedPixel_->x < glyph_.size().width - 1)
            ++focusedPixel_->x;
        break;
    case Qt::Key_Up:
    case Qt::Key_K:
        if (focusedPixel_->y > 0)
            --focusedPixel_->y;
        break;
    case Qt::Key_Down:
    case Qt::Key_J:
        if (focusedPixel_->y < glyph_.size().height - 1)
            ++focusedPixel_->y;
        break;
    case Qt::Key_Space:
        togglePixel(focusedPixel_.value());
        updateMode = UpdateMode::UpdateFocusAndPixels;
        break;
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
        penState_ = false;
        break;
    }

    updateIfNeeded(updateMode, previousPixel);
}

void GlyphWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (!isDuringMouseMove_) {
        return;
    }

    if (event->key() == Qt::Key_Alt || event->key() == Qt::Key_AltGr) {
        penState_ = true;
    }
}

void GlyphWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    handleMousePress(event);
}

void GlyphWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    handleMousePress(event);
}

void GlyphWidget::handleMousePress(QGraphicsSceneMouseEvent *event)
{
    Font::Point currentPixel = pointForEvent(event);
    qDebug() << event << currentPixel.x << currentPixel.y;

    penState_ = !event->modifiers().testFlag(Qt::AltModifier);
    affectedPixels_.changes.clear();
    isDuringMouseMove_ = true;

    setPixel(currentPixel, penState_);

    auto previousPixel = focusedPixel_;
    focusedPixel_ = currentPixel;
    updateIfNeeded(UpdateMode::UpdateFocusAndPixels, previousPixel);
}

void GlyphWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    Font::Point currentPixel = pointForEvent(event);

    auto updateMode = UpdateMode::UpdateFocus;

    // If item not visited or visited with a different state
    if (affectedPixels_.changes.find(currentPixel) == affectedPixels_.changes.end() ||
            affectedPixels_.changes[currentPixel] != penState_)
    {
//        qDebug() << "mouse move to new item" << currentPixel.x << currentPixel.y << penState_;
        updateMode = UpdateMode::UpdateFocusAndPixels;

        setPixel(currentPixel, penState_);
    }

    auto previousPixel = focusedPixel_;
    focusedPixel_ = currentPixel;
    updateIfNeeded(updateMode, previousPixel);
}

void GlyphWidget::updateIfNeeded(UpdateMode updateMode, std::optional<Font::Point> previousFocusedPixel)
{
    QRectF rect;

    if (focusedPixel_.has_value()
            && previousFocusedPixel.has_value()
            && *focusedPixel_ == *previousFocusedPixel
            && updateMode == UpdateMode::UpdateFocus)
    {
        // no change in focus so no redraw
        return;
    }

    if (focusedPixel_.has_value()) {
        rect = rectForPoint(focusedPixel_.value());
    }
    if (previousFocusedPixel.has_value()) {
        auto previousRect = rectForPoint(previousFocusedPixel.value());
        if (rect.isValid()) {
            rect = rect.united(previousRect);
        } else {
            rect = previousRect;
        }
    }

    if (rect.isValid()) {
        update(rect.marginsAdded({0.5, 0.5, 0.5, 0.5}));
    }
}

void GlyphWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    isDuringMouseMove_ = false;

    for (const auto& i : affectedPixels_.changes) {
        std::cout << "((" << i.first.x << ", " << i.first.y << "), " << i.second << ") ";
    }
    std::cout << std::endl;

    emit pixelsChanged(affectedPixels_);
    affectedPixels_.changes.clear();
}

Font::Point GlyphWidget::pointForEvent(QGraphicsSceneMouseEvent *event) const
{
    auto row = static_cast<std::size_t>(std::max(event->pos().y() / gridSize, 0.0));
    auto col = static_cast<std::size_t>(std::max(event->pos().x() / gridSize, 0.0));
    row = std::min(row, glyph_.size().height-1);
    col = std::min(col, glyph_.size().width-1);

    return { col, row };
}
