#include "glyphwidget.h"
#include "pixelwidget.h"
#include <QGraphicsGridLayout>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include "f2b_qt_compat.h"

GlyphWidget::GlyphWidget(qreal pixel_size, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    pixel_size_ { pixel_size }
{
    auto palette = QPalette(this->palette());
    palette.setColor(QPalette::WindowText, QColor(Qt::red));
    setPalette(palette);

    setFocusPolicy(Qt::ClickFocus);

    layout_->setSpacing(0);
    setLayout(layout_);
}

void GlyphWidget::load(const Font::Glyph &glyph)
{
    width_ = glyph.size().width;
    height_ = glyph.size().height;
    updateLayout();

    for (std::vector<bool>::size_type i = 0; i < glyph.pixels.size(); ++i) {
        setItemState(layout_->itemAt(i), glyph.pixels[i]);
    }
}

void GlyphWidget::updateLayout()
{
    if (static_cast<std::size_t>(layout_->rowCount()) == height_ &&
            static_cast<std::size_t>(layout_->columnCount()) == width_)
    {
        return;
    }

    // TODO: Handle changing glyph size (adding and/or removing items)
    // Currently it deletes all pixels every time
    for (auto& item : childItems()) {
        if (item->zValue() == 0) {
            delete item;
        }
    }

    if (focus_widget_ != nullptr) {
        focus_widget_->hide();
    }

    for (auto y = 0UL; y < height_; y++) {
        for (auto x = 0UL; x < width_; x++) {
            auto pixel = new PixelWidget();
            pixel->setPreferredSize(pixel_size_, pixel_size_);
            layout_->addItem(pixel, y, x, 1, 1);
        }
        qDebug() << "line" << y << "finished";
    }
}

bool GlyphWidget::sceneEvent(QEvent *event)
{
    switch (event->type()) {
    case QActionEvent::KeyPress:
        if (auto keyEvent = dynamic_cast<QKeyEvent *>(event)) {
            handleKeyPress(keyEvent);
        }
        break;
    case QActionEvent::GraphicsSceneMousePress:
    case QActionEvent::GraphicsSceneMouseDoubleClick:
        if (auto mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent *>(event)) {
            qreal leftMargin;
            qreal topMargin;
            layout_->getContentsMargins(&leftMargin, &topMargin, nullptr, nullptr);
            int row = static_cast<int>((mouseEvent->pos().y() - topMargin) / pixel_size_);
            int col = static_cast<int>((mouseEvent->pos().x() - leftMargin) / pixel_size_);
            qDebug() << mouseEvent << row << col;
            auto item = layout_->itemAt(row, col);
            setFocusForItem(item, true);
            toggleItemSet({ col, row });
        }
        break;
    default:
        break;
    }
    return QGraphicsWidget::sceneEvent(event);
}

void GlyphWidget::handleKeyPress(QKeyEvent *event) {
    if (focused_item_ == nullptr) {
        return;
    }

    const auto pos = focused_item_->geometry().topLeft();
    qreal leftMargin;
    qreal topMargin;
    layout_->getContentsMargins(&leftMargin, &topMargin, nullptr, nullptr);

    const QPoint current(static_cast<int>((pos.x() - leftMargin) / pixel_size_),
                         static_cast<int>((pos.y() - topMargin) / pixel_size_));
    QPoint updated(current);

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_H:
        updated.setX(qMax(current.x() - 1, 0));
        moveFocus(current, updated);
        break;
    case Qt::Key_Right:
    case Qt::Key_L:
        updated.setX(qMin(current.x() + 1, static_cast<int>(width_) - 1));
        moveFocus(current, updated);
        break;
    case Qt::Key_Up:
    case Qt::Key_K:
        updated.setY(qMax(current.y() - 1, 0));
        moveFocus(current, updated);
        break;
    case Qt::Key_Down:
    case Qt::Key_J:
        updated.setY(qMin(current.y() + 1, static_cast<int>(height_) - 1));
        moveFocus(current, updated);
        break;
    case Qt::Key_Space:
        toggleItemSet(updated);
        break;
    }
}

void GlyphWidget::moveFocus(const QPoint &from, const QPoint &to)
{
    if (to != from) {
        qDebug() << from << to;
        auto fromItem = layout_->itemAt(from.y(), from.x());
        auto toItem = layout_->itemAt(to.y(), to.x());
        setFocusForItem(fromItem, false);
        setFocusForItem(toItem, true);
    }
}

void GlyphWidget::setFocusForItem(QGraphicsLayoutItem *item, bool isFocused)
{
    if (focus_widget_ == nullptr) {
        focus_widget_ = std::make_unique<FocusWidget>(this);
    }

    focus_widget_->setFocus(item, isFocused);
    focused_item_ = isFocused ? item : nullptr;
}

void GlyphWidget::setItemState(QGraphicsLayoutItem *item, bool isSelected)
{
    if (auto pixel = dynamic_cast<PixelWidget *>(item)) {
        pixel->setSet(isSelected);
    }
}

void GlyphWidget::toggleItemSet(const QPoint &pos)
{
    auto item = layout_->itemAt(pos.y(), pos.x());
    if (auto pixel = dynamic_cast<PixelWidget *>(item)) {
        bool state = !pixel->isSet();
        pixel->setSet(state);

        auto glyphPos = Font::point_with_qpoint(pos);

        emit pixelChanged(glyphPos, state);
    }
}
