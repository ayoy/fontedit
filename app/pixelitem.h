#ifndef PIXELITEM_H
#define PIXELITEM_H

#include <QGraphicsWidget>

class PixelItem : public QGraphicsWidget
{
    Q_OBJECT
public:
    Q_PROPERTY(bool set READ isSet WRITE setSet)

    PixelItem(QGraphicsItem *parent = nullptr);

    bool isSet() const { return m_set; }
    void setSet(bool value) { m_set = value; this->update(); }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:
    bool m_set;
};

#endif // PIXELITEM_H
