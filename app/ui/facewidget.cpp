#include "facewidget.h"
#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"

#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QQueue>
#include <algorithm>
#include <iterator>

static constexpr auto printable_ascii_offset = ' ';
static constexpr auto min_cell_height = 120.0;
static constexpr auto min_image_height = min_cell_height - GlyphInfoWidget::descriptionHeight - 3 * GlyphInfoWidget::cellMargin;
static constexpr auto max_image_width = FaceWidget::cell_width - 2 * GlyphInfoWidget::cellMargin;

FaceWidget::FaceWidget(int columnCount, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    columnCount_ { columnCount }
{
    setFocusPolicy(Qt::ClickFocus);
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

QSizeF FaceWidget::calculateImageSize(f2b::font::Size glyph_size)
{
    // height: 6 + desc.height + 6 + img.height + 6
    // width: 6 + img.width + 6
    // max image width: 80 - 2*6 = 68

    QSizeF imageSize { f2b::font::qsize_with_size(glyph_size) };
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

void FaceWidget::load(const f2b::font::Face &face, f2b::font::Margins margins)
{
    face_ = &face;
    margins_ = margins;
    reset();
    auto imageSize = calculateImageSize(face.glyph_size());

    auto index = 0;
    for (const auto& g : face.glyphs()) {
        auto glyphWidget = new GlyphInfoWidget(g, index, true, printable_ascii_offset + index, imageSize, margins);
        glyphWidget->setIsExportedAdjustable(false);

        addGlyphInfoWidget(glyphWidget, index);
        index++;
    }
}

void FaceWidget::load(f2b::font::Face &face, f2b::font::Margins margins)
{
    face_ = &face;
    margins_ = margins;
    reloadFace();
}

void FaceWidget::reloadFace()
{
    reset();

    if (face_ == nullptr) {
        return;
    }

    auto imageSize = calculateImageSize(face_->glyph_size());

    auto index = 0;
    auto widgetIndex = 0;
    auto exportedGlyphIDs = face_->exported_glyph_ids();
    for (const auto& g : face_->glyphs()) {
        auto isExported = exportedGlyphIDs.find(index) != exportedGlyphIDs.end();

        if (isExported || showsNonExportedItems_) {
            auto glyphWidget = new GlyphInfoWidget(g, index, isExported, printable_ascii_offset + index, imageSize, margins_);

            connect(glyphWidget, &GlyphInfoWidget::isExportedChanged, [&, index] (bool isExported) {
                emit glyphExportedStateChanged(index, isExported);
            });

            addGlyphInfoWidget(glyphWidget, widgetIndex);
            widgetIndex++;
        }

        index++;
    }
}

void FaceWidget::addGlyphInfoWidget(QGraphicsLayoutItem *glyphWidget, std::size_t index)
{
    auto row = index / columnCount_;
    auto col = index % columnCount_;

    if (row == 0) {
        layout_->setColumnFixedWidth(index % columnCount_, itemSize_.width());
    }

    if (col == 0) {
        layout_->setRowFixedHeight(index / columnCount_, itemSize_.height());
    }

    layout_->addItem(glyphWidget, index / columnCount_, index % columnCount_, 1, 1);
}

void FaceWidget::setCurrentGlyphIndex(std::optional<std::size_t> index)
{
    if (index.has_value()) {
        auto item = glyphWidgetAtIndex(index.value());
        if (item) {
            setFocusForItem(item, true);
        }
    } else {
        clearFocus();
    }
}

void FaceWidget::updateGlyphInfo(std::size_t index, std::optional<f2b::font::Glyph> glyph, std::optional<bool> isExported)
{
    auto item = glyphWidgetAtIndex(index);
    if (item) {
        item->updateGlyph(glyph, isExported);
    }
}

GlyphInfoWidget* FaceWidget::glyphWidgetAtIndex(std::size_t index)
{
    int itemIndex = index;

    if (!showsNonExportedItems_) {
        itemIndex = std::count_if(face_->exported_glyph_ids().begin(),
                                  face_->exported_glyph_ids().find(index),
                                  [&](std::size_t i) { return i < index; });
    }

    return dynamic_cast<GlyphInfoWidget *>(layout_->itemAt(itemIndex / columnCount_,
                                                           itemIndex % columnCount_));
}

QSize FaceWidget::glyphCoordsAtPos(QPointF pos)
{
    qreal leftMargin;
    qreal topMargin;
    layout_->getContentsMargins(&leftMargin, &topMargin, nullptr, nullptr);
    int row = static_cast<int>((pos.y() - topMargin) / itemSize_.height());
    int col = static_cast<int>((pos.x() - leftMargin) / itemSize_.width());
    return { col, row };
}

GlyphInfoWidget* FaceWidget::glyphWidgetAtPos(QPointF pos)
{
    auto coords = glyphCoordsAtPos(pos);
    return dynamic_cast<GlyphInfoWidget *>(layout_->itemAt(coords.height(), coords.width()));
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

void FaceWidget::keyPressEvent(QKeyEvent *event)
{
    if (focusWidget_ == nullptr) {
        return;
    }

    auto coords = glyphCoordsAtPos(focusWidget_->pos());
    auto item = dynamic_cast<GlyphInfoWidget *>(layout_->itemAt(coords.height(), coords.width()));
    if (item == nullptr) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_H:
        if (coords.width() > 0)
            --coords.rwidth();
        break;
    case Qt::Key_Right:
    case Qt::Key_L:
        if (coords.width() < columnCount_ - 1)
            ++coords.rwidth();
        break;
    case Qt::Key_Up:
    case Qt::Key_K:
        if (coords.height() > 0)
            --coords.rheight();
        break;
    case Qt::Key_Down:
    case Qt::Key_J: {
        auto rowCount = (layout_->count() + columnCount_) / columnCount_;
        if (coords.height() < rowCount - 1)
            ++coords.rheight();
        break;
    }
    case Qt::Key_Space: {
        auto isExported = face_->exported_glyph_ids().find(item->glyphIndex()) != face_->exported_glyph_ids().end();
        emit glyphExportedStateChanged(item->glyphIndex(), !isExported);
    }
    }

    item = dynamic_cast<GlyphInfoWidget *>(layout_->itemAt(coords.height(), coords.width()));
    if (item) {
        setFocusForItem(item, true);
        emit currentGlyphIndexChanged(item->glyphIndex());
    }
}

void FaceWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    handleMousePress(event);
}

void FaceWidget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    handleMousePress(event);
}

void FaceWidget::handleMousePress(QGraphicsSceneMouseEvent *event)
{
    auto item = glyphWidgetAtPos(event->pos());
    if (item != nullptr) {
        setFocusForItem(item, true);
        emit currentGlyphIndexChanged(item->glyphIndex());
    } else {
        resetFocusWidget();
    }
}

void FaceWidget::updateGeometry()
{
    QGraphicsWidget::updateGeometry();
    if (auto fw = focusWidget_.get()) {
        fw->setFocus(focusedItem_, true);
    }
}

void FaceWidget::setShowsNonExportedItems(bool isEnabled)
{
    if (showsNonExportedItems_ != isEnabled) {
        showsNonExportedItems_ = isEnabled;
        if (face_ != nullptr && face_->exported_glyph_ids().size() != face_->num_glyphs()) {
            reloadFace();
        }
        if (auto fw = focusWidget_.get()) {
            auto item = glyphWidgetAtPos(fw->pos());
            if (item != nullptr) {
                setFocusForItem(item, true);
                emit currentGlyphIndexChanged(item->glyphIndex());
            }
        }
    }
}

