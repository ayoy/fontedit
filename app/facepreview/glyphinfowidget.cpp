#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"

#include <QPainter>
#include <QDebug>

#include <sstream>
#include <iomanip>

static QString description(char asciiCode)
{
    std::stringstream stream;
    stream << "hex: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<uint>(asciiCode) << std::endl;
    stream << "dec: " << std::setw(3) << std::dec << static_cast<uint>(asciiCode) << std::endl;
    stream << "chr: '" << asciiCode << "'";
    return QString::fromStdString(stream.str());
}

GlyphInfoWidget::GlyphInfoWidget(const Font::Glyph &glyph, char asciiCode, QSizeF imageSize, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    description_ { description(asciiCode) },
    preview_ { Font::glyph_bitmap_preview(glyph) },
    imageSize_ { imageSize }
{
}

void GlyphInfoWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->fillRect(rect(), QBrush(Qt::white));
    QPen pen(QBrush(Qt::darkGray), 0.5);
    painter->setPen(pen);
    painter->drawRect(rect());

    QFont f("Monaco");
    f.setStyleHint(QFont::TypeWriter);
    f.setPixelSize(12);
    QRectF textRect(rect());
    textRect.setTop(cellMargin);
    textRect.setLeft(cellMargin);
    textRect.setWidth(textRect.width() - cellMargin);
    textRect.setHeight(descriptionHeight);

    painter->setFont(f);
    painter->drawText(textRect, Qt::TextWordWrap, description_);

    QRectF imageRect { QPointF(), imageSize_ };

    imageRect.moveCenter(rect().center().toPoint());
    imageRect.moveBottom(rect().height() - cellMargin);
    painter->setPen(QPen(QBrush(Qt::black), 1));
    painter->drawPixmap(imageRect, preview_, preview_.rect());
}
