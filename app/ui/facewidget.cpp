#include "facewidget.h"
#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"

#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QGraphicsView>

static constexpr auto printable_ascii_offset = ' ';
static constexpr auto min_cell_height = 120.0;
static constexpr auto min_image_height = min_cell_height - GlyphInfoWidget::descriptionHeight - 3 * GlyphInfoWidget::cellMargin;
static constexpr auto max_image_width = FaceWidget::cell_width - 2 * GlyphInfoWidget::cellMargin;

FaceWidget::FaceWidget(int columnCount, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    columnCount_ { columnCount }
{
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    setLayout(layout_);
}

void FaceWidget::reset()
{
    // TODO: Reuse items instead of deleting them all
    for (auto& item : childItems()) {
        if (item->zValue() == 0) {
            delete item;
        }
    }

    resetFocusWidget();
}

QSizeF FaceWidget::calculateImageSize(Font::Size glyph_size)
{
    // height: 6 + desc.height + 6 + img.height + 6
    // width: 6 + img.width + 6
    // max image width: 80 - 2*6 = 68

    QSizeF imageSize { Font::qsize_with_size(glyph_size) };
    if (imageSize.width() > max_image_width) {
        imageSize.scale(max_image_width, qInf(), Qt::KeepAspectRatio);
    } else if (imageSize.height() < min_image_height) {
        imageSize.scale(qInf(), min_image_height, Qt::KeepAspectRatio);
    }
    auto size = imageSize;
    size.rheight() += GlyphInfoWidget::cellMargin + GlyphInfoWidget::descriptionHeight;
    size.rwidth() += 2 * GlyphInfoWidget::cellMargin;
    size.rheight() += 2 * GlyphInfoWidget::cellMargin;
    size.rheight() = qMax(size.height(), min_cell_height);
    size.rwidth() = qMax(size.width(), cell_width);

    itemSize_ = size;
    qDebug() << "NEW ITEM SIZE:" << itemSize_;

    return imageSize;
}

void FaceWidget::load(const Font::Face &face, Font::Margins margins)
{
    reset();
    auto imageSize = calculateImageSize(face.glyph_size());

    auto index = 0;
    for (const auto& g : face.glyphs()) {
        auto glyphWidget = new GlyphInfoWidget(g, true, printable_ascii_offset + index, imageSize, margins);
        glyphWidget->setIsExportedAdjustable(false);

        // TODO: reduce number of these calls
        layout_->setRowFixedHeight(index / columnCount_, itemSize_.height());
        layout_->setColumnFixedWidth(index % columnCount_, itemSize_.width());

        layout_->addItem(glyphWidget, index / columnCount_, index % columnCount_, 1, 1);
        index += 1;
    }
}

void FaceWidget::load(Font::Face &face, Font::Margins margins)
{
    reset();
    auto imageSize = calculateImageSize(face.glyph_size());

    auto index = 0;
    auto exportedGlyphIDs = face.exported_glyph_ids();
    for (const auto& g : face.glyphs()) {
        auto isExported = exportedGlyphIDs.find(index) != exportedGlyphIDs.end();
        auto glyphWidget = new GlyphInfoWidget(g, isExported, printable_ascii_offset + index, imageSize, margins);

        connect(glyphWidget, &GlyphInfoWidget::isExportedChanged, [&, index](bool isExported) {
            emit glyphExportedStateChanged(index, isExported);
        });

        // TODO: reduce number of these calls
        layout_->setRowFixedHeight(index / columnCount_, itemSize_.height());
        layout_->setColumnFixedWidth(index % columnCount_, itemSize_.width());

        layout_->addItem(glyphWidget, index / columnCount_, index % columnCount_, 1, 1);
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

void FaceWidget::updateGlyphPreview(std::size_t index, const Font::Glyph &glyph)
{
    auto item = dynamic_cast<GlyphInfoWidget *>(layout()->itemAt(index));
    if (item) {
        item->updateGlyph(glyph);
    }
}

void FaceWidget::setFocusForItem(QGraphicsLayoutItem *item, bool isFocused)
{
    if (focusWidget_ == nullptr) {
        focusWidget_ = std::make_unique<FocusWidget>(this);
        focusWidget_->setZValue(1);
        focusWidget_->setColor(Qt::blue);
    }

    focusedItem_ = item;
    focusWidget_->setFocus(item, isFocused);
    if (isFocused) {
        QGraphicsView *graphicsView = scene()->views().first();
        if (graphicsView != nullptr)
            graphicsView->ensureVisible(focusWidget_->geometry());
    }
}

void FaceWidget::resetFocusWidget()
{
    if (focusWidget_ != nullptr) {
        focusWidget_->setFocus(nullptr);
    }
    focusedItem_ = nullptr;
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
            auto item = layout_->itemAt(row, col);
            if (item != nullptr) {
                setFocusForItem(item, true);
                emit currentGlyphIndexChanged(row * columnCount_ + col);
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

void FaceWidget::updateGeometry()
{
    QGraphicsWidget::updateGeometry();
    if (auto fw = focusWidget_.get()) {
        fw->setFocus(focusedItem_, true);
    }
}
