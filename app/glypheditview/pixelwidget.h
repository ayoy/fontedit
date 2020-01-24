#ifndef PIXELWIDGET_H
#define PIXELWIDGET_H

#include <QGraphicsWidget>

class PixelWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    Q_PROPERTY(bool set READ isSet WRITE setSet)

    PixelWidget(QGraphicsItem *parent = nullptr);

    bool isSet() const { return m_set; }
    void setSet(bool value) { m_set = value; this->update(); }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    bool m_set;
};

#endif // PIXELWIDGET_H
