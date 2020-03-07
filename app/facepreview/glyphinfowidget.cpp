#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"
#include "common.h"

#include <QPainter>
#include <QDebug>

#include <sstream>
#include <iomanip>

static QString description(unsigned char asciiCode)
{
    std::stringstream stream;
    stream << "hex: 0x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<uint>(asciiCode) << std::endl;
    stream << "dec: " << std::setw(3) << std::dec << static_cast<uint>(asciiCode) << std::endl;
    if (std::isprint(asciiCode)) {
        stream << "chr: '" << asciiCode << "'";
    }
    return QString::fromStdString(stream.str());
}

GlyphInfoWidget::GlyphInfoWidget(const Font::Glyph &glyph, unsigned char asciiCode, QSizeF imageSize,
                                 Font::Margins margins, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    description_ { description(asciiCode) },
    imageSize_ { imageSize },
    preview_ { Font::glyph_preview_pixmap(glyph, margins) },
    margins_ { margins }
{
}

void GlyphInfoWidget::updateGlyph(const Font::Glyph &glyph, std::optional<Font::Margins> margins)
{
    if (margins.has_value()) {
        margins_ = margins.value();
    }
    preview_ = Font::glyph_preview_pixmap(glyph, margins_);
    update();
}

void GlyphInfoWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->fillRect(rect(), QBrush(Qt::white));
    QPen pen(QBrush(Qt::darkGray), 0.5);
    painter->setPen(pen);
    painter->drawRect(rect());

    QFont f(consoleFontName);
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
    painter->setPen(QPen(QBrush(Qt::lightGray), 1));
    painter->drawRect(imageRect.marginsAdded(QMarginsF(0.5, 0.5, 0.5, 0.5)));
}
