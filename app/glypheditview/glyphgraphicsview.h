#ifndef GLYPHGRAPHICSVIEW_H
#define GLYPHGRAPHICSVIEW_H

#include <QGraphicsView>

class GlyphGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GlyphGraphicsView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void setScale(qreal factor);
};

#endif // GLYPHGRAPHICSVIEW_H
