#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include <f2b.h>
#include "focuswidget.h"

#include <memory>

class FaceWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static constexpr auto cell_width = 80.0;

    explicit FaceWidget(int columnCount = 3, QGraphicsItem *parent = nullptr);

    void load(const Font::Face& face, Font::Margins margins);
    void setCurrentGlyphIndex(std::size_t index);
    void updateGlyphPreview(std::size_t index, const Font::Glyph& glyph);

signals:
    void currentGlyphIndexChanged(std::size_t index);

protected:
    bool sceneEvent(QEvent *event) override;
    void updateGeometry() override;

private:
    void setFocusForItem(QGraphicsLayoutItem *item, bool isFocused);
    void resetFocusWidget();

    QGraphicsLayoutItem *focusedItem_ { nullptr };
    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
    std::unique_ptr<FocusWidget> focusWidget_ { nullptr };
    QSizeF itemSize_;
    int columnCount_;
};

#endif // FACEWIDGET_H
