#ifndef GLYPHWIDGET_H
#define GLYPHWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include "f2b.h"

class FocusWidget;

class GlyphWidget : public QGraphicsWidget
{
public:
    explicit GlyphWidget(qreal pixel_size, QGraphicsItem *parent = nullptr);
    virtual ~GlyphWidget() = default;

    void loadGlyph(const Font::Glyph &glyph);

protected:
    bool sceneEvent(QEvent *event) override;

private:
    void updateLayout(uint8_t width, uint8_t height);
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);

    void setItemState(QGraphicsLayoutItem *item, bool isSelected);
    void toggleItemSet(QGraphicsLayoutItem *item);

    void handleKeyPress(QKeyEvent *keyEvent);
    void moveFocus(const QPoint& from, const QPoint& to);

    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
    FocusWidget *focus_widget_ { nullptr };
    QGraphicsLayoutItem *focused_item_ { nullptr };
    int width_ { 0 };
    int height_ { 0 };
    const qreal pixel_size_;
};

#endif // GLYPHWIDGET_H
