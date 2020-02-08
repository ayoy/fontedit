#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <f2b.h>
#include "../glypheditview/focuswidget.h"

#include <memory>

class FaceWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static constexpr auto cell_width = 80.0;

    explicit FaceWidget(QGraphicsItem *parent = nullptr);

    void load(const Font::Face &face);
    void setCurrentGlyphIndex(std::size_t index);

signals:
    void currentGlyphIndexChanged(std::size_t index);

protected:
    bool sceneEvent(QEvent *event) override;

private:
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);
    void resetFocusWidget();

    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
    std::unique_ptr<FocusWidget> focusWidget_ { nullptr };
    QSizeF itemSize_;
};

#endif // FACEWIDGET_H
