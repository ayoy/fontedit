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
    layout_->getContentsMargins(&leftMargin_, &topMargin_, nullptr, nullptr);
}

void GlyphWidget::load(const Font::Glyph &glyph)
{
    setVisible(true);
    width_ = glyph.size().width;
    height_ = glyph.size().height;
    updateLayout();

    auto pixels = glyph.pixels();
    for (std::vector<bool>::size_type i = 0; i < pixels.size(); ++i) {
        setItemState(layout_->itemAt(i), pixels[i]);
    }
}

void GlyphWidget::clear()
{
    setVisible(false);
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
//        qDebug() << "line" << y << "layout complete";
    }
}

void GlyphWidget::keyPressEvent(QKeyEvent *event)
{
    if (focused_item_ == nullptr) {
        return;
    }

    const auto pos = focused_item_->geometry().topLeft();
    QPoint itemPos(static_cast<int>((pos.x() - leftMargin_) / pixel_size_),
                   static_cast<int>((pos.y() - topMargin_) / pixel_size_));

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_H:
        itemPos.setX(qMax(itemPos.x() - 1, 0));
        moveFocus(itemPos);
        break;
    case Qt::Key_Right:
    case Qt::Key_L:
        itemPos.setX(qMin(itemPos.x() + 1, static_cast<int>(width_) - 1));
        moveFocus(itemPos);
        break;
    case Qt::Key_Up:
    case Qt::Key_K:
        itemPos.setY(qMax(itemPos.y() - 1, 0));
        moveFocus(itemPos);
        break;
    case Qt::Key_Down:
    case Qt::Key_J:
        itemPos.setY(qMin(itemPos.y() + 1, static_cast<int>(height_) - 1));
        moveFocus(itemPos);
        break;
    case Qt::Key_Space:
        toggleItemSet(itemPos);
        break;
    case Qt::Key_Alt:
    case Qt::Key_AltGr:
        penState_ = false;
        break;
    }
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
    int row = static_cast<int>((event->pos().y() - topMargin_) / pixel_size_);
    int col = static_cast<int>((event->pos().x() - leftMargin_) / pixel_size_);
    qDebug() << event << row << col;
    auto item = layout_->itemAt(row, col);

    QPoint itemPos { col, row };

    penState_ = !event->modifiers().testFlag(Qt::AltModifier);
    setFocusForItem(item, true);
    setItemState(itemPos, penState_);

    affectedItems_.clear();
    affectedItems_[itemPos] = penState_;
    isDuringMouseMove_ = true;
}

void GlyphWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    int row = static_cast<int>((event->pos().y() - topMargin_) / pixel_size_);
    int col = static_cast<int>((event->pos().x() - leftMargin_) / pixel_size_);

    QPoint itemPos { col, row };

    // If item not visited or visited with a different state
    if (affectedItems_.find(itemPos) == affectedItems_.end() ||
            affectedItems_[itemPos] != penState_)
    {
        affectedItems_[itemPos] = penState_;
        qDebug() << "mouse move to new item" << row << col << penState_;
        setItemState(itemPos, penState_);
        moveFocus(itemPos);
    }
}

void GlyphWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    isDuringMouseMove_ = false;

    QDebug d(QtDebugMsg);
    for (const auto& i : affectedItems_) {
        d << i.first;
    }
}

void GlyphWidget::moveFocus(const QPoint &to)
{
    if (auto toItem = layout_->itemAt(to.y(), to.x())) {
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

void GlyphWidget::setItemState(const QPoint &pos, bool isSelected)
{
    auto item = layout_->itemAt(pos.y(), pos.x());
    if (auto pixel = dynamic_cast<PixelWidget *>(item)) {
        if (pixel->isSet() != isSelected) {

            pixel->setSet(isSelected);
            auto glyphPos = Font::point_with_qpoint(pos);
            emit pixelChanged(glyphPos, isSelected);
        }
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
