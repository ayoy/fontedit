#ifndef GLYPHGRAPHICSVIEW_H
#define GLYPHGRAPHICSVIEW_H

#include <QGraphicsView>

class GlyphGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GlyphGraphicsView(QWidget *parent = nullptr);
    void setupFontLayout(uint8_t width, uint8_t height, qreal size);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setScale(qreal factor);
    std::unique_ptr<QGraphicsScene> scene_;
};

#endif // GLYPHGRAPHICSVIEW_H
