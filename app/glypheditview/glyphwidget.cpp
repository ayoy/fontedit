#include "glyphwidget.h"
#include "pixelwidget.h"
#include "focuswidget.h"
#include <QGraphicsGridLayout>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

GlyphWidget::GlyphWidget(uint8_t width, uint8_t height, qreal size, QGraphicsItem *parent)
    : QGraphicsWidget(parent)
    , m_layout { new QGraphicsGridLayout() }
    , m_focusWidget { nullptr }
    , m_focusedItem { nullptr }
    , m_width { width }
    , m_height { height }
    , m_pixelDimension { size }
{
    auto palette = QPalette(this->palette());
    palette.setColor(QPalette::WindowText, QColor(Qt::red));
    setPalette(palette);
    setupFontLayout(width, height);
    setFocusPolicy(Qt::ClickFocus);
}

void GlyphWidget::setupFontLayout(uint8_t width, uint8_t height)
{
    m_layout->setSpacing(0);

    for (auto x = 0; x < width; x++) {
        for (auto y = 0; y < height; y++) {
            PixelWidget *pixel = new PixelWidget();
            pixel->setPreferredSize(m_pixelDimension, m_pixelDimension);
            m_layout->addItem(pixel, y, x, 1, 1);
        }
    }

    setLayout(m_layout);
}

bool GlyphWidget::sceneEvent(QEvent *event)
{
    switch (event->type()) {
    case QActionEvent::KeyPress:
        if (auto keyEvent = dynamic_cast<QKeyEvent *>(event)) {
            handleKeyPress(keyEvent);
        }
        break;
    case QActionEvent::GraphicsSceneMousePress:
    case QActionEvent::GraphicsSceneMouseDoubleClick:
        if (auto mouseEvent = dynamic_cast<QGraphicsSceneMouseEvent *>(event)) {
            qreal leftMargin, topMargin;
            m_layout->getContentsMargins(&leftMargin, &topMargin, nullptr, nullptr);
            int row = (mouseEvent->pos().y() - topMargin)/m_pixelDimension;
            int col = (mouseEvent->pos().x() - leftMargin)/m_pixelDimension;
            qDebug() << mouseEvent << row << col;
            auto item = m_layout->itemAt(row, col);
            setFocusForItem(item, true);
            toggleItemSet(item);
        }
        break;
    default:
        break;
    }
    return QGraphicsWidget::sceneEvent(event);
}

void GlyphWidget::handleKeyPress(QKeyEvent *event) {
    if (m_focusedItem == nullptr) {
        return;
    }

    const auto pos = m_focusedItem->geometry().topLeft();
    qreal leftMargin, topMargin;
    m_layout->getContentsMargins(&leftMargin, &topMargin, nullptr, nullptr);

    const QPoint current((pos.x() - leftMargin) / m_pixelDimension,
                         (pos.y() - topMargin) / m_pixelDimension);
    QPoint updated(current);

    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_H:
        updated.setX(qMax(current.x() - 1, 0));
        moveFocus(current, updated);
        break;
    case Qt::Key_Right:
    case Qt::Key_L:
        updated.setX(qMin(current.x() + 1, m_width - 1));
        moveFocus(current, updated);
        break;
    case Qt::Key_Up:
    case Qt::Key_K:
        updated.setY(qMax(current.y() - 1, 0));
        moveFocus(current, updated);
        break;
    case Qt::Key_Down:
    case Qt::Key_J:
        updated.setY(qMin(current.y() + 1, m_height - 1));
        moveFocus(current, updated);
        break;
    case Qt::Key_Space:
        toggleItemSet(m_focusedItem);
        break;
    }
}

void GlyphWidget::moveFocus(const QPoint& from, const QPoint& to)
{
    if (to != from) {
        qDebug() << from << to;
        auto fromItem = m_layout->itemAt(from.y(), from.x());
        auto toItem = m_layout->itemAt(to.y(), to.x());
        setFocusForItem(fromItem, false);
        setFocusForItem(toItem, true);
    }
}

void GlyphWidget::setFocusForItem(QGraphicsLayoutItem *item, bool isFocused)
{
    if (m_focusWidget == nullptr) {
        m_focusWidget = new FocusWidget(this);
    }

    m_focusWidget->setFocus(item, isFocused);
    m_focusedItem = isFocused ? item : nullptr;
}

void GlyphWidget::toggleItemSet(QGraphicsLayoutItem *item)
{
    if (auto pixel = dynamic_cast<PixelWidget *>(item)) {
        pixel->setSet(!pixel->isSet());
    }
}
