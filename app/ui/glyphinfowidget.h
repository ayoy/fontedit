#ifndef GLYPHINFOWIDGET_H
#define GLYPHINFOWIDGET_H

#include <QGraphicsWidget>
#include <QImage>
#include <QAction>
#include <f2b.h>

#include <optional>

class GlyphInfoWidget : public QGraphicsWidget
{
    Q_OBJECT

public:
    static constexpr auto cellMargin = 6.0;
    static constexpr auto descriptionHeight = 50.0;

    GlyphInfoWidget(const Font::Glyph& glyph, bool isExported,
                    unsigned char asciiCode, QSizeF imageSize,
                    Font::Margins margins = {}, QGraphicsItem *parent = nullptr);

    void updateGlyph(const Font::Glyph& glyph, std::optional<bool> isExported = {}, std::optional<Font::Margins> margins = {});

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void setIsExportedAdjustable(bool isEnabled);

signals:
    void isExportedChanged(bool isExported);

protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    const QString description_;
    const QSizeF imageSize_;
    bool isExportedAdjustable_;
    bool isExported_;
    QImage preview_;
    Font::Margins margins_;
    QAction toggleExportedAction_;
};

#endif // GLYPHINFOWIDGET_H
