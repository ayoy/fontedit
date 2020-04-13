#include "glyphinfowidget.h"
#include "f2b_qt_compat.h"
#include "common.h"

#include <QPainter>
#include <QDebug>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>

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

GlyphInfoWidget::GlyphInfoWidget(const Font::Glyph &glyph, std::size_t index, bool isExported,
                                 unsigned char asciiCode, QSizeF imageSize,
                                 Font::Margins margins, QGraphicsItem *parent) :
    QGraphicsWidget(parent),
    description_ { description(asciiCode) },
    imageSize_ { imageSize },
    isExportedAdjustable_ { true },
    isExported_ { isExported },
    preview_ { Font::glyph_preview_image(glyph, margins) },
    margins_ { margins },
    toggleExportedAction_ { QAction(tr("Exported")) },
    glyphIndex_ { index }
{
    toggleExportedAction_.setCheckable(true);
    toggleExportedAction_.setChecked(isExported_);

    connect(&toggleExportedAction_, &QAction::triggered, this, &GlyphInfoWidget::isExportedChanged);
}

void GlyphInfoWidget::setIsExportedAdjustable(bool isEnabled)
{
    isExportedAdjustable_ = isEnabled;
}

void GlyphInfoWidget::updateGlyph(std::optional<Font::Glyph> glyph, std::optional<bool> isExported, std::optional<Font::Margins> margins)
{
    if (isExported.has_value()) {
        isExported_ = isExported.value();
    }
    if (margins.has_value()) {
        margins_ = margins.value();
    }
    if (glyph.has_value()) {
        preview_ = Font::glyph_preview_image(glyph.value(), margins_);
    }
    update();
}

void GlyphInfoWidget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    if (!isExportedAdjustable_) {
        return;
    }
    QMenu menu;
    menu.addAction(&toggleExportedAction_);

    menu.exec(event->screenPos());
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

    if (!isExported_) {
        painter->setPen(QPen(QBrush(QColor(Color::inactiveText)), 0.5));
    }

    painter->setFont(f);
    painter->drawText(textRect, Qt::TextWordWrap, description_);


    // Preview rect (with margins)

    QRectF imageRect { QPointF(), imageSize_ };
    imageRect.moveCenter(rect().center().toPoint());
    imageRect.moveBottom(rect().height() - cellMargin);
    painter->fillRect(imageRect, Color::glyphMargin);


    // Glyph rect (margins removed)
    preview_.setColor(0, isExported_ ? Color::activeGlyph : Color::inactiveGlyph);

    QRectF imageSansMarginsRect = imageRect.marginsRemoved(QMarginsF(0, margins_.top, 0, margins_.bottom));
    painter->drawImage(imageSansMarginsRect, preview_, preview_.rect());


    // Glyph preview outline

    painter->setPen(QPen(QBrush(Qt::lightGray), 1));
    painter->drawRect(imageRect.marginsAdded(QMarginsF(0.5, 0.5, 0.5, 0.5)));
}
