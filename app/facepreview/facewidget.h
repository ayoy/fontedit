#ifndef FACEWIDGET_H
#define FACEWIDGET_H

#include <QGraphicsWidget>
#include "f2b.h"

class FaceWidget : public QGraphicsWidget
{
public:
    explicit FaceWidget(QGraphicsItem *parent = nullptr);

    void load(const Font::Face &face);

private:
};

#endif // FACEWIDGET_H
