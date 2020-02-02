#include "facewidget.h"
#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"

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
    size = size.grownBy({ 6, 6, 6, 6 });
    size.rheight() = qMax(size.height(), 100);
    size.rwidth() = qMax(size.width(), 80);

    auto index = 0;
    for (const auto& g : face.glyphs()) {
        auto glyphWidget = new GlyphInfoWidget(g, ' ' + index);

        glyphWidget->setPreferredSize(size);

        layout_->addItem(glyphWidget, index / col_count, index % col_count, 1, 1);
        index += 1;
    }
}
