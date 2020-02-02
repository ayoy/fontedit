#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsWidget>
#include <QGraphicsGridLayout>
#include "f2b.h"

class FaceWidget : public QGraphicsWidget
{
public:
    explicit FaceWidget(QGraphicsItem *parent = nullptr);

    void load(const Font::Face &face);

private:
    QGraphicsGridLayout *layout_ { new QGraphicsGridLayout() };
};

#endif // FACEWIDGET_H
