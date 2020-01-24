#ifndef GLYPHWIDGET_H
#define GLYPHWIDGET_H

#include <QGraphicsWidget>
#include <memory>

class QGraphicsGridLayout;
class FocusWidget;

class GlyphWidget : public QGraphicsWidget
{
public:
    GlyphWidget(uint8_t width, uint8_t height, qreal size, QGraphicsItem *parent = nullptr);
    virtual ~GlyphWidget() = default;

protected:
    bool sceneEvent(QEvent *event) override;

private:
    void setupFontLayout(uint8_t width, uint8_t height);
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);
    void toggleItemSet(QGraphicsLayoutItem *item);

    void handleKeyPress(QKeyEvent *keyEvent);
    void moveFocus(const QPoint& from, const QPoint& to);

    QGraphicsGridLayout *m_layout;
    FocusWidget *m_focusWidget;
    QGraphicsLayoutItem *m_focusedItem;
    const int m_width;
    const int m_height;
    const qreal m_pixelDimension;
};

#endif // GLYPHWIDGET_H
