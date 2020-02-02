#include "facewidget.h"
#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"

#include <QGraphicsSceneEvent>

static constexpr auto col_count = 3;

FaceWidget::FaceWidget(QGraphicsItem *parent) :
    QGraphicsWidget(parent)
{
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);
}

void FaceWidget::load(const Font::Face &face)
{
    for (auto& item : childItems()) {
        if (item->zValue() == 0) {
            delete item;
        }
    }

    auto size = Font::qsize_with_size(face.glyph_size());
    size.rheight() += 6 + 50;
    size.rwidth() += 12;
    size.rheight() += 12;
//    size = size.grownBy({ 6, 6, 6, 6 });
    size.rheight() = qMax(size.height(), 100);
    size.rwidth() = qMax(size.width(), 80);

    itemSize_ = size;

    auto index = 0;
    for (const auto& g : face.glyphs()) {
        auto glyphWidget = new GlyphInfoWidget(g, ' ' + index);

        glyphWidget->setPreferredSize(size);

        layout_->addItem(glyphWidget, index / col_count, index % col_count, 1, 1);
        index += 1;
    }
}

void FaceWidget::setCurrentGlyphIndex(std::size_t index)
{
    auto item = layout()->itemAt(index);
    if (item) {
        setFocusForItem(item, true);
    }
}

void FaceWidget::setFocusForItem(QGraphicsLayoutItem *item, bool isFocused)
{
    if (focusWidget_ == nullptr) {
        focusWidget_ = std::make_unique<FocusWidget>(this);
        focusWidget_->setColor(Qt::black);
    }

    focusWidget_->setFocus(item, isFocused);
//    focused_item_ = isFocused ? item : nullptr;
}

bool FaceWidget::sceneEvent(QEvent *event)
{
    switch (event->type()) {
    case QActionEvent::GraphicsSceneMousePress:
    case QActionEvent::GraphicsSceneMouseDoubleClick:
        if (auto mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent *>(event)) {
            qreal leftMargin;
            qreal topMargin;
            layout_->getContentsMargins(&leftMargin, &topMargin, nullptr, nullptr);
            int row = static_cast<int>((mouseEvent->pos().y() - topMargin) / itemSize_.height());
            int col = static_cast<int>((mouseEvent->pos().x() - leftMargin) / itemSize_.width());
            qDebug() << mouseEvent << row << col;
            auto item = layout_->itemAt(row, col);
            setFocusForItem(item, true);
            emit currentGlyphIndexChanged(row * 3 + col);
        }
        break;
    default:
        break;
    }
    return QGraphicsWidget::sceneEvent(event);
}

