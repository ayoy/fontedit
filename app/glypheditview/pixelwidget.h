#ifndef PIXELWIDGET_H
#define PIXELWIDGET_H

#include <QGraphicsWidget>

class PixelWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    Q_PROPERTY(bool set READ isSet WRITE setSet)

    explicit PixelWidget(QGraphicsItem *parent = nullptr);

    bool isSet() const { return set_; }
    void setSet(bool value) { set_ = value; this->update(); }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    bool set_ { false };
};

#endif // PIXELWIDGET_H
