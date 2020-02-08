#include "facewidget.h"
#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"

#include <QGraphicsSceneEvent>

static constexpr auto col_count = 3;
static constexpr auto printable_ascii_offset = ' ';
static constexpr auto min_cell_height = 100.0;
static constexpr auto max_image_width = FaceWidget::cell_width - 2 * GlyphInfoWidget::cellMargin;

FaceWidget::FaceWidget(QGraphicsItem *parent) :
    QGraphicsWidget(parent)
{
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);
}

void FaceWidget::load(const Font::Face &face)
{
    // TODO: Reuse items instead of deleting them all
    for (auto& item : childItems()) {
        if (item->zValue() == 0) {
            delete item;
        }
    }

    resetFocusWidget();

    qDebug() << childItems().count() << "child item(s)";

    // height: 6 + desc.height + 6 + img.height + 6
    // width: 6 + img.width + 6
    // max image width: 80 - 2*6 = 68

    QSizeF imageSize { Font::qsize_with_size(face.glyph_size()) };
    if (imageSize.width() > max_image_width) {
        imageSize.scale({ max_image_width, qInf() }, Qt::KeepAspectRatio);
    }
    auto size = imageSize;
    size.rheight() += GlyphInfoWidget::cellMargin + GlyphInfoWidget::descriptionHeight;
    size.rwidth() += 2 * GlyphInfoWidget::cellMargin;
    size.rheight() += 2 * GlyphInfoWidget::cellMargin;
    size.rheight() = qMax(size.height(), min_cell_height);
    size.rwidth() = qMax(size.width(), cell_width);

    itemSize_ = size;

    qDebug() << "NEW ITEM SIZE:" << itemSize_;

    auto index = 0;
    for (const auto& g : face.glyphs()) {
        auto glyphWidget = new GlyphInfoWidget(g, printable_ascii_offset + index, imageSize);

        // TODO: reduce number of these calls
        layout_->setRowFixedHeight(index / col_count, itemSize_.height());
        layout_->setColumnFixedWidth(index % col_count, itemSize_.width());

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
}

void FaceWidget::resetFocusWidget()
{
    if (focusWidget_ != nullptr) {
        focusWidget_->setFocus(nullptr);
    }
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
            if (item != nullptr) {
                setFocusForItem(item, true);
                emit currentGlyphIndexChanged(row * 3 + col);
            } else {
                resetFocusWidget();
            }
        }
        break;
    default:
        break;
    }
    return QGraphicsWidget::sceneEvent(event);
}

