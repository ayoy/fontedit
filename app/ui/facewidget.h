#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <f2b.h>
#include "focuswidget.h"

#include <memory>
#include <optional>

class GlyphInfoWidget;

class FaceWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static constexpr auto cell_width = 80.0;

    explicit FaceWidget(int columnCount = 3, QGraphicsItem *parent = nullptr);

    void load(const f2b::font::face& face, f2b::font::margins margins);
    void load(f2b::font::face& face, f2b::font::margins margins);
    void setCurrentGlyphIndex(std::optional<std::size_t> index);
    void updateGlyphInfo(std::size_t index, std::optional<f2b::font::glyph> glyph, std::optional<bool> isExported = {});

    bool showsNonExportedItems() const { return showsNonExportedItems_; }
    void setShowsNonExportedItems(bool isEnabled);

signals:
    void currentGlyphIndexChanged(std::optional<std::size_t> index);
    void glyphExportedStateChanged(std::size_t index, bool isExported);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void updateGeometry() override;

private:
    void handleMousePress(QGraphicsSceneMouseEvent *event);
    void reloadFace();
    void reset();
    QSizeF calculateImageSize(f2b::font::glyph_size glyph_size);
    void addGlyphInfoWidget(QGraphicsLayoutItem* glyphWidget, std::size_t index);
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);
    void resetFocusWidget();


    GlyphInfoWidget* glyphWidgetAtIndex(std::size_t index);
    GlyphInfoWidget* glyphWidgetAtPos(QPointF pos);
    QSize glyphCoordsAtPos(QPointF pos);

    QGraphicsLayoutItem *focusedItem_ { nullptr };
    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
    std::unique_ptr<FocusWidget> focusWidget_ { nullptr };
    QSizeF itemSize_;
    int columnCount_;
    bool showsNonExportedItems_;
    const f2b::font::face* face_ { nullptr };
    f2b::font::margins margins_;
};

#endif // FACEWIDGET_H
