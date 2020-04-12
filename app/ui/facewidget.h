#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <f2b.h>
#include "focuswidget.h"

#include <memory>

class GlyphInfoWidget;

class FaceWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static constexpr auto cell_width = 80.0;

    explicit FaceWidget(int columnCount = 3, QGraphicsItem *parent = nullptr);

    void load(const Font::Face& face, Font::Margins margins);
    void load(Font::Face& face, Font::Margins margins);
    void setCurrentGlyphIndex(std::size_t index);
    void updateGlyphPreview(std::size_t index, const Font::Glyph& glyph);

    bool showsNonExportedItems() const { return showsNonExportedItems_; }
    void setShowsNonExportedItems(bool isEnabled) { showsNonExportedItems_ = isEnabled; }

signals:
    void currentGlyphIndexChanged(std::size_t index);
    void glyphExportedStateChanged(std::size_t index, bool isExported);

protected:
    bool sceneEvent(QEvent *event) override;
    void updateGeometry() override;

private:
    void reset();
    QSizeF calculateImageSize(Font::Size glyph_size);
    void addGlyphInfoWidget(QGraphicsLayoutItem* glyphWidget, std::size_t index);
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);
    void resetFocusWidget();

//    void hideItem(std::size_t index);

    QGraphicsLayoutItem *focusedItem_ { nullptr };
    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
    std::unique_ptr<FocusWidget> focusWidget_ { nullptr };
    QSizeF itemSize_;
    int columnCount_;
    bool showsNonExportedItems_ { true };
//    const Font::Face* face_;
};

#endif // FACEWIDGET_H
