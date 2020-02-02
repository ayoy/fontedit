#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"

#include <QPainter>
#include <QDebug>

#include <sstream>
#include <iomanip>

GlyphInfoWidget::GlyphInfoWidget(const Font::Glyph &glyph, char asciiCode, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    preview { Font::glyph_bitmap_preview(glyph) }
{
    std::stringstream stream;
    stream << "hex: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<uint>(asciiCode) << std::endl;
    stream << "dec: " << std::setw(3) << std::dec << static_cast<uint>(asciiCode) << std::endl;
    stream << "chr: '" << asciiCode << "'";
    description = QString::fromStdString(stream.str());
//    qDebug() << description;
}

void GlyphInfoWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->fillRect(rect(), QBrush(Qt::white));
    QPen pen(QBrush(Qt::darkGray), 0); // 0px wide???
    pen.setStyle(Qt::DashLine);
    painter->setPen(pen);
    painter->drawRect(rect());

    QFont f("Monaco");
    f.setStyleHint(QFont::TypeWriter);
    f.setPointSize(12);
    QRectF textRect(rect());
    textRect.setTop(6);
    textRect.setLeft(6);
    textRect.setWidth(textRect.width() - 6);
    textRect.setHeight(50);

    painter->setFont(f);
    painter->drawText(textRect, Qt::TextWordWrap, description);

    auto imageRect = preview.rect();
    imageRect.moveCenter(rect().center().toPoint());
    imageRect.moveBottom(rect().height() - 6);
    painter->setPen(QPen(QBrush(Qt::black), 1));
    painter->drawPixmap(imageRect, preview);
}
