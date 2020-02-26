#ifndef GLYPHGRAPHICSVIEW_H
#define GLYPHGRAPHICSVIEW_H

#include <QGraphicsView>
#include <optional>
#include "f2b.h"

class GlyphWidget;

class GlyphGraphicsView : public QGraphicsView
{
public:
    explicit GlyphGraphicsView(QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void setScale(qreal factor);

    std::unique_ptr<QGraphicsScene> scene_;
};

#endif // GLYPHGRAPHICSVIEW_H
