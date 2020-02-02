#ifndef GLYPHWIDGET_H
#define GLYPHWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include "f2b.h"
#include <memory>
#include "focuswidget.h"

class GlyphWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    explicit GlyphWidget(qreal pixel_size, QGraphicsItem *parent = nullptr);
    virtual ~GlyphWidget() = default;

    void loadGlyph(const Font::Glyph &glyph);

signals:
    void pixelChanged(Font::Point pos, bool isSelected);

protected:
    bool sceneEvent(QEvent *event) override;

private:
    void updateLayout();
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);

    void setItemState(QGraphicsLayoutItem *item, bool isSelected);
    void toggleItemSet(const QPoint &pos);

    void handleKeyPress(QKeyEvent *keyEvent);
    void moveFocus(const QPoint &from, const QPoint &to);

    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
    std::unique_ptr<FocusWidget> focus_widget_ { nullptr };
    QGraphicsLayoutItem *focused_item_ { nullptr };
    std::size_t width_ { 0 };
    std::size_t height_ { 0 };
    const qreal pixel_size_;
};

#endif // GLYPHWIDGET_H
